#ifndef ALBUMFILTERPROXYMODEL_H
#define ALBUMFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include "albummodel.h"

class AlbumFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(int  sortRole    READ sortRole    WRITE setSortRole   NOTIFY sortRoleChanged)
public:
    explicit AlbumFilterProxyModel(QObject *parent = nullptr);

    Q_INVOKABLE QString filterText() const { return m_filterText; }
    Q_INVOKABLE void setFilterText(const QString &text);

    int sortRole() const { return QSortFilterProxyModel::sortRole(); }
    void setSortRole(int role);

// public slots:
    // AlbumModel* toAlbumModel(QObject* parent);

signals:
    void filterTextChanged();
    void sortRoleChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_filterText;
};

#endif // ALBUMFILTERPROXYMODEL_H
