#include "clockworkglobal_p.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include "iconupdater.h"
#include <MDesktopEntry>
#include <MGConfItem>
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QHash>
#include <QSocketNotifier>
#include <QUrl>

namespace {

int sigtermFd[2];
QSocketNotifier *termNotifier;

QHash<QString, MGConfItem *> applicationConfItems;
QHash<QString, DynamicIcon *> dynamicIcons;
QHash<QString, IconPack *> iconPacks;
QHash<QString, IconUpdater *> updaters;

void termSignalHandler(int)
{
    char a = 1;
    if (write(sigtermFd[0], &a, 1) != 1) {
        /* XXX: is there anything to do here? */
    }
}

void setupSignalHandlers()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
        qFatal("Could not create TERM socketpair");

    termNotifier = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read);
    QObject::connect(termNotifier, &QSocketNotifier::activated, []() {
        termNotifier->setEnabled(false);

        char tmp;
        if (read(sigtermFd[1], &tmp, 1) != 1) {
            /* XXX: is there anything to do here? */
        }

        qApp->quit();

        termNotifier->setEnabled(true);
    });

    struct sigaction term;

    term.sa_handler = termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, 0))
        qFatal("Could not setup signal handler");
}

MGConfItem *currentIconPackConf()
{
    static const auto path = QStringLiteral("/com/dseight/clockwork/icon-pack");
    static auto *conf = new MGConfItem(path);
    return conf;
}

MGConfItem *applicationConf(const QString &desktopPath)
{
    auto conf = applicationConfItems.value(desktopPath, nullptr);

    if (conf)
        return conf;

    QFileInfo desktopInfo(desktopPath);
    const auto dconfPath = QStringLiteral("/com/dseight/clockwork/applications/%1/provider")
                               .arg(desktopInfo.baseName());

    conf = new MGConfItem(dconfPath);
    applicationConfItems[desktopPath] = conf;

    return conf;
}

QUrl getProviderUri(const QString &desktopPath)
{
    // Try to find application-specific settings first
    const auto applicationProvider = applicationConf(desktopPath)->value().toString();

    if (!applicationProvider.isEmpty())
        return applicationProvider;

    // Then look at globally set icon pack
    const auto iconPack = currentIconPackConf()->value().toString();

    if (!iconPack.isEmpty())
        return QUrl("icon-pack://" + iconPack);

    return {};
}

IconUpdater *createIconPackUpdater(const QString &name,
                                   const QString &desktopPath,
                                   const QString &iconId = QString())
{
    const auto iconPack = iconPacks.value(name, nullptr);

    if (!iconPack)
        return nullptr;

    if (iconId.isEmpty()) {
        const auto defaultIconId = iconPack->iconByDesktopPath(desktopPath);
        return defaultIconId.isEmpty() ? nullptr
                                       : iconPack->iconUpdater(desktopPath, defaultIconId);
    } else {
        return iconPack->iconUpdater(desktopPath, iconId);
    }
}

IconUpdater *createDynamicIconUpdater(const QString &name)
{
    const auto dynamicIcon = dynamicIcons.value(name, nullptr);
    return dynamicIcon ? dynamicIcon->iconUpdater() : nullptr;
}

IconUpdater *createIconUpdater(const QString &desktopPath)
{
    const auto uri = getProviderUri(desktopPath);
    const auto scheme = uri.scheme();

    if (scheme == QStringLiteral("icon-pack")) {
        return createIconPackUpdater(uri.host(), desktopPath, uri.path());
    } else if (scheme == QStringLiteral("dynamic-icon")) {
        return createDynamicIconUpdater(uri.host());
    }

    return nullptr;
}

void rebuildIconUpdaters()
{
    for (auto updater : qAsConst(updaters))
        delete updater;

    QDir dir(QStringLiteral("/usr/share/applications"));
    const auto infoList = dir.entryInfoList({"*.desktop"}, QDir::Files);
    for (const auto &info : infoList) {
        const auto desktopPath = info.absoluteFilePath();
        MDesktopEntry desktopEntry(desktopPath);

        if (desktopEntry.noDisplay())
            continue;

        updaters[desktopPath] = createIconUpdater(desktopPath);
    }
}

} // namespace

int main(int argc, char *argv[])
{
    // Use QGuiApplication instead of QCoreApplication because QCoreApplication
    // fails to find system fonts and crashes on SVG with text.
    QGuiApplication app(argc, argv);

    // We should handle SIGTERM gracefully, otherwise icons will not be restored
    setupSignalHandlers();

    QObject::connect(&app, &QGuiApplication::aboutToQuit, &app, []() {
        for (auto updater : qAsConst(updaters))
            delete updater;
    });

    // Watch for global icon changes
    QObject::connect(currentIconPackConf(), &MGConfItem::valueChanged, rebuildIconUpdaters);

    // Watch for per-application icon changes
    QDir dir(QStringLiteral("/usr/share/applications"));
    const auto infoList = dir.entryInfoList({"*.desktop"}, QDir::Files);
    for (const auto &info : infoList) {
        const auto desktopPath = info.absoluteFilePath();
        QObject::connect(applicationConf(desktopPath), &MGConfItem::valueChanged, [=]() {
            delete updaters[desktopPath];
            updaters[desktopPath] = createIconUpdater(desktopPath);
        });
    }

    for (auto icon : loadDynamicIcons())
        dynamicIcons[icon->name()] = icon;

    for (auto iconPack : IconPackFactory::loadIconPacks())
        iconPacks[iconPack->name()] = iconPack;

    rebuildIconUpdaters();

    return app.exec();
}
