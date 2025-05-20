#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "SyncedLyrics.h"

class songmodel : public QAbstractListModel
{
    Q_OBJECT
public:
    songmodel(QObject* parent=nullptr);

    enum Roles {
        TitleRole = Qt::UserRole+1,
        PathRole,
        CoverRole,
        ArtistRole,
        AlbumRole,
        SyncedLyricsRole
    };

    int rowCount(const QModelIndex& ={}) const override { return m_songs.size(); }
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool append(QString path, QString title = "", QString cover = "", QString artist = "", QString album = "", QList<SyncedLyrics> lyrics = {});
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertAtTop(QString path, QString title = "", QString cover = "", QString artist = "", QString album = "", QList<SyncedLyrics> lyrics = {});
    bool insert(int row,
                           const QString &path,
                           const QString &title,
                           const QString &cover,
                           const QString &artist,
                           const QString &album,
                const QList<SyncedLyrics> &lyrics = {});

    // support drag-move
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    Q_INVOKABLE void setSongs(const QVariantList& list);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void move(int from, int to);

private:
    struct Song
    {
        QString title, artist, path, cover, album;
        QList<SyncedLyrics> syncedlyrics;
    };
    QList<Song> m_songs;
};

Q_DECLARE_METATYPE(QList<SyncedLyrics>)




#endif // SONGMODEL_H
