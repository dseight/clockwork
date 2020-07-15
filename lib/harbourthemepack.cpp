#include "harbourthemepack.h"
#include <silicatheme.h>
#include <QCollator>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>

namespace {

void loadIconsFromPath(const QString &path, const QString &scheme, QHash<QString, QString> &icons)
{
    QDir dir(path);
    auto entries = dir.entryInfoList({"*.png"}, QDir::Files);

    for (const auto &entry : entries) {
        if (!icons.contains(entry.completeBaseName())) {
            icons[entry.completeBaseName()] = scheme;
        }
    }
}

void loadIcons(const QString &basePath,
               const QString &primaryScheme,
               const QString &subPath,
               QHash<QString, QString> &icons)
{
    const auto primaryPath = basePath + primaryScheme + subPath;

    if (QFile::exists(primaryPath))
        loadIconsFromPath(primaryPath, primaryScheme, icons);

    QDir dir(basePath);
    auto entries = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    // Use natural reverse sort, so larger scale icon will be preferred
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(entries.begin(), entries.end(), [&](const QString &s1, const QString &s2) {
        return collator.compare(s1, s2) > 0;
    });

    for (const auto &entry : entries)
        loadIconsFromPath(dir.absoluteFilePath(entry) + subPath, entry, icons);
}

} // namespace

HarbourThemePack::HarbourThemePack(const QString &path, QObject *parent)
    : IconPack(path.section('/', -1), parent)
    , m_path(path)
{
    QFile packageFile(path + "/package");
    if (packageFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_displayName = packageFile.readLine().trimmed();
    } else {
        m_displayName = name();
    }

    const auto pixelRatio = Silica::Theme::instance()->pixelRatio();
    auto scale = "z" + QString::number(pixelRatio, 'f', 2);

    // z1.00 must be truncated to z1.0
    if (scale.endsWith('0'))
        scale.chop(1);

    const auto iconSize = Silica::Theme::instance()->iconSizeLauncher();
    const auto size = QStringLiteral("%1x%1").arg(QString::number(iconSize, 'f', 0));

    loadIcons(m_path + "/jolla/", scale, "/icons", m_jollaIcons);
    loadIcons(m_path + "/native/", size, "/apps", m_nativeIcons);
    loadIcons(m_path + "/apk/", size, QString(), m_apkIcons);

    static const QStringList suggestedPreviewIcons = {
        "icon-launcher-messaging",
        "icon-launcher-weather",
        "icon-launcher-camera",
        "icon-launcher-browser",
        "icon-launcher-clock",
        "icon-launcher-calendar",
    };

    for (const auto &iconName : suggestedPreviewIcons) {
        const auto iconPath = findJollaIcon(iconName);

        if (iconPath.isEmpty())
            continue;

        m_previewIcons.append(iconPath);
    }
}

QString HarbourThemePack::displayName()
{
    return m_displayName;
}

QStringList HarbourThemePack::icons()
{
    QStringList icons;

    for (const auto &iconName : m_jollaIcons.keys())
        icons += findJollaIcon(iconName);

    for (const auto &iconName : m_nativeIcons.keys())
        icons += findNativeIcon(iconName);

    for (const auto &iconName : m_apkIcons.keys())
        icons += findApkIcon(iconName);

    return icons;
}

QStringList HarbourThemePack::previewIcons()
{
    return m_previewIcons;
}

QString HarbourThemePack::iconByPackageName(const QString &packageName)
{
    const static QHash<QString, QString> jollaPackages = {
        {"fingerterm", "icon-launcher-shell"},
        {"jolla-calculator", "icon-launcher-calculator"},
        {"jolla-calendar", "icon-launcher-calendar"},
        {"jolla-camera", "icon-launcher-camera"},
        {"jolla-clock", "icon-launcher-clock"},
        {"jolla-contacts", "icon-launcher-people"},
        {"jolla-email", "icon-launcher-email"},
        {"jolla-gallery", "icon-launcher-gallery"},
        {"jolla-mediaplayer", "icon-launcher-mediaplayer"},
        {"jolla-messages", "icon-launcher-messaging"},
        {"jolla-notes", "icon-launcher-notes"},
        {"jolla-settings", "icon-launcher-settings"},
        {"sailfish-browser", "icon-launcher-browser"},
        {"sailfish-office", "icon-launcher-office"},
        {"sailfish-tutorial", "icon-launcher-tutorial"},
        {"sailfish-weather", "icon-launcher-weather"},
        {"store-client", "icon-launcher-store"},
        {"voicecall-ui", "icon-launcher-phone"},
    };

    QString path;

    if (jollaPackages.contains(packageName)) {
        path = findJollaIcon(jollaPackages.value(packageName));
    } else {
        path = findNativeIcon(packageName);
    }

    return path;
}

QString HarbourThemePack::iconByActivity(const QString &activity)
{
    const auto normalized = QString(activity).replace('.', "_").replace('/', "-");
    const auto iconName = "apkd_launcher_" + normalized;
    return findApkIcon(iconName);
}

QImage HarbourThemePack::requestIcon(const QString &iconId, const QSize &requestedSize)
{
    return loadImageFromFile(m_path + "/" + iconId, requestedSize);
}

QList<IconPack *> HarbourThemePack::loadAll()
{
    static QList<IconPack *> themePacks;

    if (!themePacks.isEmpty())
        return themePacks;

    QDir dir("/usr/share", "harbour-themepack-*");
    const auto themePacksInfo = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const auto &info : themePacksInfo) {
        const auto path = info.absoluteFilePath();

        QDir themePackDir(path);
        const auto dirs = themePackDir.entryList(QDir::Dirs);
        const bool hasIcons = dirs.contains("apk") || dirs.contains("jolla")
                              || dirs.contains("native");

        if (!hasIcons) {
            qDebug() << "Theme pack" << info.baseName() << "doesn't contain icons, skipping";
            continue;
        }

        themePacks.append(new HarbourThemePack(path));
    }

    return themePacks;
}

QString HarbourThemePack::findJollaIcon(const QString &iconName)
{
    const auto scale = m_jollaIcons[iconName];

    if (scale.isEmpty())
        return {};

    return "jolla/" + scale + "/icons/" + iconName + ".png";
}

QString HarbourThemePack::findNativeIcon(const QString &iconName)
{
    const auto size = m_nativeIcons[iconName];

    if (size.isEmpty())
        return {};

    return "native/" + size + "/apps/" + iconName + ".png";
}

QString HarbourThemePack::findApkIcon(const QString &iconName)
{
    const auto size = m_apkIcons[iconName];

    if (size.isEmpty())
        return {};

    return "apk/" + size + "/" + iconName + ".png";
}
