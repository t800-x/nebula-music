#include "SongFilterProxyModel.h"
#include "songmodel.h"

SongFilterProxyModel::SongFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    // Default: case‑insensitive substring matching
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void SongFilterProxyModel::setFilterText(const QString &text)
{
    if (m_filterText == text)
        return;
    m_filterText = text;
    invalidateFilter();            // re‑run filterAcceptsRow()
    emit filterTextChanged();
}

void SongFilterProxyModel::setSortRole(int role)
{
    if (role == sortRole())
        return;
    QSortFilterProxyModel::setSortRole(role);
    invalidate();                  // re‑run sort + filter
    emit sortRoleChanged();
}

songmodel *SongFilterProxyModel::toSongModel(QObject *parent)
{
    // 1) Create the new model
    songmodel* filtered = new songmodel(parent);

    // 2) For each row in the proxy, pull out all the data
    const auto roles = sourceModel()->roleNames(); // same roles as SongModel
    for (int row = 0; row < rowCount(); ++row) {
        QModelIndex pidx = index(row, 0);

        // Fetch each role’s data
        QString path, title, artist, album;
        int songId;

        // You know your Roles enum, so:
        path   = data(pidx, songmodel::PathRole).   toString();
        songId = data(pidx, songmodel::SongIdRole). toInt();
        title  = data(pidx, songmodel::TitleRole).  toString();
        artist = data(pidx, songmodel::ArtistRole). toString();
        album  = data(pidx, songmodel::AlbumRole).  toString();

        // 3) Append into the new SongModel
        filtered->append(path, songId, title, artist, album);
    }

    return filtered;
}

bool SongFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterText.isEmpty())
        return true;

    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    // Pull out each field
    const QVariant title  = sourceModel()->data(idx, songmodel::TitleRole);
    const QVariant artist = sourceModel()->data(idx, songmodel::ArtistRole);
    const QVariant album  = sourceModel()->data(idx, songmodel::AlbumRole);

    // Make them all strings (invalid QVariant becomes empty string)
    const QString sTitle  = title .toString();
    const QString sArtist = artist.toString();
    const QString sAlbum  = album .toString();

    // Check any of them for the filter text
    return sTitle .contains(m_filterText, filterCaseSensitivity())
           || sArtist.contains(m_filterText, filterCaseSensitivity())
           || sAlbum .contains(m_filterText, filterCaseSensitivity());
}


bool SongFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // Compare based on the currently‑set sortRole()
    QVariant lhs = sourceModel()->data(left, sortRole());
    QVariant rhs = sourceModel()->data(right, sortRole());

    // Handle missing or non‑string types if needed
    if (!lhs.isValid()) return true;
    if (!rhs.isValid()) return false;

    // If it’s a string, do a locale‑aware compare
    return lhs.toString().localeAwareCompare(rhs.toString()) < 0;
}
