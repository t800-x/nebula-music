#include "player.h"
#include "songmodel.h"
#include "database.h"
#include "tag_reader.h"

#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QQmlApplicationEngine>
#include <QVariantList>
#include <QVariantMap>
#include <QAudioOutput>
#include <QTimer>
#include <QEventLoop>


player::player(QObject *parent)
    : QObject{parent}
{

}

void player::init(songmodel *model)
{
    mediaplayer = new QMediaPlayer(this);
    output = new QAudioOutput(this);
    connect(mediaplayer, &QMediaPlayer::playbackStateChanged, this, &player::player_state_changed);
    connect(mediaplayer, &QMediaPlayer::positionChanged, this, &player::time_changed);
    mediaplayer->setAudioOutput(output);
    output->setVolume(100);

    this->model = model;
    reader = new Tag_reader(this);


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
    if (current_index < model -> rowCount())
    {
        play_current();
    }
}

void player::prev()
{
    if (model -> rowCount() != 0)
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
        if (current_index < model->rowCount())
        {
            play_current();
        }
    }
}

void player::set_queue(int index)
{
    database *db = new database(this);
    table = db->get_table();

    model->clear();
    queue.clear();

    for (int i = index; i<table.size(); i++)
    {
        queue.append(table[i]);
    }

    model->setSongs(queue);

    current_index = 0;
    delete db;
    db = nullptr;
}

void player::play_current()
{

    QModelIndex index = model->index(current_index, 0);
    mediaplayer->setSource(QUrl::fromLocalFile(model->data(index, songmodel::PathRole).toString()));

    reader->read(model->data(index, songmodel::PathRole).toString().toUtf8().data());
    reader->get_synced_lyrics(model->data(index, songmodel::PathRole).toString().toUtf8().data());

    //mp3 files dont seem to play nice
    if (model->data(index, songmodel::PathRole).toString().endsWith(".mp3"))
    {
        mediaplayer->play();
        wait(250);
        set_position(1);
    }else {
        mediaplayer->play();
    }
}

void player::wait(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec(); // Blocks here until the timer calls quit()
}



