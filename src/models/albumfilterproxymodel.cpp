#include "albumfilterproxymodel.h"
#include "albummodel.h"

AlbumFilterProxyModel::AlbumFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    // Default: case‑insensitive substring matching
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void AlbumFilterProxyModel::setFilterText(const QString &text)
{
    if (m_filterText == text)
        return;
    m_filterText = text;
    invalidateFilter();            // re‑run filterAcceptsRow()
    emit filterTextChanged();
}

void AlbumFilterProxyModel::setSortRole(int role)
{
    if (role == sortRole())
        return;
    QSortFilterProxyModel::setSortRole(role);
    invalidate();                  // re‑run sort + filter
    emit sortRoleChanged();
}

bool AlbumFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (m_filterText.isEmpty())
        return true;

    const QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    const QString title = sourceModel()->data(idx, AlbumModel::TitleRole).toString();
    const QString artist = sourceModel()->data(idx, AlbumModel::ArtistRole).toString();

    return title.contains(m_filterText, filterCaseSensitivity())
           || artist.contains(m_filterText, filterCaseSensitivity());
}

bool AlbumFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
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



