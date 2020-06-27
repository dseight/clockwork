#include "appgridiconprovider.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include <QDebug>

namespace {

IconPack *iconPackByName(const QString &name)
{
    const auto iconPacks = IconPackFactory::loadIconPacks();

    for (auto iconPack : iconPacks) {
        if (iconPack->name() == name) {
            return iconPack;
        }
    }

    qDebug() << "Icon pack" << name << "not found";
    return nullptr;
}

} // namespace

AppGridIconProvider::AppGridIconProvider(QObject *parent)
    : QObject(parent)
    , m_iconPack(nullptr)
{
}

QString AppGridIconProvider::iconPackName()
{
    if (m_iconPack == nullptr)
        return {};

    return m_iconPack->name();
}

void AppGridIconProvider::setIconPackName(const QString &name)
{
    m_iconPack = iconPackByName(name);
}

bool AppGridIconProvider::iconAvailable(const QString &desktopPath)
{
    if (m_iconPack == nullptr)
        return false;

    return !m_iconPack->iconByDesktopPath(desktopPath).isEmpty();
}

QString AppGridIconProvider::iconSource(const QString &desktopPath)
{
    if (m_iconPack == nullptr)
        return {};

    const auto name = m_iconPack->name();
    const auto id = m_iconPack->iconByDesktopPath(desktopPath);
    return QStringLiteral("image://clockwork/icon-pack/%1/%2").arg(name, id);
}
