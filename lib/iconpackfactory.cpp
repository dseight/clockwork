#include "iconpackfactory.h"
#include "harbourthemepack.h"

QList<IconPack *> IconPackFactory::loadIconPacks()
{
    return HarbourThemePack::loadAll();
}
