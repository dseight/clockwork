#ifndef CLOCKWORKIMAGEPROVIDER_H
#define CLOCKWORKIMAGEPROVIDER_H

#include <QQuickImageProvider>

class IconProvider;

class ClockworkImageProvider : public QQuickImageProvider
{
public:
    ClockworkImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QHash<QString, IconProvider *> m_providers;
};

#endif // CLOCKWORKIMAGEPROVIDER_H
