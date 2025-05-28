#ifndef SYNCEDLYRICSMODEL_H
#define SYNCEDLYRICSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "SyncedLyrics.h"

class SyncedLyricsModel : public QAbstractListModel
{
public:
    explicit SyncedLyricsModel(QObject *parent = nullptr);

    enum Roles {
        TextRole = Qt::UserRole + 1,
        Timestamp_msRole
    };

    bool append(QString text, unsigned int timestamp);
    int rowCount(const QModelIndex& ={}) const override { return m_lyrics.size(); }
    QVariant data(const QModelIndex& idx, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    void clear();

private:
    QList<SyncedLyrics> m_lyrics;
};

#endif // SYNCEDLYRICSMODEL_H
