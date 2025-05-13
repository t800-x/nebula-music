#include "tag_reader.h"

#include <cstdio>
#include <tpropertymap.h>
#include <tstringlist.h>
#include <tvariant.h>
#include <fileref.h>
#include <tag.h>
#include <id3v2tag.h>
#include <id3v2header.h>
#include <id3v2frame.h>
#include <flacfile.h>
#include <synchronizedlyricsframe.h>
#include <xiphcomment.h>
#include <string>
#include <cstring>
#include <vector>
#include <regex>
#include <QString>
#include <QStringList>
#include <mpegfile.h>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include <QDebug>

Tag_reader::Tag_reader(QObject *parent)
    : QObject{parent}
{}


bool Tag_reader::ends_with(const std::string &value, const std::string &suffix)
{
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void Tag_reader::read(char *file)
{
    TagLib::FileRef f(file);

    if(!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        qDebug() << "Title: " << tag->title().toCString(true);
        qDebug() << "Artist: " << tag->artist().toCString(true);
        qDebug() << "Album: " << tag->album().toCString(true);
    }
}

QString Tag_reader::get_cover(const QString audioPath)
{
    qDebug() << "Opening:" << audioPath;
    TagLib::FileRef ref(audioPath.toUtf8().constData());
    auto *baseFile = ref.file();
    if (!baseFile) {
        qDebug() << "Unsupported format or cannot open file.";
        return {};
    }

    QByteArray imgData;
    QString  mimeType;

    // 1) MP3 / MP4 via FileRef complex props (APIC, covr atoms)
    TagLib::StringList keys;
    if (auto *tag = baseFile->tag())
        keys = tag->complexPropertyKeys();

    for (const auto &key : keys) {
        // look for byte-vector props named APIC, covr, PICTURE, etc.
        auto props = baseFile->tag()->complexProperties(key);
        for (const auto &m : props) {
            for (const auto &p : m) {
                if (p.second.type() == TagLib::Variant::ByteVector) {
                    auto bv = p.second.value<TagLib::ByteVector>();
                    if (bv.isEmpty()) continue;
                    imgData  = QByteArray(reinterpret_cast<const char*>(bv.data()), bv.size());
                    // some formats (MP4) give the MIME via a text field
                    mimeType = QString::fromUtf8(m.value("MIME", {}).toString().toCString());
                    if (mimeType.isEmpty()) {
                        // fallback: guess from key
                        QString k = QString::fromUtf8(key.toCString()).toLower();
                        if (k.contains("png")) mimeType = "image/png";
                        else mimeType = "image/jpeg";
                    }
                    qDebug() << "Found image in complex prop" << QString::fromUtf8(key.toCString())
                             << ", size =" << imgData.size();
                    break;
                }
            }
            if (!imgData.isEmpty()) break;
        }
        if (!imgData.isEmpty()) break;
    }

    // 2) FLAC / Ogg/Opus: Vorbis picture blocks (only if nothing yet)
    if (imgData.isEmpty()) {
        if (auto *flac = dynamic_cast<TagLib::FLAC::File*>(baseFile)) {
            auto pics = flac->pictureList();
            if (!pics.isEmpty()) {
                auto &pic = pics.front();
                auto bv  = pic->data();
                imgData   = QByteArray(reinterpret_cast<const char*>(bv.data()), bv.size());
                mimeType  = QString::fromUtf8(pic->mimeType().toCString());
                qDebug() << "Found FLAC/Ogg picture block, mime =" << mimeType
                         << ", size =" << imgData.size();
            }
        }
    }

    if (imgData.isEmpty()) {
        qDebug() << "No embedded cover art found.";
        return {};
    }

    // 3) Determine extension
    QString ext;
    if (mimeType.contains("png", Qt::CaseInsensitive)) ext = "png";
    else if (mimeType.contains("jpeg", Qt::CaseInsensitive) ||
             mimeType.contains("jpg",  Qt::CaseInsensitive)) ext = "jpg";
    else ext = "bin";

    // 4) Build write path
    QFileInfo fi(audioPath);
    QDir dir = fi.dir();
    QString base = fi.completeBaseName();
    QString name = QString("%1_cover.%2").arg(base, ext);
    QString out  = dir.filePath(name);

    // if (!dir.isWritable()) {
    //     QString tmp = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    //     out = QDir(tmp).filePath(name);
    //     qDebug() << "Directory not writable, using temp location:" << out;
    // }

    // 5) Write to disk
    QFile fout(out);
    if (!fout.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open for writing:" << out;
        return {};
    }
    fout.write(imgData);
    fout.close();
    qDebug() << "Wrote" << imgData.size() << "bytes to" << out;

    return out;
}


std::vector<Tag_reader::SyncedLyrics> Tag_reader::get_synced_lyrics(char *filename)
{
    int filetype = Tag_reader::filetype::unsupported;
    std::string file(filename);
    std::vector<Tag_reader::SyncedLyrics> lyrics;

    get_cover(QString(filename));

    if (Tag_reader::ends_with(file, ".flac")) filetype = Tag_reader::filetype::flac;

    switch (filetype) {
    case Tag_reader::filetype::flac:
        return get_lyrics_flac(filename);
    }
    return {};
}

std::vector<Tag_reader::SyncedLyrics> Tag_reader::parse_lyrics(QString lyrics_tag) {
    std::vector<SyncedLyrics> result;

    // Split into lines
    QStringList lines = lyrics_tag.split('\n', Qt::SkipEmptyParts);

    // Regex to match [mm:ss.xx] timestamp
    std::regex timestamp_regex(R"(\[(\d{2}):(\d{2})\.(\d{2})\])");

    for (const QString &line : lines) {
        std::string std_line = line.toStdString();
        std::smatch match;

        if (std::regex_search(std_line, match, timestamp_regex)) {
            int minutes = std::stoi(match[1].str());
            int seconds = std::stoi(match[2].str());
            int hundredths = std::stoi(match[3].str());

            int timestamp_ms = (minutes * 60 + seconds) * 1000 + hundredths * 10;

            // Remove timestamp part from line
            std::string lyric_text = std_line.substr(match[0].length());

            // Skip empty lyrics (optional)
            if (!lyric_text.empty()) {
                qDebug() << timestamp_ms << " " << lyric_text;
                result.push_back({lyric_text, static_cast<unsigned int>(timestamp_ms)});
            }
        }
    }

    return result;
}

std::vector<Tag_reader::SyncedLyrics> Tag_reader::get_lyrics_flac(char *filename)
{

    TagLib::FLAC::File file(filename);
    if (!file.isValid()) {
        qDebug() << "Invalid FLAC file!";
    }

    TagLib::Ogg::XiphComment* comment = file.xiphComment();  // <- get Vorbis Comment block
    if (!comment) {
        qDebug() << "No Vorbis Comments found!";
    }

    // Iterate over the fields in the Vorbis comment
    const auto& map = comment->fieldListMap();
    // Look for the "LYRICS" tag
    auto it = map.find("LYRICS");
    if (it != map.end() && !it->second.isEmpty()) {
        QString lyrics = QString::fromUtf8(it->second.front().to8Bit(true));
        if (!lyrics.isEmpty()) return parse_lyrics(lyrics);
    } else {
        qDebug() << "No lyrics found.";
    }

    return {};
}


//This doesnt work, lyrics extraction support for more filetypes is to be added


// std::vector<Tag_reader::SyncedLyrics> Tag_reader::get_lyrics_mp3(char *filename)
// {
//     std::vector<SyncedLyrics> lyrics;
//     qDebug() << "Starting lyrics extraction for:" << filename;

//     TagLib::MPEG::File file(filename);
//     if(!file.isValid()) {
//         qDebug() << "File is invalid or could not be opened";
//         return lyrics;
//     }

//     if(!file.ID3v2Tag()) {
//         qDebug() << "No ID3v2 tag found in file";
//         return lyrics;
//     }

//     const auto frames = file.ID3v2Tag()->frameList("SYLT");
//     qDebug() << "Found" << frames.size() << "SYLT frames";

//     for(auto* frame : frames) {
//         if(auto* sylt = dynamic_cast<TagLib::ID3v2::SynchronizedLyricsFrame*>(frame)) {
//             qDebug() << "Processing SYLT frame:";
//             qDebug() << "  Description:" << sylt->description().toCString(true);
//             qDebug() << "  Language:" << QString::fromLatin1(sylt->language().data(), 3);
//             qDebug() << "  Content type:" << sylt->type();

//             const auto entries = sylt->synchedText();
//             qDebug() << "  Found" << entries.size() << "lyric entries";

//             for(const auto& entry : entries) {
//                 lyrics.push_back({
//                     entry.text.to8Bit(true),
//                     entry.time
//                 });

//                 qDebug() << "    Added lyric at" << entry.time << "ms:"
//                          << QString::fromStdString(entry.text.to8Bit(true));
//             }
//         }
//     }

//     qDebug() << "Total lyrics extracted:" << lyrics.size();
//     return lyrics;
// }


