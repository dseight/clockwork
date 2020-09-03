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

    void updateThirdPartyIcon();
    void restoreThirdPartyIcon();

    IconProvider *provider;
    QString desktopPath;
    QString iconPath;
    bool themeIcon;
    bool alienDalvikIcon;
};

#endif // ICONUPDATER_P_H
