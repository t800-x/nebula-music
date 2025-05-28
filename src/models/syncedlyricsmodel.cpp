#include "syncedlyricsmodel.h"

SyncedLyricsModel::SyncedLyricsModel(QObject *parent)
    : QAbstractListModel{parent}
{}

bool SyncedLyricsModel::append(QString text, unsigned int timestamp)
{
    beginResetModel();

    m_lyrics.append({
        text,
        timestamp
    });

    endResetModel();
    return true;
}

QVariant SyncedLyricsModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_lyrics.size())
        return {};

    const auto &s = m_lyrics.at(idx.row());
    switch (role) {
    case TextRole:  return s.text;
    case Timestamp_msRole: return s.timestamp_ms;
    default:         return {};
    }
}

QHash<int, QByteArray> SyncedLyricsModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[TextRole] = "text";
    roles[Timestamp_msRole] = "timestamp_ms";

    return roles;
}

void SyncedLyricsModel::clear()
{
    beginResetModel();
    m_lyrics.clear();
    endResetModel();
}
