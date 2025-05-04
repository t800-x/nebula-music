#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>

class player : public QObject
{
    Q_OBJECT
public:
    explicit player(QObject *parent = nullptr);

public slots:
    void init();
    void set_source(QString source);
    void play();
    QString get_title();
    QString get_artist();
    int state();
    qint64 get_duration();
    qint64 get_position();
    void set_position(qint64 position);

private:
    QMediaPlayer *mediaplayer;
    QAudioOutput *output;

signals:
    void player_ready();
    void player_state_changed();
    void time_changed();
};

#endif // PLAYER_H
