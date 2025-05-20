#include "headers/player.h"
#include "models/songmodel.h"

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
    //Initialize media player
    mediaplayer = new QMediaPlayer(this);
    output = new QAudioOutput(this);
    connect(mediaplayer, &QMediaPlayer::playbackStateChanged, this, &player::player_state_changed);
    connect(mediaplayer, &QMediaPlayer::positionChanged, this, &player::time_changed);
    mediaplayer->setAudioOutput(output);
    output->setVolume(100);

    playing_next = new songmodel(this);
    currently_playing = new songmodel(this);
    visual_queue = new songmodel(this);
    history = new songmodel(this);
    manual_idx = 0;

    //media status signal
    connect(mediaplayer, &QMediaPlayer::mediaStatusChanged, this, &player::media_status_changed);

    emit player_ready();
}

void player::play(QObject* m_table, int index)
{
    songmodel* table = qobject_cast<songmodel*>(m_table);
    set_queue(table, index);
    current_index = 0;

    //Move next item to currently playing
    auto idx = playing_next->index(current_index, 0);

    auto path = playing_next->data(idx, songmodel::PathRole).toString();
    QString title = playing_next->data(idx, songmodel::TitleRole).toString();
    QString artist = playing_next->data(idx, songmodel::ArtistRole).toString();
    QString album = playing_next->data(idx, songmodel::AlbumRole).toString();
    QString cover = playing_next->data(idx, songmodel::CoverRole).toString();

    currently_playing->clear();
    currently_playing->append(path, title, cover, artist, album);

    mediaplayer->setSource(QUrl::fromLocalFile(path));
    //mp3 files dont seem to play nice
    if (path.endsWith(".mp3"))
    {
        mediaplayer->play();
        wait(500);
        set_position(1);
    }else {
        mediaplayer->play();
    }

    visual_queue->removeRows(0, 1);

}

void player::move(int from, int to)
{
    visual_queue->move(from, to);

    int offset = history->rowCount() + currently_playing->rowCount();
    playing_next->move(offset + from, offset + to);
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
    if (current_index < playing_next -> rowCount())
    {
        //Move currently playing to history
        auto index = currently_playing->index(0,0);

        auto path = currently_playing->data(index, songmodel::PathRole).toString();
        QString title = currently_playing->data(index, songmodel::TitleRole).toString();
        QString artist = currently_playing->data(index, songmodel::ArtistRole).toString();
        QString album = currently_playing->data(index, songmodel::AlbumRole).toString();
        QString cover = currently_playing->data(index, songmodel::CoverRole).toString();

        history->append(path, title, cover, artist, album);

        //Move top item of visual queue to currently playing
        index = visual_queue->index(0,0);

        path = visual_queue->data(index, songmodel::PathRole).toString();
        title = visual_queue->data(index, songmodel::TitleRole).toString();
        artist = visual_queue->data(index, songmodel::ArtistRole).toString();
        album = visual_queue->data(index, songmodel::AlbumRole).toString();
        cover = visual_queue->data(index, songmodel::CoverRole).toString();

        currently_playing->clear();
        currently_playing->append(path, title, cover, artist, album);

        visual_queue->removeRows(0,1);

        mediaplayer->setSource(QUrl::fromLocalFile(path));
        //mp3 files dont seem to play nice
        if (path.endsWith(".mp3"))
        {
            mediaplayer->play();
            wait(500);
            set_position(1);
        }else {
            mediaplayer->play();
        }
    }
}

void player::prev()
{
    if (playing_next -> rowCount() != 0)
    {

        current_index--;
        if (history -> rowCount() == 0)
        {
            return;
        }

        //Move Currently playing to top of visual queue
        auto index = currently_playing->index(0, 0);
        auto path = currently_playing->data(index, songmodel::PathRole).toString();
        if (path.isEmpty()) return;
        QString title = currently_playing->data(index, songmodel::TitleRole).toString();
        QString artist = currently_playing->data(index, songmodel::ArtistRole).toString();
        QString album = currently_playing->data(index, songmodel::AlbumRole).toString();
        QString cover = currently_playing->data(index, songmodel::CoverRole).toString();

        visual_queue->insertAtTop(path, title, cover, artist, album);

        //Move last item of history to currently playing
        index = history->index(history->rowCount() - 1, 0);

        path = history->data(index, songmodel::PathRole).toString();
        if(path.isEmpty()) return;
        title = history->data(index, songmodel::TitleRole).toString();
        artist = history->data(index, songmodel::ArtistRole).toString();
        album = history->data(index, songmodel::AlbumRole).toString();
        cover = history->data(index, songmodel::CoverRole).toString();

        currently_playing->clear();
        currently_playing->append(path, title, cover, artist, album);
        history->removeRows(history->rowCount() - 1, 1);

        mediaplayer->setSource(QUrl::fromLocalFile(path));
        //mp3 files dont seem to play nice
        if (path.endsWith(".mp3"))
        {
            set_position(1);
            mediaplayer->play();
        }else {
            mediaplayer->play();
        }
    }
}

void player::play_next(const QJSValue& data)
{
    auto path = data.property("path").toString();
    QString title = data.property("title").toString();
    QString artist = data.property("artist").toString();
    QString album = data.property("album").toString();
    QString cover = data.property("cover").toString();

    visual_queue->insertAtTop(path, title, cover, artist, album);
    int offset = history->rowCount() + currently_playing->rowCount();
    playing_next->insert(offset, path, title, cover, artist, album);
    manual_idx = offset;
}

void player::add_to_queue(const QJSValue& data)
{
    auto path = data.property("path").toString();
    QString title = data.property("title").toString();
    QString artist = data.property("artist").toString();
    QString album = data.property("album").toString();
    QString cover = data.property("cover").toString();

    int offset = history->rowCount() + currently_playing->rowCount();
    visual_queue->insert((manual_idx + 1)- offset, path, title, cover, artist, album);
    playing_next->insert(manual_idx + 1, path, title, cover, artist, album);
    manual_idx++;
}

void player::media_status_changed(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        current_index++;
        if (current_index < playing_next->rowCount())
        {
            play_current();
        }
    }
}

void player::set_queue(songmodel* table, int index)
{
    playing_next->clear();
    visual_queue->clear();
    history->clear();
    manual_idx = 0;
    for (int i = index; i<table->rowCount(); i++)
    {
        QModelIndex idx = table->index(i, 0);

        QString title = table->data(idx, songmodel::TitleRole).toString();
        QString path = table->data(idx, songmodel::PathRole).toString();
        QString artist = table->data(idx, songmodel::ArtistRole).toString();
        QString album = table->data(idx, songmodel::AlbumRole).toString();
        QString cover = table->data(idx, songmodel::CoverRole).toString();

        if (title.isEmpty())
        {
            qDebug()<< "Empty";
        }


        playing_next->append(path, title, cover, artist, album);
        visual_queue->append(path, title, cover, artist, album);
    }
}

void player::play_current()
{

    QModelIndex index = currently_playing->index(0, 0);
    QString path = currently_playing->data(index, songmodel::PathRole).toString();
    QString title = currently_playing->data(index, songmodel::TitleRole).toString();
    QString artist = currently_playing->data(index, songmodel::ArtistRole).toString();
    QString album = currently_playing->data(index, songmodel::AlbumRole).toString();
    QString cover = currently_playing->data(index, songmodel::CoverRole).toString();

    if (!path.isEmpty())
    {
        history->append(path, title, cover, artist, album);
    }

    index = visual_queue->index(0, 0);
    path = visual_queue->data(index, songmodel::PathRole).toString();
    title = visual_queue->data(index, songmodel::TitleRole).toString();
    artist = visual_queue->data(index, songmodel::ArtistRole).toString();
    album = visual_queue->data(index, songmodel::AlbumRole).toString();
    cover = visual_queue->data(index, songmodel::CoverRole).toString();
    currently_playing->clear();
    currently_playing->append(path, title, cover, artist, album);

    mediaplayer->setSource(QUrl::fromLocalFile(path));
    //mp3 files dont seem to play nice
    if (path.endsWith(".mp3"))
    {
        mediaplayer->play();
        wait(500);
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
