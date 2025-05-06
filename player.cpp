#include "player.h"
#include "database.h"
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QVariantList>
#include <QVariantMap>
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

    //media status signal
    connect(mediaplayer, &QMediaPlayer::mediaStatusChanged, this, &player::media_status_changed);

    emit player_ready();
}

// void player::set_source(QUrl source)
// {
//     mediaplayer->setSource(QUrl::fromLocalFile(source));
// }

void player::play(int index)
{

    set_queue(index);
    play_current();

    // QVariantMap song = queue[current_index].toMap();
    // mediaplayer->setSource(QUrl::fromLocalFile(song["path"].toString()));
    // mediaplayer->play();
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

void player::plause()
{
    if ((mediaplayer->playbackState() == 0) || (mediaplayer->playbackState() == 2))
    {
        mediaplayer->play();
    }else {
        mediaplayer->pause();
    }
}

void player::next()
{
    current_index++;
    if (current_index < queue.count())
    {
        play_current();
    }
}

void player::prev()
{
    if (current_index != 0)
    {
        current_index--;
        play_current();
    }
}

void player::media_status_changed(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        current_index++;
        if (current_index < queue.count())
        {
            play_current();
        }
    }
}

void player::set_queue(int index)
{
    database *db = new database(this);
    table = db->get_table();

    queue.clear();

    for (int i = index; i<table.size(); i++)
    {
        queue.append(table[i]);
    }

    current_index = 0;
}

void player::play_current()
{
    QVariantMap song = queue[current_index].toMap();
    mediaplayer->setSource(QUrl::fromLocalFile(song["path"].toString()));
    mediaplayer->play();
}



