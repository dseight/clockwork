#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include "clockworkglobal.h"
#include <QImage>
#include <QObject>

class CLOCKWORK_EXPORT IconProvider : public QObject
{
    Q_OBJECT

public:
    explicit IconProvider(QObject *parent = nullptr);

    virtual QImage requestImage(const QSize &requestedSize) = 0;

signals:
    void imageUpdated();
};

#endif // ICONPROVIDER_H
