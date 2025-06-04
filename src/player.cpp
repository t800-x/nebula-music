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
    connect(mediaplayer, &QMediaPlayer::mediaStatusChanged, this, &player::media_status_changed);

    emit player_ready();
}

// Helper function to play files with MP3 workaround
void player::playFile(const QString& path)
{
    mediaplayer->setSource(QUrl::fromLocalFile(path));
    if (path.endsWith(".mp3")) {
        mediaplayer->play();
        wait(500);
        set_position(1);
    } else {
        mediaplayer->play();
    }
}

// Helper to move current song to history
void player::moveCurrentToHistory()
{
    if (currently_playing->rowCount() == 0) return;

    QModelIndex idx = currently_playing->index(0, 0);
    history->append(
        currently_playing->data(idx, songmodel::PathRole).toString(),
        currently_playing->data(idx, songmodel::SongIdRole).toInt(),
        currently_playing->data(idx, songmodel::TitleRole).toString(),
        currently_playing->data(idx, songmodel::ArtistRole).toString(),
        currently_playing->data(idx, songmodel::AlbumRole).toString()
        );
}

// Helper to move top of queue to current
void player::moveQueueTopToCurrent()
{
    if (visual_queue->rowCount() == 0) return;

    QModelIndex idx = visual_queue->index(0, 0);
    currently_playing->clear();
    currently_playing->append(
        visual_queue->data(idx, songmodel::PathRole).toString(),
        visual_queue->data(idx, songmodel::SongIdRole).toInt(),
        visual_queue->data(idx, songmodel::TitleRole).toString(),
        visual_queue->data(idx, songmodel::ArtistRole).toString(),
        visual_queue->data(idx, songmodel::AlbumRole).toString()
        );
    visual_queue->removeRows(0, 1);
}

// Unified media loading sequence
void player::loadMediaAndNotify()
{
    const QString path = currently_playing->data(currently_playing->index(0, 0), songmodel::PathRole).toString();
    playFile(path);
    load_synced_lyrics();
    emit songChanged();
}

void player::play(QObject* m_table, int index)
{
    songmodel* table = qobject_cast<songmodel*>(m_table);
    set_queue(table, index);
    current_index = 0;

    moveQueueTopToCurrent();
    loadMediaAndNotify();
}

void player::move(int from, int to)
{
    visual_queue->move(from, to);
    playing_next->move(history->rowCount() + currently_playing->rowCount() + from,
                       history->rowCount() + currently_playing->rowCount() + to);
}

void player::plause()
{
    if (mediaplayer->playbackState() == QMediaPlayer::StoppedState ||
        mediaplayer->playbackState() == QMediaPlayer::PausedState) {
        mediaplayer->play();
    } else {
        mediaplayer->pause();
    }
}

void player::next()
{
    current_index++;
    if (current_index >= playing_next->rowCount()) return;

    moveCurrentToHistory();
    moveQueueTopToCurrent();
    loadMediaAndNotify();
}

void player::prev()
{
    if (playing_next->rowCount() == 0 || history->rowCount() == 0) return;

    current_index--;

    // Move current to top of queue
    QModelIndex idx = currently_playing->index(0, 0);
    visual_queue->insertAtTop(
        currently_playing->data(idx, songmodel::PathRole).toString(),
        currently_playing->data(idx, songmodel::SongIdRole).toInt(),
        currently_playing->data(idx, songmodel::TitleRole).toString(),
        currently_playing->data(idx, songmodel::ArtistRole).toString(),
        currently_playing->data(idx, songmodel::AlbumRole).toString()
        );

    // Move last history to current
    idx = history->index(history->rowCount() - 1, 0);
    currently_playing->clear();
    currently_playing->append(
        history->data(idx, songmodel::PathRole).toString(),
        history->data(idx, songmodel::SongIdRole).toInt(),
        history->data(idx, songmodel::TitleRole).toString(),
        history->data(idx, songmodel::ArtistRole).toString(),
        history->data(idx, songmodel::AlbumRole).toString()
        );
    history->removeRows(history->rowCount() - 1, 1);

    loadMediaAndNotify();
}

void player::play_next(const QJSValue& data)
{
    const QString path = data.property("path").toString();
    const int songId = data.property("songId").toInt();
    const QString title = data.property("title").toString();
    const QString artist = data.property("artist").toString();
    const QString album = data.property("album").toString();


    visual_queue->insertAtTop(path, songId, title, artist, album);
    const int offset = history->rowCount() + currently_playing->rowCount();
    playing_next->insert(offset, path, title, artist, album,  songId);
    manual_idx = offset;
}

void player::add_to_queue(const QJSValue& data)
{
    const QString path = data.property("path").toString();
    const int songId = data.property("songId").toInt();
    const QString title = data.property("title").toString();
    const QString artist = data.property("artist").toString();
    const QString album = data.property("album").toString();

    const int offset = history->rowCount() + currently_playing->rowCount();
    const int insertPos = (manual_idx + 1) - offset;
    visual_queue->insert(insertPos, path, title, artist, album, songId);
    playing_next->insert(manual_idx + 1, path, title, artist, album, songId);
    manual_idx++;
}

bool player::isInFrame(unsigned int lyricTimestamp, int idx)
{
    const qint64 currentPos = get_position();
    const int lyricCount = synced_lyrics->rowCount();

    if (lyricCount <= 0) return false;

    if (idx == 0) {
        return lyricCount > 1 ?
                   (currentPos >= lyricTimestamp && currentPos < synced_lyrics->data(synced_lyrics->index(1, 0), SyncedLyricsModel::Timestamp_msRole).toUInt()) :
                   currentPos >= lyricTimestamp;
    }

    if (idx == lyricCount - 1) {
        return currentPos >= lyricTimestamp;
    }

    const unsigned int nextTimestamp = synced_lyrics->data(synced_lyrics->index(idx + 1, 0), SyncedLyricsModel::Timestamp_msRole).toUInt();
    return currentPos >= lyricTimestamp && currentPos < nextTimestamp;
}

void player::media_status_changed(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        current_index++;
        if (current_index < playing_next->rowCount()) {
            moveCurrentToHistory();
            moveQueueTopToCurrent();
            loadMediaAndNotify();
        }
    }
}

void player::set_queue(songmodel* table, int index)
{
    playing_next->clear();
    visual_queue->clear();
    history->clear();
    manual_idx = 0;

    for (int i = index; i < table->rowCount(); i++) {
        QModelIndex idx = table->index(i, 0);
        playing_next->append(
            table->data(idx, songmodel::PathRole).toString(),
            table->data(idx, songmodel::SongIdRole).toInt(),
            table->data(idx, songmodel::TitleRole).toString(),
            table->data(idx, songmodel::ArtistRole).toString(),
            table->data(idx, songmodel::AlbumRole).toString()
            );
        visual_queue->append(
            table->data(idx, songmodel::PathRole).toString(),
            table->data(idx, songmodel::SongIdRole).toInt(),
            table->data(idx, songmodel::TitleRole).toString(),
            table->data(idx, songmodel::ArtistRole).toString(),
            table->data(idx, songmodel::AlbumRole).toString()
            );
    }
}

void player::play_current()
{
    if (currently_playing->rowCount() > 0) {
        QModelIndex idx = currently_playing->index(0, 0);
        history->append(
            currently_playing->data(idx, songmodel::PathRole).toString(),
            currently_playing->data(idx, songmodel::SongIdRole).toInt(),
            currently_playing->data(idx, songmodel::TitleRole).toString(),
            currently_playing->data(idx, songmodel::ArtistRole).toString(),
            currently_playing->data(idx, songmodel::AlbumRole).toString()
            );
    }

    moveQueueTopToCurrent();
    if (currently_playing->rowCount() > 0) {
        loadMediaAndNotify();
    }
}

void player::wait(int milliseconds)
{
    QEventLoop loop;
    QTimer::singleShot(milliseconds, &loop, &QEventLoop::quit);
    loop.exec();
}

void player::load_synced_lyrics()
{
    synced_lyrics->clear();
    if (currently_playing->rowCount() == 0) return;

    const QString path = currently_playing->data(currently_playing->index(0, 0), songmodel::PathRole).toString();
    if (path.isEmpty()) return;

    const QString lrcPath = path.left(path.lastIndexOf('.')) + ".lrc";
    QFile file(lrcPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    QRegularExpression regex(R"(\[(\d+):(\d+)(?:\.(\d+))?\](.*))");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = regex.match(line);
        if (!match.hasMatch()) continue;

        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        int milliseconds = match.captured(3).left(3).rightJustified(3, '0').toInt();
        QString lyric = match.captured(4).trimmed();

        synced_lyrics->append(lyric, minutes * 60000 + seconds * 1000 + milliseconds);
    }
    file.close();
}
