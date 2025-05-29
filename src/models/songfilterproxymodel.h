#ifndef SONGFILTERPROXYMODEL_H
#define SONGFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

class SongFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(int  sortRole    READ sortRole    WRITE setSortRole   NOTIFY sortRoleChanged)

public:
    explicit SongFilterProxyModel(QObject* parent = nullptr);

    QString filterText() const { return m_filterText; }
    void setFilterText(const QString &text);

    int sortRole() const { return QSortFilterProxyModel::sortRole(); }
    void setSortRole(int role);

signals:
    void filterTextChanged();
    void sortRoleChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_filterText;
};

#endif // SONGFILTERPROXYMODEL_H
