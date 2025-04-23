#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>

class database : public QObject
{
    Q_OBJECT
public:
    explicit database(QObject *parent = nullptr);

signals:
};

#endif // DATABASE_H
