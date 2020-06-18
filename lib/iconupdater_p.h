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

    void updateThemeIcon();
    void restoreThemeIcon();

    IconProvider *provider;
    QString desktopPath;
};

#endif // ICONUPDATER_P_H
