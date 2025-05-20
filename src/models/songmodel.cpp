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

bool songmodel::append(QString path, QString title, QString cover, QString artist, QString album, QList<SyncedLyrics> syncedlyrics)
{
    beginResetModel();

    // qDebug() << "Begin reset model";
    m_songs.append({
        title,
        artist,
        path,
        cover,
        album,
        syncedlyrics
    });

    // qDebug() << "Success";

    endResetModel();

    // qDebug() << "End reset model";

    return true;
}

bool songmodel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || (row + count) > m_songs.size() || count <= 0)
        return false;

    beginRemoveRows(parent, row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_songs.removeAt(row);  // always remove at 'row' since the list shifts
    endRemoveRows();

    return true;
}

bool songmodel::insertAtTop(QString path, QString title, QString cover, QString artist, QString album, QList<SyncedLyrics> lyrics)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_songs.prepend({
        title,
        artist,
        path,
        cover,
        album,
        lyrics
    });
    endInsertRows();

    return true;
}

bool songmodel::insert(int row,
                       const QString &path,
                       const QString &title,
                       const QString &cover,
                       const QString &artist,
                       const QString &album,
                       const QList<SyncedLyrics> &lyrics)
{
    // Validate insertion index
    int count = m_songs.size();
    if (row < 0 || row > count)
        return false;

    beginInsertRows(QModelIndex(), row, row);
    // Insert an entry at the given position
    m_songs.insert(row, Song{title, artist, path, cover, album, lyrics});
    endInsertRows();

    return true;
}

