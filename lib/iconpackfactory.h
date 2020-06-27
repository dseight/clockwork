#ifndef ICONPACKFACTORY_H
#define ICONPACKFACTORY_H

#include "clockworkglobal.h"
#include <QList>

class IconPack;

class CLOCKWORK_EXPORT IconPackFactory
{
public:
    static QList<IconPack *> loadIconPacks();
};

#endif // ICONPACKFACTORY_H
