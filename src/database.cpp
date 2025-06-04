#include "headers/database.h"
#include "models/songmodel.h"
#include "headers/tag_reader.h"

#include <QDirIterator>
#include <QEventLoop>
#include <QFile>
#include <QObject>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>
#include <QThreadPool>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>
#include <QtConcurrent>
#include <QElapsedTimer>


database::database(QObject *parent)
    : QObject{parent}
{
    setReady(false);
}

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
            duration     INTEGER,
            UNIQUE(album_id, track_number)
        )
    )";

    cmds << R"(
        CREATE TABLE IF NOT EXISTS covers (
            cover_id INTEGER PRIMARY KEY,
            cover_path TEXT
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

    qDebug() << "All tables created or already exist.";
    return true;
}

bool database::insertSongModel_db(const songmodel* model, QSqlDatabase &db)
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

    QSqlQuery q(db);
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
          (artist_id, title)
        VALUES
          (:artist_id, :title )
    )");
    q.bindValue(":artist_id", artistId);
    q.bindValue(":title",     album);
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
          (album_id, track_number, title, file_path)
        VALUES
          (:album_id, :track,       :title, :path)
    )");
    q.bindValue(":album_id", albumId);
    q.bindValue(":track",    nextTrack);
    q.bindValue(":title",    title);
    q.bindValue(":path",     filePath);

    if (!q.exec()) {
        qDebug() << "Song insert failed:" << q.lastError().text();
        q.exec("ROLLBACK;");
        return false;
    }

    //Get the cover
    QString coverBase;
    if (!album.isEmpty()) coverBase = QString::number(albumId);
    else coverBase = "_" + QString::number(q.lastInsertId().toInt());
    Tag_reader reader;
    QString cover_path = reader.get_cover(filePath, coverBase);
    q.prepare(R"(
        INSERT OR IGNORE INTO covers
            (cover_id, cover_path)
        VALUES
            (:cover_id, :cover_path)
    )");

    q.bindValue(":cover_id", albumId);
    q.bindValue(":cover_path", cover_path);

    if (!q.exec()) {
        qDebug() << "Cover insert failed:" << q.lastError().text();
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

    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString appFolderPath = homeDir + "/.nebula";

    QString dbPath = appFolderPath + "/library.db";
    m_databasePath = dbPath; // Store for thread connections

    QDir dir(appFolderPath);
    if (!dir.exists()) {
        if (!QDir().mkpath(appFolderPath)) {
            qDebug() << "Failed to create base directory:" << appFolderPath;
            return {};
        }
    }

    db.setDatabaseName(dbPath);

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
    all_albums = new AlbumModel(this);
    songs_data = new SongFilterProxyModel(this);
    albums_data = new AlbumFilterProxyModel(this);

    songs_data -> setSourceModel(all_songs);
    albums_data -> setSourceModel(all_albums);

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [=]() {
        // This runs on the main thread after background task finishes
        qDebug() << "Background task done. Executing follow-up on main thread.";
        get_all_songs();
        get_all_albums();
        setReady(true);

        watcher->deleteLater();
    });


    QFuture<void> future = QtConcurrent::run([=]() {
        QElapsedTimer timer;
        timer.start();
        checkDeleted();
        checkForAddedFiles();
        qDebug() << "Time Taken: " << timer.elapsed() << " ms";
    });

    watcher->setFuture(future);

    return true;
}

SongFilterProxyModel *database::get_album_songs(int albumId, QObject* parent)
{
    QString query(R"(
        SELECT
            s.song_id,
            s.track_number,
            s.title           AS song_title,
            s.duration,
            s.file_path,
            a.title           AS album_title,
            ar.name           AS artist_name
        FROM songs AS s
        JOIN albums AS a ON s.album_id = a.album_id
        JOIN artists AS ar ON a.artist_id = ar.artist_id
        WHERE s.album_id = :albumId
        ORDER BY ar.name, a.release_date, s.track_number
    )");

    QSqlQuery exe;
    exe.prepare(query);
    exe.bindValue(":albumId", albumId);

    if (!exe.exec()) {
        qWarning() << "Query execution failed:" << exe.lastError().text();
        return {};
    }

    songmodel* result = new songmodel(parent);

    while (exe.next())
    {
        auto title = exe.value("song_title").toString();
        auto artist = exe.value("artist_name").toString();
        auto path = exe.value("file_path").toString();
        auto album = exe.value("album_title").toString();
        auto songId = exe.value("song_id").toInt();

        qDebug() << artist + "-" + title;

        result->append(path, songId, title, artist, album);
    }

    SongFilterProxyModel* proxy = new SongFilterProxyModel(parent);
    proxy->setSourceModel(result);


    return proxy;
}

void database::get_all_songs()
{
    // 1. Construct your SQL query
    all_songs->clear();
    QString sql = R"(
        SELECT
            s.song_id,
            s.track_number,
            s.title           AS song_title,
            s.duration,
            s.file_path,
            a.title           AS album_title,
            ar.name           AS artist_name
        FROM songs AS s
        JOIN albums AS a ON s.album_id = a.album_id
        JOIN artists AS ar ON a.artist_id = ar.artist_id
        ORDER BY ar.name, a.release_date, s.track_number
    )";

    // 2. Prepare and execute the query
    auto db_conn = getThreadLocalConnection();
    QSqlQuery query(db_conn);
    if (!query.exec(sql)) {
        qWarning() << "Query execution failed:" << query.lastError().text();
        return;
    }

    // 3. Iterate over results
    while (query.next())
    {
        // Access by column name
        QString songTitle = query.value("song_title").toString();
        QString filePath  = query.value("file_path").toString();
        QString album     = query.value("album_title").toString();
        QString artist    = query.value("artist_name").toString();
        int songId        = query.value("song_id").toInt();

        all_songs->append(filePath, songId, songTitle, artist, album);
    }
}


void database::get_all_albums()
{
    all_albums->clear();

    QString query =R"(
        SELECT
            albums.album_id,
            albums.title AS album_title,
            artists.name AS artist_name
        FROM albums
        JOIN artists ON albums.artist_id = artists.artist_id;
    )";

    QSqlQuery exe(getThreadLocalConnection());

    if (!exe.exec(query)) {
        qWarning() << "Query failed:" << exe.lastError().text();
        return;
    }

    while (exe.next())
    {
        auto albumId = exe.value("album_id").toInt();
        auto title = exe.value("album_title").toString();
        auto artist = exe.value("artist_name").toString();

        all_albums->append(albumId, title, artist);
    }
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

bool database::add_to_library(const QStringList &files)
{
    QSqlDatabase db_conn = getThreadLocalConnection();

    for (const QString &file : files)
    {
        QSqlQuery query(db_conn);
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

        if (query.next()) {
            qDebug() << "Skipping (already in DB):" << file;
            continue;
        }

        qDebug() << "Adding:" << file;
        Tag_reader reader;
        auto metadata = reader.parse_tags(file);
        bool ok = insertSongModel(metadata);
        if (ok) {
            qDebug() << "Insert for" << file << "successful.";
        } else {
            qDebug() << "Insert for" << file << "failed.";
        }
    }

    return true;
}


QUrl database::getCover(int songId)
{
    QSqlQuery q;
    q.prepare(R"(
    SELECT album_id
      FROM songs
     WHERE song_id = :songId
    )");
    q.bindValue(":songId", songId);

    if (!q.exec()) {
        qWarning() << "Failed to fetch album_id:" << q.lastError().text();
        return {};
    }

    int albumId = -1;
    if (q.next()) {
        albumId = q.value(0).toInt();
    }

    q.prepare(R"(
        SELECT cover_path
            FROM covers
        WHERE cover_id = :album_id
    )");
    q.bindValue(":album_id", albumId);

    if (!q.exec()) {
        qWarning() << "Failed to fetch cover path (song id search):" << q.lastError().text();
        return {};
    }

    if(q.next())
    {
        return QUrl::fromLocalFile(q.value(0).toString());
    }
    return {};
}

QUrl database::getAlbumCover(int albumId)
{
    QSqlQuery q;
    q.prepare(R"(
        SELECT cover_path
            FROM covers
        WHERE cover_id = :album_id
    )");
    q.bindValue(":album_id", albumId);

    if (!q.exec()) {
        qWarning() << "Failed to fetch cover path (album id search):" << q.lastError().text();
        return {};
    }

    if(q.next())
    {
        return QUrl::fromLocalFile(q.value(0).toString());
    }

    return {};
}

void database::checkDeleted()
{
    QSqlDatabase db_conn = getThreadLocalConnection();

    // 1) Fetch all songs (song_id, file_path, album_id) into a local list
    struct SongInfo { int songId; QString filePath; int albumId; };
    QVector<SongInfo> allSongs;
    {
        QSqlQuery fetchQuery(db_conn);
        fetchQuery.prepare("SELECT song_id, file_path, album_id FROM songs");
        while (fetchQuery.next()) {
            allSongs.append({
                fetchQuery.value(0).toInt(),
                fetchQuery.value(1).toString(),
                fetchQuery.value(2).toInt()
            });
        }
    }

    // 2) Use 50 parallel threads to check which files no longer exist on disk
    QThreadPool::globalInstance()->setMaxThreadCount(50);
    auto missingFuture = QtConcurrent::filtered(
        allSongs,
        [](const SongInfo &info) {
            return !QFile::exists(info.filePath);
        }
        );
    missingFuture.waitForFinished();
    QVector<SongInfo> missingSongs = missingFuture.results();

    if (missingSongs.isEmpty()) {
        // Nothing to delete at the song level—still need to check orphaned albums/artists
        // but skip straight to step 4 below.
    }

    QSet<int> affectedAlbums;
    QSqlQuery delQuery;
    if (!delQuery.exec(QStringLiteral("BEGIN TRANSACTION;"))) {
        qDebug() << "refresh(): Failed to BEGIN TRANSACTION:" << delQuery.lastError().text();
        return;
    }

    // 3) Delete each missing song and collect its album_id
    for (const SongInfo &s : qAsConst(missingSongs)) {
        affectedAlbums.insert(s.albumId);

        delQuery.prepare(QStringLiteral("DELETE FROM songs WHERE song_id = :sid;"));
        delQuery.bindValue(QStringLiteral(":sid"), s.songId);

        if (!delQuery.exec()) {
            qDebug() << "refresh(): Failed to delete song_id"
                     << s.songId << ":" << delQuery.lastError().text();
            delQuery.exec(QStringLiteral("ROLLBACK;"));
            return;
        }
    }

    // 4) Find albums (from affectedAlbums) that now have zero songs
    QSet<int> albumsToDelete;
    for (int albumId : qAsConst(affectedAlbums)) {
        delQuery.prepare(QStringLiteral(
            "SELECT 1 "
            "FROM songs "
            "WHERE album_id = :aid "
            "LIMIT 1;"
            ));
        delQuery.bindValue(QStringLiteral(":aid"), albumId);

        if (!delQuery.exec()) {
            qDebug() << "refresh(): Failed to check songs for album_id"
                     << albumId << ":" << delQuery.lastError().text();
            delQuery.exec(QStringLiteral("ROLLBACK;"));
            return;
        }

        if (!delQuery.next()) {
            // No songs remain under this album
            albumsToDelete.insert(albumId);
        }
    }

    // 5) Delete orphaned albums and their covers
    for (int albumId : qAsConst(albumsToDelete)) {
        delQuery.prepare(QStringLiteral("DELETE FROM albums WHERE album_id = :aid;"));
        delQuery.bindValue(QStringLiteral(":aid"), albumId);
        if (!delQuery.exec()) {
            qDebug() << "refresh(): Failed to delete album_id"
                     << albumId << ":" << delQuery.lastError().text();
            delQuery.exec(QStringLiteral("ROLLBACK;"));
            return;
        }

        delQuery.prepare(QStringLiteral("DELETE FROM covers WHERE cover_id = :aid;"));
        delQuery.bindValue(QStringLiteral(":aid"), albumId);
        if (!delQuery.exec()) {
            qDebug() << "refresh(): Failed to delete cover for album_id"
                     << albumId << ":" << delQuery.lastError().text();
            delQuery.exec(QStringLiteral("ROLLBACK;"));
            return;
        }
    }

    // 6) Find artists that now have zero albums
    QSet<int> artistsToDelete;
    {
        QSqlQuery fetchArtists(QStringLiteral("SELECT artist_id FROM artists;"));
        while (fetchArtists.next()) {
            int artistId = fetchArtists.value(0).toInt();

            delQuery.prepare(QStringLiteral(
                "SELECT 1 "
                "FROM albums "
                "WHERE artist_id = :arid "
                "LIMIT 1;"
                ));
            delQuery.bindValue(QStringLiteral(":arid"), artistId);

            if (!delQuery.exec()) {
                qDebug() << "refresh(): Failed to check albums for artist_id"
                         << artistId << ":" << delQuery.lastError().text();
                delQuery.exec(QStringLiteral("ROLLBACK;"));
                return;
            }

            if (!delQuery.next()) {
                // No albums remain under this artist
                artistsToDelete.insert(artistId);
            }
        }
    }

    // 7) Delete orphaned artists
    for (int artistId : qAsConst(artistsToDelete)) {
        delQuery.prepare(QStringLiteral("DELETE FROM artists WHERE artist_id = :arid;"));
        delQuery.bindValue(QStringLiteral(":arid"), artistId);
        if (!delQuery.exec()) {
            qDebug() << "refresh(): Failed to delete artist_id"
                     << artistId << ":" << delQuery.lastError().text();
            delQuery.exec(QStringLiteral("ROLLBACK;"));
            return;
        }
    }

    // 8) Commit all deletions
    if (!delQuery.exec(QStringLiteral("COMMIT;"))) {
        qDebug() << "refresh(): Failed to COMMIT transaction:" << delQuery.lastError().text();
        delQuery.exec(QStringLiteral("ROLLBACK;"));
        return;
    }
}

void database::checkForAddedFiles()
{
    QSqlDatabase db_conn = getThreadLocalConnection();

    // 1) Read list of media folders from "mediafolders.txt"
    QStringList folderPaths;
    QFile file("mediafolders.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "checkForAddedFiles: Unable to open mediafolders.txt";
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty())
            folderPaths << line;
    }
    file.close();

    if (folderPaths.isEmpty()) {
        // No folders to scan
        return;
    }

    // 2) Limit to 50 parallel tasks
    QThreadPool::globalInstance()->setMaxThreadCount(50);

    // 3) For each folder, collect all audio files recursively
    auto folderToFilesFuture = QtConcurrent::mapped(folderPaths, [](const QString &folder) {
        QStringList extensions = { "*.mp3", "*.wav", "*.flac", "*.aac", "*.ogg", "*.m4a" };
        QStringList filesInFolder;
        QDirIterator it(folder, extensions, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            filesInFolder << it.next();
        }
        return filesInFolder;
    });

    folderToFilesFuture.waitForFinished();
    QList<QStringList> listOfFileLists = folderToFilesFuture.results();

    // 4) Flatten into a single list of all files found on disk
    QStringList allDiscoveredFiles;
    for (const QStringList &sublist : listOfFileLists) {
        allDiscoveredFiles += sublist;
    }

    // 5) Fetch all existing file paths from the database
    QSet<QString> existingPaths;
    {
        QSqlQuery query(db_conn);
        query.prepare("SELECT file_path FROM songs");
        while (query.next()) {
            existingPaths.insert(query.value(0).toString());
        }
    }

    // 6) Compare and collect any files that are on disk but not in the DB
    QStringList newlyAddedFiles;
    for (const QString &path : allDiscoveredFiles) {
        if (!existingPaths.contains(path)) {
            newlyAddedFiles << path;
        }
    }

    add_to_library(newlyAddedFiles);
}

QSqlDatabase database::getThreadLocalConnection()
{
    QString connectionName = "thread_" + QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));

    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(m_databasePath);
    if (!db.open()) {
        qWarning() << "Failed to open database for thread:" << db.lastError();
    }
    return db;
}

bool database::ready() const
{
    return m_ready;
}

void database::setReady(bool newReady)
{
    if (m_ready == newReady)
        return;
    m_ready = newReady;
    emit readyChanged();
}
