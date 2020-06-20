#ifndef DYNAMICICONSMODEL_H
#define DYNAMICICONSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class DynamicIcon;

class DynamicIconsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole,
        IconSourceRole,
        AvailableRole,
        EnabledRole,
        UniqueRole,
    };
    Q_ENUM(Roles)

    explicit DynamicIconsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    const QList<DynamicIcon *> m_icons;
    QHash<QString, int> m_iconsCountForPackage;
};

class DynamicIconsSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    void setSourceModel(QAbstractItemModel *model) override;
};

#endif // DYNAMICICONSMODEL_H
