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
    int state();

private:
    QMediaPlayer *mediaplayer;
    QAudioOutput *output;

signals:
    void player_ready();
    void player_state_changed();
};

#endif // PLAYER_H
