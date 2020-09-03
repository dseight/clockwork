#ifndef ALIENDALVIKWATCHER_H
#define ALIENDALVIKWATCHER_H

#include <QDBusInterface>

class AlienDalvikWatcher : public QObject
{
    Q_OBJECT
    explicit AlienDalvikWatcher(QObject *parent = nullptr);

public:
    static AlienDalvikWatcher *instance();

signals:
    void serviceStateChanged();

private slots:
    void handleJobRemoval(quint32 id,
                          const QDBusObjectPath &job,
                          const QString &unit,
                          const QString &result);

private:
    QDBusInterface m_systemdInterface;
};

#endif // ALIENDALVIKWATCHER_H
