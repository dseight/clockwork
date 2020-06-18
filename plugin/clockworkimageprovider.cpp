#include "clockworkimageprovider.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"
#include "iconprovider.h"

ClockworkImageProvider::ClockworkImageProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Image)
    , m_providers()
{
    for (const auto dynamicIcon : loadDynamicIcons()) {
        m_providers.insert(dynamicIcon->name(), dynamicIcon->iconProvider());
    }
}

QImage ClockworkImageProvider::requestImage(const QString &id,
                                            QSize *size,
                                            const QSize &requestedSize)
{
    if (!m_providers.contains(id))
        return {};

    const auto provider = m_providers.value(id);
    auto image = provider->requestImage(requestedSize);

    if (size)
        *size = image.size();

    return image;
}
