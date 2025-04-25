#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <qsqlquery.h>
#include <QVariantList>
#include <QVariantMap>
#include <QDirIterator>
#include <QStringList>
#include <QMediaMetaData>
#include <QMediaPlayer>

class database : public QObject
{
    Q_OBJECT
public:
    explicit database(QObject *parent = nullptr);

private:
    QSqlDatabase db;
    QVariantList table;

    bool create_table();
    QVariantList query_db(QString query);
    QStringList get_audio_files(const QString &directoryPath);

public slots:
    bool init();
    QVariantList get_table();
    bool add_to_library(QString path);

signals:
    void table_changed();
};

#endif // DATABASE_H
