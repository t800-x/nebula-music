#ifndef SYNCEDLYRICS_H
#define SYNCEDLYRICS_H

#include <QString>

struct SyncedLyrics {
    QString text;
    unsigned int timestamp_ms;

    SyncedLyrics(const QString& t, unsigned int ts)
        : text(t), timestamp_ms(ts)
    {}
};

#endif // SYNCEDLYRICS_H
