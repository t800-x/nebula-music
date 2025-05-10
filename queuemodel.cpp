#include "queuemodel.h"

queuemodel::queuemodel(QObject *parent) : QAbstractListModel(parent)
{

}

bool queuemodel::moveRows(const QModelIndex &srcParent, int srcRow, int count, const QModelIndex &destParent, int destRow)
{
    if (count!=1 || srcRow==destRow) return false;
    beginMoveRows(srcParent, srcRow, srcRow, destParent, destRow > srcRow ? destRow+1 : destRow);
    m_songs.move(srcRow, destRow > srcRow ? destRow-1 : destRow);
    endMoveRows();
    return true;
}

void queuemodel::setSongs(const QVariantList &list)
{
    beginResetModel();
    m_songs.clear();
    for (auto v : list) {
        auto map = v.toMap();
        m_songs.append({ map["title"].toString(),
                        map["artist"].toString(),
                        map["path"].toString() }),
                        map["album"].toString(),
                        map["cover"].toString();
    }
    endResetModel();
}

void queuemodel::moveSong(int from, int to)
{
    moveRows(QModelIndex(), from, 1, QModelIndex(), to);
}

void queuemodel::clear()
{
    beginResetModel();
    m_songs.clear();
    endResetModel();
}

QVariant queuemodel::data(const QModelIndex &idx, int role) const
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
    default:         return {};
    }
}

QHash<int, QByteArray> queuemodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole]  = "title";
    roles[ArtistRole] = "artist";
    roles[PathRole]   = "path";
    roles[CoverRole]  = "cover";
    roles[AlbumRole]  = "album";
    return roles;
}

Qt::ItemFlags queuemodel::flags(const QModelIndex &index) const {
    auto f = QAbstractListModel::flags(index);
    // allow dragging items around
    if (index.isValid())
        f |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    return f;
}

Qt::DropActions queuemodel::supportedDropActions() const {
    // we only support moving rows
    return Qt::MoveAction;
}
