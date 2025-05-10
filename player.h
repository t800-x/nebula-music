#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QVariantList>
#include <QVariantMap>
#include "queuemodel.h"

class player : public QObject
{
    Q_OBJECT
public:
    explicit player(QObject *parent = nullptr);

public slots:
    void init(queuemodel *model);
    void play(int index);
    QString get_title();
    QString get_artist();
    int state();
    qint64 get_duration();
    qint64 get_position();
    void set_position(qint64 position);
    void plause();
    void next();
    void prev();

private slots:
    void media_status_changed(QMediaPlayer::MediaStatus status);

private:
    void set_queue(int index);
    void play_current();
    // void set_source(QUrl source);
    QMediaPlayer *mediaplayer;
    QAudioOutput *output;
    QVariantList queue;
    int current_index;
    QVariantList table;
    queuemodel *model;

signals:
    void player_ready();
    void player_state_changed();
    void time_changed();
};

#endif // PLAYER_H
