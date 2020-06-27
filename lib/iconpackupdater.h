#ifndef ICONPACKUPDATER_H
#define ICONPACKUPDATER_H

#include "clockworkglobal.h"
#include <QObject>

class IconPack;

class CLOCKWORK_EXPORT IconPackUpdater : public QObject
{
    Q_OBJECT

public:
    explicit IconPackUpdater(IconPack *iconPack);
    ~IconPackUpdater() override;
};

#endif // ICONPACKUPDATER_H
