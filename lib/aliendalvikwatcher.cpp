#include "aliendalvikwatcher.h"

AlienDalvikWatcher::AlienDalvikWatcher(QObject *parent)
    : QObject(parent)
    , m_systemdInterface("org.freedesktop.systemd1",
                         "/org/freedesktop/systemd1",
                         "org.freedesktop.systemd1.Manager",
                         QDBusConnection::systemBus())
{
    connect(&m_systemdInterface, SIGNAL(JobRemoved(quint32, QDBusObjectPath, QString, QString)),
            this, SLOT(handleJobRemoval(quint32, QDBusObjectPath, QString, QString)));
}

AlienDalvikWatcher *AlienDalvikWatcher::instance()
{
    static auto watcher = new AlienDalvikWatcher;
    return watcher;
}

void AlienDalvikWatcher::handleJobRemoval(quint32,
                                          const QDBusObjectPath &,
                                          const QString &unit,
                                          const QString &result)
{
    if (unit != QStringLiteral("aliendalvik.service"))
        return;

    if (result == QStringLiteral("done"))
        // FIXME: this indicates that service was either started or stopped
        // successfully. But we need to update icon only after aliendalvik *start*.
        // So, it's better to watch for PropertiesChanged signal and especially
        // for ActiveState property of aliendalvik unit.
        emit serviceStateChanged();
}
