#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "tag_reader.h"
#include <vector>

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
    bool append(QString path, QString title = "", QString cover = "", QString artist = "", QString album = "", QList<Tag_reader::SyncedLyrics> lyrics = {});

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
        QList<Tag_reader::SyncedLyrics> syncedlyrics;
    };
    QList<Song> m_songs;
};

Q_DECLARE_METATYPE(QList<Tag_reader::SyncedLyrics>)




#endif // SONGMODEL_H
