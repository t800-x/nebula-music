#ifndef TAG_READER_H
#define TAG_READER_H

#include "models/SyncedLyrics.h"

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

class songmodel;

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

    songmodel* parse_tags(QString filepath);
    QString get_cover(const QString filepath, const QString outBaseName);

private:
    std::vector<QString> get_basic_tags(QString filepath);
signals:
};

#endif // TAG_READER_H
