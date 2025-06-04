#include "albummodel.h"

AlbumModel::AlbumModel(QObject *parent)
    : QAbstractListModel{parent}
{}

QVariant AlbumModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_albums.size())
        return {};

    const auto &a = m_albums.at(idx.row());

    switch (role)
    {
        case AlbumIdRole: return a.albumId;
        case TitleRole: return a.title;
        case ArtistRole: return a.artist;
        default: return {};
    }
}

QHash<int, QByteArray> AlbumModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[AlbumIdRole] = "albumId";
    roles[TitleRole] = "title";
    roles[ArtistRole] = "artist";
    return roles;
}

void AlbumModel::append(int albumId, QString title, QString artist)
{
    beginResetModel();

    m_albums.append({
        albumId,
        title,
        artist
    });

    endResetModel();
}

void AlbumModel::logAlbums() const
{
    qDebug() << "========== AlbumModel Contents ==========";
    for (int i = 0; i < m_albums.size(); ++i) {
        const Album &a = m_albums.at(i);
        qDebug()
            << "Row" << i
            << ":"
            << "ID =" << a.albumId
            << ", Title =" << a.title
            << ", Artist =" << a.artist;
    }
    qDebug() << "==========================================";
}

void AlbumModel::clear()
{
    beginResetModel();
    m_albums.clear();
    endResetModel();
}
