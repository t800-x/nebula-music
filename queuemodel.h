#ifndef QUEUEMODEL_H
#define QUEUEMODEL_H

#include <QObject>
#include <QAbstractListModel>

class queuemodel : public QAbstractListModel
{
    Q_OBJECT
public:
    queuemodel(QObject* parent=nullptr);

    enum Roles {
        TitleRole = Qt::UserRole+1,
        PathRole,
        CoverRole,
        ArtistRole,
        AlbumRole
    };

    int rowCount(const QModelIndex& ={}) const override { return m_songs.size(); }
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;

    // support drag-move
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    Q_INVOKABLE void setSongs(const QVariantList& list);
    Q_INVOKABLE void moveSong(int from, int to);
    Q_INVOKABLE void clear();

private:
    struct Song { QString title, artist, path, cover, album; };
    QList<Song> m_songs;

};




#endif // QUEUEMODEL_H
