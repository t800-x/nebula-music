#ifndef TAG_READER_H
#define TAG_READER_H

#include <QObject>
#include <cstdio>
#include <vector>
#include <tpropertymap.h>
#include <tstringlist.h>
#include <tvariant.h>
#include <fileref.h>
#include <tag.h>
#include <id3v2tag.h>
#include <id3v2header.h>
#include <id3v2frame.h>
#include <mpegfile.h>
#include <synchronizedlyricsframe.h>

class Tag_reader : public QObject
{
    Q_OBJECT
private:
    enum filetype {
        mp3,
        flac,
        unsupported = 99
    };
public:
    explicit Tag_reader(QObject *parent = nullptr);
    struct SyncedLyrics {
        std::string text;
        unsigned int timestamp_ms;
    };

    void read(char* file);
    std::vector<SyncedLyrics> get_synced_lyrics(char* filename);
    QString get_cover(const QString audiopath);
private:
    std::vector<SyncedLyrics> parse_lyrics(QString lyrics_tag);
    bool ends_with(const std::string& value, const std::string& suffix);
    std::vector<Tag_reader::SyncedLyrics> get_lyrics_flac(char *filename);
    std::vector<Tag_reader::SyncedLyrics> get_lyrics_mp3(char *filename);

signals:
};

#endif // TAG_READER_H
