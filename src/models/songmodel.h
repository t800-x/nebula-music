#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <QObject>
#include <QAbstractListModel>

class songmodel : public QAbstractListModel
{
    Q_OBJECT
public:
    songmodel(QObject* parent=nullptr);

    enum Roles {
        TitleRole = Qt::UserRole+1,
        PathRole,
        ArtistRole,
        AlbumRole,
        SongIdRole
    };

    int rowCount(const QModelIndex& ={}) const override { return m_songs.size(); }
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool append(QString path, int songId,  QString title = "", QString artist = "", QString album = "");
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertAtTop(QString path, int songId, QString title = "", QString artist = "", QString album = "");
    bool insert(int row,
                           QString path,
                           QString title,
                           QString artist,
                           QString album,
                           int songId);

    // support drag-move
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    Q_INVOKABLE void setSongs(const QVariantList& list);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void move(int from, int to);

private:
    struct Song
    {
        QString title, artist, path, album;
        int songId;
    };
    QList<Song> m_songs;
};




#endif // SONGMODEL_H
