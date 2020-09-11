#ifndef ICONUPDATER_P_H
#define ICONUPDATER_P_H

#include "clockworkglobal.h"
#include <QObject>

class IconUpdater;
class IconProvider;

class IconUpdaterPrivate
{
public:
    IconUpdaterPrivate(IconProvider *provider, const QString &desktopPath);

    void updateNonMonitoredIcon();
    void restoreNonMonitoredIcon();

    void updateMonitoredIcon();
    void restoreMonitoredIcon();

    IconProvider *provider;
    QString desktopPath;
    QString iconPath;
    bool monitoredIcon;
    bool alienDalvikIcon;
};

#endif // ICONUPDATER_P_H
