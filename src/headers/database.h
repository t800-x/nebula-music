#ifndef DATABASE_H
#define DATABASE_H

#include "models/songmodel.h"
#include "models/songfilterproxymodel.h"
#include "models/albummodel.h"
#include "models/albumfilterproxymodel.h"

#include <QObject>
#include <QSqlDatabase>
#include <qsqlquery.h>
#include <QVariantList>
#include <QVariantMap>
#include <QDirIterator>
#include <QStringList>
#include <QMediaMetaData>
#include <QMediaPlayer>

class database : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged FINAL)
public:
    explicit database(QObject *parent = nullptr);
    void checkDeleted();
    void checkForAddedFiles();
    bool add_to_library(const QStringList &files);

    bool ready() const;
    void setReady(bool newReady);

private:
    QSqlDatabase db;
    QVariantList table;

    songmodel* all_songs;
    AlbumModel* all_albums;

    SongFilterProxyModel* songs_data;
    AlbumFilterProxyModel* albums_data;

    bool create_tables();
    QStringList get_audio_files(const QString &directoryPath);
    // bool insertSongModel(const songmodel* model);
    bool insertSongModel(const songmodel* model) {
        return insertSongModel_db(model, this->db);
    }
    bool insertSongModel_db(const songmodel* model, QSqlDatabase &db);


    QString m_databasePath;
    QSqlDatabase getThreadLocalConnection();

    bool m_ready;

public slots:
    bool init();
    SongFilterProxyModel* get_songs_data() {return songs_data;}
    AlbumFilterProxyModel* get_albums_data() {get_all_albums(); return albums_data;}
    void setFilter(QString filter) {songs_data->setFilterText(filter);}
    SongFilterProxyModel* get_album_songs(int albumId, QObject* parent);
    void get_all_albums();
    void get_all_songs();

    QUrl getCover(int songId);
    QUrl getAlbumCover(int albumId);
    void logAlbums() const {all_albums->logAlbums();}

signals:
    void table_changed();
    void readyChanged();
};

#endif // DATABASE_H
