#include "iconupdater.h"
#include "aliendalvikwatcher.h"
#include "desktopentry.h"
#include "iconprovider.h"
#include "iconupdater_p.h"
#include <MDesktopEntry>
#include <MGConfItem>
#include <silicatheme.h>
#include <silicathemeiconresolver.h>
#include <sys/stat.h>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QRegularExpression>
#include <QStandardPaths>

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

QString normalizePath(const QString &path)
{
    return QCryptographicHash::hash(path.toLatin1(), QCryptographicHash::Sha1).toHex();
}

QString getDconfFingerprintPath(const QString &path)
{
    return QStringLiteral("/com/dseight/clockwork/fingerprint/%1").arg(normalizePath(path));
}

QString getFileFingerprint(const QString &path)
{
    QByteArray ba = path.toLocal8Bit();
    struct stat sb;

    if (stat(ba.data(), &sb)) {
        qWarning() << "Could not stat" << path;
        return {};
    }

    return QString::number(sb.st_ino, 16) + QString::number(sb.st_size, 16)
           + QString::number(sb.st_mtim.tv_sec, 16) + QString::number(sb.st_mtim.tv_nsec, 16);
}

QString getStoredFingerprint(const QString &path)
{
    MGConfItem dconf(getDconfFingerprintPath(path));
    return dconf.value("<unknown>").toString();
}

void storeFingerprint(const QString &path, const QString &fingerprint)
{
    MGConfItem dconf(getDconfFingerprintPath(path));
    dconf.set(fingerprint);
}

bool isOurIcon(const QString &iconPath)
{
    return getFileFingerprint(iconPath) == getStoredFingerprint(iconPath);
}

QString getIconBackupPath(const QString &iconPath)
{
    const auto dataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    return QStringLiteral("%1/clockwork/backup/%2").arg(dataPath, normalizePath(iconPath));
}

void backupIcon(const QString &iconPath)
{
    const auto iconBackupPath = getIconBackupPath(iconPath);

    // Create backup directory if not exist yet
    QFileInfo(iconBackupPath).dir().mkpath(".");

    QFile::remove(iconBackupPath);
    QFile::copy(iconPath, iconBackupPath);
}

void restoreIcon(const QString &iconPath)
{
    const auto iconBackupPath = getIconBackupPath(iconPath);
    QFile::remove(iconPath);
    QFile::copy(iconBackupPath, iconPath);
}

bool touchFile(const QString &path)
{
    QFile file(path);

    // Do not create new file, only update existing
    if (!file.exists() || !file.open(QIODevice::Append))
        return false;

    return futimens(file.handle(), NULL) == 0;
}

QString resolveIconPath(const QString &iconId)
{
    // Using direct path to icon instead of ID
    if (iconId.startsWith("/"))
        return iconId;

    Silica::ThemeIconResolver iconResolver;
    auto resolvedPath = iconResolver.resolvePath(iconId);

    // First try to use icon with proper scale, because ThemeIconResolver
    // returns 86x86 icon for third-party apps
    if (resolvedPath.contains("86x86")) {
        const auto iconSize = Silica::Theme::instance()->iconSizeLauncher();
        const auto size = QStringLiteral("%1x%1").arg(QString::number(iconSize, 'f', 0));

        QString scaledPath(resolvedPath);
        scaledPath.replace("86x86", size);

        if (QFile::exists(scaledPath))
            return scaledPath;
    }

    return resolvedPath;
}

bool isAlienDalvikIcon(const QString &iconPath)
{
    return iconPath.startsWith("/var/lib/apkd/");
}

bool isMonitoredIcon(const QString &iconPath)
{
    static QRegularExpression re("/usr/share/icons/hicolor/\\w+/apps/.*");
    return re.match(iconPath).hasMatch();
}

} // namespace

IconUpdaterPrivate::IconUpdaterPrivate(IconProvider *provider, const QString &desktopPath)
    : provider(provider)
    , desktopPath(desktopPath)
{
    MDesktopEntry desktopEntry(desktopPath);
    iconPath = resolveIconPath(desktopEntry.icon());
    monitoredIcon = isMonitoredIcon(iconPath);
    alienDalvikIcon = isAlienDalvikIcon(iconPath);
}

void IconUpdaterPrivate::updateMonitoredIcon()
{
    if (iconPath.isEmpty()) {
        qWarning() << "Icon path for" << desktopPath << "is unresolved, ignoring update request";
        return;
    }

    if (!isOurIcon(iconPath))
        backupIcon(iconPath);

    if (!QFile::remove(iconPath)) {
        qDebug() << "Could not remove" << iconPath;
        return;
    }

    const auto size = qRound(Silica::Theme::instance()->iconSizeLauncher());
    QImage icon = provider->requestImage({size, size});

    if (!icon.save(iconPath)) {
        qWarning() << "Could not save new icon to" << iconPath << ", icon update aborted";
        return;
    }

    if (!touchFile(desktopPath)) {
        qWarning() << "Could not touch desktop file" << desktopPath << ", icon update aborted";
        return;
    }

    storeFingerprint(iconPath, getFileFingerprint(iconPath));

    qDebug() << "Updated icon for" << desktopPath;
}

void IconUpdaterPrivate::restoreMonitoredIcon()
{
    if (iconPath.isEmpty() || !isOurIcon(iconPath))
        return;

    restoreIcon(iconPath);
    touchFile(desktopPath);

    qDebug() << "Restored icon for" << desktopPath;
}

void IconUpdaterPrivate::updateNonMonitoredIcon()
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

    qDebug() << "Updated non-monitored icon for" << desktopPath;
}

void IconUpdaterPrivate::restoreNonMonitoredIcon()
{
    const auto iconPath = storedIconPath(desktopPath);
    if (iconPath.isEmpty())
        return;

    DesktopEntry desktop(desktopPath);

    const auto currentIconPath = desktop.icon();
    const auto isOurIconPath = currentIconPath.startsWith("/usr/share/clockwork");

    if (!isOurIconPath) {
        qDebug() << "Desktop file" << desktopPath
                 << "was updated or removed, no need to restore non-monitored icon";
        return;
    }

    desktop.setIcon(iconPath);
    desktop.save();
    QFile::remove(currentIconPath);

    qDebug() << "Restored non-monitored icon for" << desktopPath;
}

IconUpdater::IconUpdater(IconProvider *provider, const QString &desktopPath, QObject *parent)
    : QObject(parent)
    , d_ptr(new IconUpdaterPrivate(provider, desktopPath))
{
    connect(provider, &IconProvider::imageUpdated, this, &IconUpdater::update);

    if (d_ptr->alienDalvikIcon) {
        connect(AlienDalvikWatcher::instance(), &AlienDalvikWatcher::serviceStateChanged,
                this, &IconUpdater::update);
    }

    update();
}

IconUpdater::~IconUpdater()
{
    if (d_ptr->monitoredIcon) {
        d_ptr->restoreMonitoredIcon();
    } else {
        d_ptr->restoreNonMonitoredIcon();
    }
}

void IconUpdater::update()
{
    if (d_ptr->monitoredIcon) {
        d_ptr->updateMonitoredIcon();
    } else {
        d_ptr->updateNonMonitoredIcon();
    }
}
