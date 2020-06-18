#include "dynamiciconsmodel.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"

DynamicIconsModel::DynamicIconsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_icons(loadDynamicIcons())
{
    for (auto icon : m_icons) {
        connect(icon, &DynamicIcon::enabledChanged, this, [=]() {
            const auto row = m_icons.indexOf(icon);
            const auto index = createIndex(row, 0);
            emit dataChanged(index, index, {EnabledRole});
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
        return QStringLiteral("image://clockwork/%1").arg(icon->name());
    case AvailableRole:
        return icon->available();
    case EnabledRole:
        return icon->enabled();
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
    };
    return names;
}

void DynamicIconsSortModel::setSourceModel(QAbstractItemModel *model)
{
    QSortFilterProxyModel::setSourceModel(model);
    sort(0);
}
