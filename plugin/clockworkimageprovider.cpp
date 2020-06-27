#include "clockworkimageprovider.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include "iconprovider.h"
#include <QDebug>

ClockworkImageProvider::ClockworkImageProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Image)
    , m_dynamicIconProviders()
    , m_iconPacks()
{
    for (const auto dynamicIcon : loadDynamicIcons()) {
        m_dynamicIconProviders.insert(dynamicIcon->name(), dynamicIcon->iconProvider());
    }

    const auto iconPacks = IconPackFactory::loadIconPacks();
    for (const auto iconPack : iconPacks) {
        m_iconPacks.insert(iconPack->name(), iconPack);
    }
}

QImage ClockworkImageProvider::requestImage(const QString &id,
                                            QSize *size,
                                            const QSize &requestedSize)
{
    QImage result;
    const auto type = id.section('/', 0, 0);
    const auto path = id.section('/', 1);

    if (type == QStringLiteral("dynamic-icon")) {
        result = dynamicIcon(path, requestedSize);
    } else if (type == QStringLiteral("icon-pack")) {
        result = iconFromPack(path, requestedSize);
    } else {
        qWarning() << "Requesting invalid image type:" << type;
    }

    if (size)
        *size = result.size();

    return result;
}

QImage ClockworkImageProvider::dynamicIcon(const QString &path, const QSize &requestedSize)
{
    if (!m_dynamicIconProviders.contains(path))
        return {};

    const auto provider = m_dynamicIconProviders.value(path);
    return provider->requestImage(requestedSize);
}

QImage ClockworkImageProvider::iconFromPack(const QString &path, const QSize &requestedSize)
{
    const auto iconPackName = path.section('/', 0, 0);
    const auto iconId = path.section('/', 1);

    if (!m_iconPacks.contains(iconPackName))
        return {};

    const auto provider = m_iconPacks.value(iconPackName);
    return provider->requestIcon(iconId, requestedSize);
}
