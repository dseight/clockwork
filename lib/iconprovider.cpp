#include "iconprovider.h"

IconProvider::IconProvider(QObject *parent)
    : QObject(parent)
{
}

// XXX: maybe it's worth to add possibility to propagate changes up from the
// icon provider? E.g. if provider will be internally configurable by DConf,
// it would be better to see changes on Settings page immediately rather than
// a minute later.
