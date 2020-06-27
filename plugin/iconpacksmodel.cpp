#include "iconpacksmodel.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include <QRegularExpression>

IconPacksModel::IconPacksModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_iconPacks(IconPackFactory::loadIconPacks())
{
    for (int i = 0; i < m_iconPacks.size(); ++i) {
        const auto iconPack = m_iconPacks.at(i);
        connect(iconPack, &IconPack::enabledChanged, this, [=]() {
            const auto index = createIndex(i, 0);
            emit dataChanged(index, index, {EnabledRole});
        });
    }
}

int IconPacksModel::rowCount(const QModelIndex &) const
{
    return m_iconPacks.size();
}

QVariant IconPacksModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_iconPacks.size())
        return {};

    auto iconPack = m_iconPacks.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return iconPack->displayName();
    case NameRole:
        return iconPack->name();
    case EnabledRole:
        return iconPack->enabled();
    case IconsRole:
        return iconPack->icons();
    case PreviewIconsRole: {
        const QString path("image://clockwork/icon-pack/" + iconPack->name() + "/");
        return iconPack->previewIcons().replaceInStrings(QRegularExpression("^"), path);
    }
    default:
        return {};
    }
}

bool IconPacksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_iconPacks.size())
        return false;

    auto iconPack = m_iconPacks.at(index.row());

    switch (role) {
    case EnabledRole:
        iconPack->setEnabled(value.toBool());
        break;
    default:
        return false;
    }

    return true;
}

QHash<int, QByteArray> IconPacksModel::roleNames() const
{
    static QHash<int, QByteArray> names = {
        {Qt::DisplayRole, "displayName"},
        {NameRole, "name"},
        {EnabledRole, "enabled"},
        {IconsRole, "icons"},
        {PreviewIconsRole, "previewIcons"},
    };
    return names;
}
