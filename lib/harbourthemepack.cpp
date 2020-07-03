#include "harbourthemepack.h"
#include <silicatheme.h>
#include <QCollator>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>

namespace {

QString findIcon(const QString &basePath,
                 const QString &iconName,
                 const QString &primaryScheme,
                 const QString &subPath)
{
    auto primaryPath = basePath + primaryScheme + subPath + "/" + iconName + ".png";

    if (QFile::exists(primaryPath))
        return primaryPath;

    QDir dir(basePath);
    auto entries = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    // Use natural reverse sort, so larger scale icon will be preferred
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(entries.begin(), entries.end(), [&](const QString &s1, const QString &s2) {
        return collator.compare(s1, s2) > 0;
    });

    for (const auto &entry : entries) {
        auto path = dir.absoluteFilePath(entry) + subPath + "/" + iconName + ".png";

        if (QFile::exists(path))
            return path;
    }

    qDebug() << "No icon with name" << iconName << "found";
    return {};
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
    m_scale = "z" + QString::number(pixelRatio, 'f', 2);

    // z1.00 must be truncated to z1.0
    if (m_scale.endsWith('0'))
        m_scale.chop(1);

    const auto iconSize = Silica::Theme::instance()->iconSizeLauncher();
    m_size = QStringLiteral("%1x%1").arg(QString::number(iconSize, 'f', 0));

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

        const auto iconId = QDir(m_path).relativeFilePath(iconPath);
        m_previewIcons.append(iconId);
    }
}

QString HarbourThemePack::displayName()
{
    return m_displayName;
}

QStringList HarbourThemePack::icons()
{
    return {};
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

    return QDir(m_path).relativeFilePath(path);
}

QString HarbourThemePack::iconByActivity(const QString &activity)
{
    const auto normalized = QString(activity).replace('.', "_").replace('/', "-");
    const auto iconName = "apkd_launcher_" + normalized;
    const auto path = findApkIcon(iconName);
    return QDir(m_path).relativeFilePath(path);
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
    return findIcon(m_path + "/jolla/", iconName, m_scale, "/icons");
}

QString HarbourThemePack::findNativeIcon(const QString &iconName)
{
    return findIcon(m_path + "/native/", iconName, m_size, "/apps");
}

QString HarbourThemePack::findApkIcon(const QString &iconName)
{
    return findIcon(m_path + "/apk/", iconName, m_size, QString());
}
