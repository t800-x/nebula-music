#ifndef QUEUEMODEL_H
#define QUEUEMODEL_H

#include <QObject>
#include <QAbstractListModel>

class queuemodel : public QAbstractListModel
{
    Q_OBJECT
public:
    queuemodel();
};

#endif // QUEUEMODEL_H
