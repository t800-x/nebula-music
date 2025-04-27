#include "database.h"

#include <QObject>
#include <QSqlDatabase>
#include <qsqlquery.h>
#include <QVariantList>
#include <QVariantMap>
#include <QSqlError>
#include <QDirIterator>
#include <QStringList>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QEventLoop>

database::database(QObject *parent)
    : QObject{parent}
{}

bool database::create_table()
{
    QSqlQuery exe;
    QString cmd = R"(
        CREATE TABLE IF NOT EXISTS library (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            path TEXT NOT NULL,
            cover TEXT,
            title TEXT,
            artist TEXT,
            album TEXT
        )
    )";

    if(!exe.exec(cmd))
    {
        qDebug() << "Failed to create table: " << exe.lastError().text();
        return false;
    }

    qDebug() << "Table created or already exists";
    return true;
}

bool database::init()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("library.db");

    if(!db.open())
    {
        qDebug() << "Error opening databse: " << db.lastError().text();
        return false;
    }

    if (!create_table())
    {
        return false;
    }

    return true;
}

QVariantList database::query_db(QString query)
{
    QVariantList result;
    QSqlQuery exe(query);

    while (exe.next()) {
        QVariantMap entry;
        entry["id"] = exe.value("id");
        entry["path"] = exe.value("path");
        entry["cover"] = exe.value("cover");
        entry["title"] = exe.value("title");
        entry["artist"] = exe.value("artist");
        entry["album"] = exe.value("album");
        result.append(entry);
    }

    return result;
}

QVariantList database::get_table()
{
    table = query_db("SELECT * FROM library");

    return table;
}

QStringList database::get_audio_files(const QString &directoryPath) {
    QStringList audioFiles;
    QStringList audioExtensions = {"*.mp3", "*.wav", "*.flac", "*.aac", "*.ogg", "*.m4a"};

    QDirIterator it(directoryPath, audioExtensions, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        audioFiles << it.next();
    }

    return audioFiles;
}

bool database::add_to_library(QString path)
{
    QStringList files = get_audio_files(path);

    for (const QString &file : files)
    {
        // Check if file already exists in database
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT id FROM library WHERE path = :path");
        checkQuery.bindValue(":path", file);

        if (!checkQuery.exec()) {
            qDebug() << "Database error:" << checkQuery.lastError().text();
            continue;
        }

        if (checkQuery.next()) {
            qDebug() << "Skipping existing file:" << file;
            continue;
        }


        QMediaPlayer *mediaPlayer = new QMediaPlayer(this);
        QEventLoop loop;
        bool metadataLoaded = false;

        // Connect status change handler
        QObject::connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged,
                         [&](QMediaPlayer::MediaStatus status) {
                             if (status == QMediaPlayer::LoadedMedia) {
                                 metadataLoaded = true;
                                 loop.quit();
                             } else if (status == QMediaPlayer::InvalidMedia) {
                                 qDebug() << "Error loading media:" << file;
                                 loop.quit();
                             }
                         });

        mediaPlayer->setSource(QUrl::fromLocalFile(file));

        // Wait for metadata to load or error
        loop.exec();

        if (!metadataLoaded) {
            qDebug() << "Skipping file due to load error:" << file;
            mediaPlayer->deleteLater();
            continue;
        }

        // Extract metadata
        QMediaMetaData metadata = mediaPlayer->metaData();
        QString title = metadata.value(QMediaMetaData::Title).toString();
        QString artist = metadata.value(QMediaMetaData::AlbumArtist).toString();
        QString album = metadata.value(QMediaMetaData::AlbumTitle).toString();

        // Fallback to filename if title is empty
        if (title.isEmpty()) {
            title = QFileInfo(file).baseName();
        }

        // Insert into database
        QSqlQuery query;
        query.prepare("INSERT INTO library (path, title, artist, album, cover) "
                      "VALUES (:path, :title, :artist, :album, :cover)");
        query.bindValue(":path", file);
        query.bindValue(":title", title);
        query.bindValue(":artist", artist);
        query.bindValue(":album", album);
        query.bindValue(":cover", ""); // Add cover extraction logic if needed

        if (!query.exec()) {
            qDebug() << "Insert error:" << query.lastError().text();
        } else {
            qDebug() << "Added:" << file;
        }

        mediaPlayer->deleteLater();
    }

    return true;
}


