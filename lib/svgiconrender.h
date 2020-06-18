#ifndef SVGICONRENDER_H
#define SVGICONRENDER_H

#include "clockworkglobal.h"
#include <QImage>

CLOCKWORK_EXPORT
QImage renderSvgIcon(const QByteArray &data, const QSize &requestedSize);

#endif // SVGICONRENDER_H
