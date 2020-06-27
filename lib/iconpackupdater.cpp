#include "iconpackupdater.h"
#include "iconpack.h"
#include "iconprovider.h"
#include "iconupdater.h"
#include <QDir>
#include <QImage>

namespace {

class IconPackIconProvider : public IconProvider
{
    Q_OBJECT

public:
    IconPackIconProvider(IconPack *iconPack, const QString &iconId, QObject *parent)
        : IconProvider(parent)
        , m_iconPack(iconPack)
        , m_iconId(iconId)
    {
    }

    QImage requestImage(const QSize &requestedSize) override
    {
        return m_iconPack->requestIcon(m_iconId, requestedSize);
    }

private:
    IconPack *m_iconPack;
    QString m_iconId;
};

} // namespace

IconPackUpdater::IconPackUpdater(IconPack *iconPack)
    : QObject(iconPack)
{
    QDir dir(QStringLiteral("/usr/share/applications"));
    const auto infoList = dir.entryInfoList({"*.desktop"}, QDir::Files);

    for (const auto info : infoList) {
        const auto desktopPath = info.absoluteFilePath();
        const auto iconId = iconPack->iconByDesktopPath(desktopPath);

        if (!iconId.isEmpty()) {
            auto provider = new IconPackIconProvider(iconPack, iconId, this);
            new IconUpdater(provider, desktopPath, this);
        }
    }
}

IconPackUpdater::~IconPackUpdater() = default;

#include "iconpackupdater.moc"
