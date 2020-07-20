#include "dynamiciconsmodel.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"
#include "iconprovider.h"
#include <QDateTime>

DynamicIconsModel::DynamicIconsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_icons(loadDynamicIcons())
{
    for (auto icon : m_icons) {
        m_iconsCountForPackage[icon->packageName()]++;

        connect(icon, &DynamicIcon::enabledChanged, this, [=]() {
            const auto row = m_icons.indexOf(icon);
            const auto index = createIndex(row, 0);
            emit dataChanged(index, index, {EnabledRole});
        });

        connect(icon->iconProvider(), &IconProvider::imageUpdated, this, [=]() {
            const auto row = m_icons.indexOf(icon);
            const auto index = createIndex(row, 0);
            emit dataChanged(index, index, {IconSourceRole});
        });
    }
}

int DynamicIconsModel::rowCount(const QModelIndex &) const
{
    return m_icons.size();
}

QVariant DynamicIconsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_icons.size())
        return {};

    auto icon = m_icons.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return icon->displayName();
    case NameRole:
        return icon->name();
    case IconSourceRole:
        return QStringLiteral("image://clockwork/dynamic-icon/%1?t=%2")
            .arg(icon->name(), QString::number(QDateTime::currentMSecsSinceEpoch()));
    case AvailableRole:
        return icon->available();
    case EnabledRole:
        return icon->enabled();
    case UniqueRole:
        return m_iconsCountForPackage[icon->packageName()] == 1;
    default:
        return {};
    }
}

bool DynamicIconsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_icons.size())
        return false;

    auto icon = m_icons.at(index.row());

    switch (role) {
    case EnabledRole:
        icon->setEnabled(value.toBool());
        break;
    default:
        return false;
    }

    return true;
}

QHash<int, QByteArray> DynamicIconsModel::roleNames() const
{
    static QHash<int, QByteArray> names = {
        {Qt::DisplayRole, "applicationName"},
        {NameRole, "name"},
        {IconSourceRole, "iconSource"},
        {EnabledRole, "enabled"},
        {UniqueRole, "unique"},
    };
    return names;
}

void DynamicIconsSortModel::setSourceModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel::setSourceModel(model);
    sort(0);
}
