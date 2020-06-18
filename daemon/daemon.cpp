#include "clockworkglobal_p.h"
#include "dynamicicon.h"
#include "dynamicicon_p.h"
#include "iconupdater.h"
#include <csignal>
#include <sys/socket.h>
#include <unistd.h>
#include <QDebug>
#include <QGuiApplication>
#include <QHash>
#include <QSocketNotifier>

namespace {

int sigtermFd[2];
QSocketNotifier *termNotifier;

QHash<DynamicIcon *, IconUpdater *> dynamicIconUpdaters;

void dynamicIconAvailabilityChanged(DynamicIcon *icon)
{
    bool enable = icon->available() && icon->enabled();

    if (enable && dynamicIconUpdaters[icon] == nullptr) {
        dynamicIconUpdaters[icon] = icon->iconUpdater();
    } else if (!enable && dynamicIconUpdaters[icon] != nullptr) {
        delete dynamicIconUpdaters[icon];
        dynamicIconUpdaters[icon] = nullptr;
    }
}

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

} // namespace

int main(int argc, char *argv[])
{
    // Use QGuiApplication instead of QCoreApplication because QCoreApplication
    // fails to find system fonts and crashes on SVG with text.
    QGuiApplication app(argc, argv);

    // We should handle SIGTERM gracefully, otherwise icons will not be restored
    setupSignalHandlers();

    QObject::connect(&app, &QGuiApplication::aboutToQuit, &app, []() {
        for (auto updater : qAsConst(dynamicIconUpdaters)) {
            delete updater;
        }
    });

    for (auto icon : loadDynamicIcons()) {
        if (icon->available() && icon->enabled()) {
            dynamicIconUpdaters[icon] = icon->iconUpdater();
        }
        QObject::connect(icon, &DynamicIcon::availableChanged, [=]() {
            dynamicIconAvailabilityChanged(icon);
        });
        QObject::connect(icon, &DynamicIcon::enabledChanged, [=]() {
            dynamicIconAvailabilityChanged(icon);
        });
    }

    return app.exec();
}
