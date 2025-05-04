#include "player.h"
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>

player::player(QObject *parent)
    : QObject{parent}
{

}

void player::init()
{
    mediaplayer = new QMediaPlayer(this);
    output = new QAudioOutput(this);
    connect(mediaplayer, &QMediaPlayer::playbackStateChanged, this, &player::player_state_changed);
    connect(mediaplayer, &QMediaPlayer::positionChanged, this, &player::time_changed);
    mediaplayer->setAudioOutput(output);
    output->setVolume(100);

    emit player_ready();
}

void player::set_source(QString source)
{
    mediaplayer->setSource(QUrl::fromLocalFile(source));
}

void player::play()
{
    if ((mediaplayer->playbackState() == 0) || (mediaplayer->playbackState() == 2))
    {
        mediaplayer->play();
    }else {
        mediaplayer->pause();
    }
}

QString player::get_title()
{
    QString title = "";
    QMediaMetaData meta = mediaplayer->metaData();
    title = meta.stringValue(QMediaMetaData::Title);

    return title;
}

QString player::get_artist()
{
    QString artist = "";
    QMediaMetaData meta = mediaplayer->metaData();
    artist = meta.stringValue(QMediaMetaData::AlbumArtist);

    return artist;
}

int player::state()
{
    int st = mediaplayer->playbackState();
    return st;
}

qint64 player::get_duration()
{
    return mediaplayer->duration();
}

qint64 player::get_position()
{
    return mediaplayer->position();
}

void player::set_position(qint64 position)
{
    mediaplayer->setPosition(position);
}



