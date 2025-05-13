#include "songmodel.h"

songmodel::songmodel(QObject *parent) : QAbstractListModel(parent)
{

}

bool songmodel::moveRows(const QModelIndex &srcParent, int srcRow, int count, const QModelIndex &destParent, int destRow)
{
    if (count!=1 || srcRow==destRow) return false;
    beginMoveRows(srcParent, srcRow, srcRow, destParent, destRow > srcRow ? destRow+1 : destRow);
    m_songs.move(srcRow, destRow > srcRow ? destRow-1 : destRow);
    endMoveRows();
    return true;
}

void songmodel::setSongs(const QVariantList &list)
{
    beginResetModel();
    m_songs.clear();
    for (auto v : list) {
        auto map = v.toMap();
        m_songs.append({ map["title"].toString(),
                        map["artist"].toString(),
                        map["path"].toString(),
                        map["album"].toString(),
                        map["cover"].toString()
        });

    }
    endResetModel();
}

void songmodel::clear()
{
    beginResetModel();
    m_songs.clear();
    endResetModel();
}

void songmodel::move(int from, int to)
{
    if(from >= 0 && from < rowCount() && to >= 0 && to < rowCount() && from != to) {
        if(from == to - 1) { // Allow item moving to the bottom
            to = from++;
        }

        beginResetModel();
        //        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        qInfo() << "model move from: " << from << " to: " << to;
        m_songs.move(from, to);
        //        endMoveRows();
        endResetModel();

    }
}

QVariant songmodel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_songs.size())
        return {};

    const auto &s = m_songs.at(idx.row());
    switch (role) {
    case TitleRole:  return s.title;
    case ArtistRole: return s.artist;
    case PathRole:   return s.path;
    case CoverRole:  return s.cover;
    case AlbumRole:  return s.album;
    case SyncedLyricsRole: return QVariant::fromValue(s.syncedlyrics);
    default:         return {};
    }
}

QHash<int, QByteArray> songmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole]  = "title";
    roles[ArtistRole] = "artist";
    roles[PathRole]   = "path";
    roles[CoverRole]  = "cover";
    roles[AlbumRole]  = "album";
    roles[SyncedLyricsRole] = "syncedlyrics";
    return roles;
}

bool songmodel::append(QString path, QString title, QString cover, QString artist, QString album, QList<Tag_reader::SyncedLyrics> syncedlyrics)
{
    beginResetModel();

    m_songs.append({
        title,
        artist,
        path,
        cover,
        album,
        syncedlyrics
    });

    endResetModel();

    return true;
}
