#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QJSValue>
#include <QVariantList>
#include <QVariantMap>
#include "models/songmodel.h"
#include "headers/tag_reader.h"
#include "models/syncedlyricsmodel.h"

class player : public QObject
{
    Q_OBJECT
public:
    explicit player(QObject *parent = nullptr);
    void playFile(const QString& path);
    void moveCurrentToHistory();
    void moveQueueTopToCurrent();
    void loadMediaAndNotify();

public slots:
    void init(songmodel *model);
    void play(QObject* table, int index);
    void move(int from, int to);

    void plause();
    void next();
    void prev();
    void play_next(const QJSValue& data);
    void add_to_queue(const QJSValue& data);

    bool isInFrame(unsigned int ms, int idx);

    QString get_title(){return mediaplayer->metaData().stringValue(QMediaMetaData::Title);}
    QString get_artist(){return mediaplayer->metaData().stringValue(QMediaMetaData::AlbumArtist);}
    QUrl get_cover() {return {};}
    int state(){return mediaplayer->playbackState();}
    qint64 get_duration(){return mediaplayer->duration();}
    qint64 get_position(){return mediaplayer->position();}
    void set_position(qint64 position){mediaplayer->setPosition(position);}
    songmodel* get_queue(){return visual_queue;}
    SyncedLyricsModel* get_synced_lyrics() {return synced_lyrics;}
    int getCurrentSongId() {return currently_playing->data(currently_playing->index(0, 0), songmodel::SongIdRole).toInt();}

private slots:
    void media_status_changed(QMediaPlayer::MediaStatus status);

private:
    void set_queue(songmodel* table, int index);
    void play_current();
    void wait(int milliseconds);
    void load_synced_lyrics();

    QMediaPlayer *mediaplayer;
    QAudioOutput *output;

    int current_index;
    int manual_idx;
    songmodel* currently_playing;
    songmodel* playing_next;
    songmodel* visual_queue;
    songmodel* history;

    SyncedLyricsModel* synced_lyrics;

signals:
    void player_ready();
    void player_state_changed();
    void time_changed();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void songChanged();
};

#endif // PLAYER_H
