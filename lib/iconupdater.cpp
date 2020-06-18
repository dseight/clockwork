#include "iconupdater.h"
#include "desktopentry.h"
#include "iconprovider.h"
#include "iconupdater_p.h"
#include <MGConfItem>
#include <silicatheme.h>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>

namespace {

QString dconfIconBackupPath(const QString &desktopPath)
{
    QFileInfo info(desktopPath);
    return QStringLiteral("/com/dseight/clockwork/saved-id/%1").arg(info.baseName());
}

QString storedIconPath(const QString &desktopPath)
{
    MGConfItem dconf(dconfIconBackupPath(desktopPath));
    return dconf.value().toString();
}

void storeIconPath(const QString &desktopPath, const QString &iconPath)
{
    MGConfItem dconf(dconfIconBackupPath(desktopPath));
    dconf.set(iconPath);
}

QString generateIconPath(const QString &desktopPath)
{
    QFileInfo info(desktopPath);

    // Timestamp appending will prevent Jolla Home from icon caching
    const auto currentSecs = QString::number(QDateTime::currentMSecsSinceEpoch() / 1000);

    return QStringLiteral("/usr/share/clockwork/icons/%1-%2.png").arg(info.baseName(), currentSecs);
}

} // namespace

IconUpdaterPrivate::IconUpdaterPrivate(IconProvider *provider, const QString &desktopPath)
    : provider(provider)
    , desktopPath(desktopPath)
{
}

void IconUpdaterPrivate::updateThemeIcon()
{
    DesktopEntry desktop(desktopPath);

    auto currentIconPath = desktop.icon();
    auto isOurIconPath = currentIconPath.startsWith("/usr/share/clockwork");

    if (!isOurIconPath)
        storeIconPath(desktopPath, currentIconPath);

    auto newIconPath = generateIconPath(desktopPath);
    QDir dir = QFileInfo(newIconPath).absoluteDir();
    if (!dir.mkpath(".")) {
        qWarning() << "Could not create icons directory" << dir.absolutePath();
        return;
    }

    const auto size = qRound(Silica::Theme::instance()->iconSizeLauncher());
    QImage newIcon = provider->requestImage({size, size});

    if (!newIcon.save(newIconPath)) {
        qWarning() << "Could not save new icon" << newIconPath << ", update aborted";
        return;
    }

    desktop.setIcon(newIconPath);

    if (!desktop.save()) {
        qWarning() << "Could not update desktop file" << desktopPath << ", icon update aborted";
        return;
    }

    // Remove old icon
    if (isOurIconPath) {
        QFile icon(currentIconPath);
        icon.remove();
    }

    qDebug() << "Updated theme icon for" << desktopPath;
}

void IconUpdaterPrivate::restoreThemeIcon()
{
    const auto iconPath = storedIconPath(desktopPath);
    if (iconPath.isEmpty())
        return;

    DesktopEntry desktop(desktopPath);

    const auto currentIconPath = desktop.icon();

    desktop.setIcon(iconPath);
    desktop.save();

    if (currentIconPath.startsWith("/usr/share/clockwork")) {
        QFile icon(currentIconPath);
        icon.remove();
    }

    qDebug() << "Restored theme icon for" << desktopPath;
}

IconUpdater::IconUpdater(IconProvider *provider, const QString &desktopPath, QObject *parent)
    : QObject(parent)
    , d_ptr(new IconUpdaterPrivate(provider, desktopPath))
{
    update();
}

IconUpdater::~IconUpdater()
{
    d_ptr->restoreThemeIcon();
}

void IconUpdater::update()
{
    d_ptr->updateThemeIcon();
}
