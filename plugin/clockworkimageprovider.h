#ifndef CLOCKWORKIMAGEPROVIDER_H
#define CLOCKWORKIMAGEPROVIDER_H

#include <QQuickImageProvider>

class IconProvider;
class IconPack;

class ClockworkImageProvider : public QQuickImageProvider
{
public:
    ClockworkImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    QImage dynamicIcon(const QString &path, const QSize &requestedSize);
    QImage iconFromPack(const QString &path, const QSize &requestedSize);

private:
    QHash<QString, IconProvider *> m_dynamicIconProviders;
    QHash<QString, IconPack *> m_iconPacks;
};

#endif // CLOCKWORKIMAGEPROVIDER_H
