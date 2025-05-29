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
