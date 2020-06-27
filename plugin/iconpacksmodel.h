#ifndef ICONPACKSMODEL_H
#define ICONPACKSMODEL_H

#include <QAbstractListModel>

class IconPack;

class IconPacksModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole,
        EnabledRole,
        IconsRole,
        PreviewIconsRole,
    };
    Q_ENUM(Roles)

    explicit IconPacksModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    QList<IconPack *> m_iconPacks;
};

#endif // ICONPACKSMODEL_H
