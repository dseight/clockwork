#ifndef DEVICELOCKSTATUS_H
#define DEVICELOCKSTATUS_H

#include <QDBusInterface>

class DevicelockStatus : public QObject
{
    Q_OBJECT

public:
    explicit DevicelockStatus(QObject *parent = nullptr);

signals:
    void unlocked();
    void locked();

private slots:
    void tklockModeChanged(const QString &mode);

private:
    QDBusInterface m_mceInterface;
};

#endif // DEVICELOCKSTATUS_H
