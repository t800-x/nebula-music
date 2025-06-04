#ifndef ALBUMMODEL_H
#define ALBUMMODEL_H

#include <QAbstractListModel>
#include <QObject>

class AlbumModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AlbumModel(QObject *parent = nullptr);

    enum Roles {
        AlbumIdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole
    };

    int rowCount(const QModelIndex& ={}) const override { return m_albums.size(); }
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void append(int albumId, QString title, QString artist);
    Q_INVOKABLE void logAlbums() const;
    void clear();

private:
    struct Album
    {
        int albumId;
        QString title, artist;
    };
    QList<Album> m_albums;
};

#endif // ALBUMMODEL_H
