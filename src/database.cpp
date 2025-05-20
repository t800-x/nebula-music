#include "headers/database.h"
#include "models/songmodel.h"
#include "headers/tag_reader.h"

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
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

database::database(QObject *parent)
    : QObject{parent}
{}

bool database::create_tables()
{
    QSqlQuery exe;
    QStringList cmds;

    // 1. Artists table
    cmds << R"(
        CREATE TABLE IF NOT EXISTS artists (
            artist_id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name        TEXT    NOT NULL UNIQUE,
            bio         TEXT,
            image_url   TEXT
        )
    )";

    // 2. Albums table
    cmds << R"(
        CREATE TABLE IF NOT EXISTS albums (
            album_id    INTEGER PRIMARY KEY AUTOINCREMENT,
            artist_id   INTEGER NOT NULL REFERENCES artists(artist_id) ON DELETE CASCADE,
            title       TEXT    NOT NULL,
            release_date DATE,
            cover_url   TEXT,
            UNIQUE(artist_id, title)
        )
    )";

    // 3. Songs table (at least as many columns as your original library table)
    cmds << R"(
        CREATE TABLE IF NOT EXISTS songs (
            song_id      INTEGER PRIMARY KEY AUTOINCREMENT,
            album_id     INTEGER NOT NULL REFERENCES albums(album_id) ON DELETE CASCADE,
            track_number INTEGER NOT NULL,
            title        TEXT    NOT NULL,
            file_path    TEXT    NOT NULL,
            cover_path   TEXT,
            duration     INTEGER,
            synced_lyrics TEXT,
            lyrics TEXT,
            UNIQUE(album_id, track_number)
        )
    )";

    // Execute all commands in a single transaction
    if (!exe.exec("BEGIN TRANSACTION;")) {
        qDebug() << "Failed to begin transaction:" << exe.lastError().text();
        return false;
    }
    for (const auto& cmd : cmds) {
        if (!exe.exec(cmd)) {
            qDebug() << "Failed to execute DDL:" << exe.lastError().text();
            exe.exec("ROLLBACK;");
            return false;
        }
    }
    if (!exe.exec("COMMIT;")) {
        qDebug() << "Failed to commit transaction:" << exe.lastError().text();
        return false;
    }

    qDebug() << "All tables created or already exist";
    return true;
}

bool database::insertSongModel(const songmodel* model)
{
    // 1) must have exactly one song in the model
    if (model->rowCount() != 1) {
        qWarning() << "insertSongModel: model must contain exactly one row";
        return false;
    }

    // 2) read out the data from row 0
    const QModelIndex idx = model->index(0, 0);
    const QString filePath = model->data(idx, songmodel::PathRole).toString();
    const QString title    = model->data(idx, songmodel::TitleRole).toString();
    const QString artist   = model->data(idx, songmodel::ArtistRole).toString();
    const QString album    = model->data(idx, songmodel::AlbumRole).toString();
    const QString cover    = model->data(idx, songmodel::CoverRole).toString();

    // lyrics come back as a QList<SyncedLyrics>
    QVariant v = model->data(idx, songmodel::SyncedLyricsRole);
    auto synced = v.value<QList<SyncedLyrics>>();

    // 3) serialize synced‑lyrics to JSON text
    QJsonArray arr;
    for (const auto &ly : synced) {
        QJsonObject o;
        o["text"]         = ly.text;
        o["timestamp_ms"] = static_cast<int>(ly.timestamp_ms);
        arr.append(o);
    }
    const QByteArray syncedJson = QJsonDocument(arr).toJson(QJsonDocument::Compact);

    QSqlQuery q;
    if (!q.exec("BEGIN TRANSACTION;")) {
        qDebug() << "Failed to begin transaction:" << q.lastError().text();
        return false;
    }

    // 4) upsert artist
    q.prepare(R"(
        INSERT OR IGNORE INTO artists (name)
         VALUES (:name)
    )");
    q.bindValue(":name", artist);
    if (!q.exec()) {
        qDebug() << "Artist insert failed:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }
    // fetch artist_id
    q.prepare("SELECT artist_id FROM artists WHERE name = :name");
    q.bindValue(":name", artist);
    if (!q.exec() || !q.next()) {
        qDebug() << "Failed to fetch artist_id:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }
    const int artistId = q.value(0).toInt();

    // 5) upsert album
    q.prepare(R"(
        INSERT OR IGNORE INTO albums
          (artist_id, title, cover_url)
        VALUES
          (:artist_id, :title, :cover_url)
    )");
    q.bindValue(":artist_id", artistId);
    q.bindValue(":title",     album);
    q.bindValue(":cover_url", cover);
    if (!q.exec()) {
        qDebug() << "Album insert failed:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }
    // fetch album_id
    q.prepare(R"(
        SELECT album_id
          FROM albums
         WHERE artist_id = :artist_id
           AND title     = :title
    )");
    q.bindValue(":artist_id", artistId);
    q.bindValue(":title",     album);
    if (!q.exec() || !q.next()) {
        qDebug() << "Failed to fetch album_id:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }
    const int albumId = q.value(0).toInt();

    // 6) compute next track_number for this album
    q.prepare("SELECT IFNULL(MAX(track_number), 0) FROM songs WHERE album_id = :album_id");
    q.bindValue(":album_id", albumId);
    if (!q.exec() || !q.next()) {
        qDebug() << "Failed to compute next track_number:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }
    const int nextTrack = q.value(0).toInt() + 1;

    // 7) insert the song (use OR REPLACE if you want to overwrite)
    q.prepare(R"(
        INSERT INTO songs
          (album_id, track_number, title, file_path, cover_path, synced_lyrics)
        VALUES
          (:album_id, :track,       :title, :path,      :cover,      :synced)
    )");
    q.bindValue(":album_id", albumId);
    q.bindValue(":track",    nextTrack);
    q.bindValue(":title",    title);
    q.bindValue(":path",     filePath);
    q.bindValue(":cover",    cover);
    q.bindValue(":synced",   QString::fromUtf8(syncedJson));

    if (!q.exec()) {
        qDebug() << "Song insert failed:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }

    // 8) commit everything
    if (!q.exec("COMMIT;")) {
        qDebug() << "Failed to commit transaction:" << q.lastError().text();
        return false;
    }

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

    if (!create_tables())
    {
        return false;
    }

    all_songs = new songmodel(this);

    return true;
}

songmodel *database::get_all_songs()
{
    // 1. Construct your SQL query
    QString sql = R"(
        SELECT
            s.song_id,
            s.track_number,
            s.title           AS song_title,
            s.duration,
            s.file_path,
            s.cover_path,
            s.synced_lyrics,
            s.lyrics,
            a.title           AS album_title,
            ar.name           AS artist_name
        FROM songs AS s
        JOIN albums AS a ON s.album_id = a.album_id
        JOIN artists AS ar ON a.artist_id = ar.artist_id
        ORDER BY ar.name, a.release_date, s.track_number
    )";

    // 2. Prepare and execute the query
    QSqlQuery query;
    if (!query.exec(sql)) {
        qWarning() << "Query execution failed:" << query.lastError().text();
        return all_songs;
    }

    // 3. Iterate over results
    while (query.next())
    {
        // Access by column name
        QString songTitle = query.value("song_title").toString();
        QString filePath  = query.value("file_path").toString();
        QString coverPath = query.value("cover_path").toString();
        QString album     = query.value("album_title").toString();
        QString artist    = query.value("artist_name").toString();

        all_songs->append(filePath, songTitle, coverPath, artist, album);
    }

    return all_songs;
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

QStringList database::get_audio_files(const QString &directoryPath)
{
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
        QSqlQuery query;
        query.prepare(R"(
        SELECT file_path
          FROM songs
         WHERE file_path = :path
        )");
        query.bindValue(":path", file);

        if (!query.exec()) {
            qWarning() << "Database error checking song existence:"
                       << query.lastError().text();
            return false;
        }

        if (query.next())
        {
            qDebug() << "Skipping " << file;
            continue;
        }

        qDebug() << "Adding: " << file;
        Tag_reader reader;
        auto metadata = reader.parse_tags(file);
        insertSongModel(metadata) ? qDebug() << "Insert for " << file << " successful." : qDebug() << "Insert for " << file << " failed.";
    }

    return true;
}
