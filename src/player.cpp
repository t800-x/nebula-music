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
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>


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

    synced_lyrics = new SyncedLyricsModel(this);

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

    load_synced_lyrics();
    songChanged();

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
        load_synced_lyrics();
        emit songChanged();
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

        load_synced_lyrics();
        emit songChanged();
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

bool player::isInFrame(unsigned int lyricTimestamp, int idx)
{
    qint64 currentPos = get_position();

    // No lyrics case
    if (synced_lyrics->rowCount() <= 0) return false;

    // First lyric: active from start to next lyric
    if (idx == 0) {
        if (synced_lyrics->rowCount() > 1) {
            unsigned int nextTimestamp = synced_lyrics->data(
                                                          synced_lyrics->index(1, 0),
                                                          SyncedLyricsModel::Timestamp_msRole
                                                          ).toUInt();
            return currentPos >= lyricTimestamp && currentPos < nextTimestamp;
        }
        return currentPos >= lyricTimestamp;
    }
    // Last lyric: active from its timestamp onward
    else if (idx == synced_lyrics->rowCount() - 1) {
        return currentPos >= lyricTimestamp;
    }
    // Middle lyrics: active between current and next timestamp
    else {
        unsigned int nextTimestamp = synced_lyrics->data(
                                                      synced_lyrics->index(idx + 1, 0),
                                                      SyncedLyricsModel::Timestamp_msRole
                                                      ).toUInt();
        return currentPos >= lyricTimestamp && currentPos < nextTimestamp;
    }
}

void player::media_status_changed(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
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
            load_synced_lyrics();
            emit songChanged();
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
    load_synced_lyrics();
    emit songChanged();
}

void player::wait(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec(); // Blocks here until the timer calls quit()
}

void player::load_synced_lyrics()
{
    synced_lyrics->clear();
    const QString path = currently_playing->data(currently_playing->index(0, 0), songmodel::PathRole).toString();
    if (path.isEmpty() || path.isNull()) return;
    const QString filepath = path.left(path.lastIndexOf('.')) + ".lrc";

    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpression regex(R"(\[(\d+):(\d+)(?:\.(\d+))?\](.*))");
        QRegularExpressionMatch match = regex.match(line);

        if (match.hasMatch()) {
            int minutes = match.captured(1).toInt();
            int seconds = match.captured(2).toInt();
            int milliseconds = match.captured(3).isEmpty() ? 0 : match.captured(3).left(3).rightJustified(3, '0').toInt();
            QString lyric = match.captured(4).trimmed();

            int totalMs = minutes * 60000 + seconds * 1000 + milliseconds;
            synced_lyrics->append(lyric, totalMs);
        }
    }

    file.close();
}
