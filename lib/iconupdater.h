#ifndef ICONUPDATER_H
#define ICONUPDATER_H

#include "clockworkglobal.h"
#include <QObject>
#include <QScopedPointer>

class IconProvider;
class IconUpdaterPrivate;

class CLOCKWORK_EXPORT IconUpdater : public QObject
{
    Q_OBJECT

public:
    IconUpdater(IconProvider *provider, const QString &desktopPath, QObject *parent = nullptr);
    ~IconUpdater() override;

public slots:
    /* Request icon update.
     * Useful when subclass wants to update icon dynamically. */
    void update();

private:
    QScopedPointer<IconUpdaterPrivate> d_ptr;
};

#endif // ICONUPDATER_H
