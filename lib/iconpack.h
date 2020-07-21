#ifndef ICONPACK_H
#define ICONPACK_H

#include "clockworkglobal.h"
#include <QObject>
#include <QScopedPointer>

class IconUpdater;
class IconPackPrivate;

class CLOCKWORK_EXPORT IconPack : public QObject
{
    Q_OBJECT

public:
    explicit IconPack(const QString &name, QObject *parent = nullptr);
    ~IconPack() override;

    /* Name of this icon pack. */
    QString name() const;

    /* Human-readable name of icon pack. */
    virtual QString displayName() = 0;

    /* List of icons IDs provided by this icon pack.
     * Names must be valid for requestIcon() call. */
    virtual QStringList icons() = 0;

    /* List of icons IDs to display on icon preview.
     * Names must be valid for requestIcon() call. */
    virtual QStringList previewIcons() = 0;

    /* Icon ID for native application, e.g. "jolla-clock". */
    virtual QString iconByPackageName(const QString &packageName) = 0;

    /* Icon ID for Android applications by path to activity,
     * e.g. "org.telegram.messenger/org.telegram.ui.LaunchActivity". */
    virtual QString iconByActivity(const QString &activity) = 0;

    /* Get icon with specified id and size. */
    virtual QImage requestIcon(const QString &iconId, const QSize &requestedSize) = 0;

    /* Get parts of dynamic clock icon. */
    virtual QImage requestClockDialIcon(const QSize &requestedSize) = 0;
    virtual QImage requestHoursHandIcon(const QSize &requestedSize) = 0;
    virtual QImage requestMinutesHandIcon(const QSize &requestedSize) = 0;

    /* If icon pack enabled, it will be applied on daemon launch. */
    bool enabled() const;

    /* Enable or disable this icon pack. */
    void setEnabled(bool enabled);

    /* Icon ID for application by its desktop path. Automatically detects
     * whether provided application is native or Android. */
    QString iconByDesktopPath(const QString &desktopPath);

    /* Updater for specific application with icon from this pack.
     * Returns nullptr if there is no icon with specified id. */
    IconUpdater *iconUpdater(const QString &desktopPath, const QString &iconId);

signals:
    void enabledChanged();

protected:
    /* Load icon (either .png or .svg) from specified file, and scale it
     * to requested size. */
    static QImage loadImageFromFile(const QString &path, const QSize &requestedSize);

private:
    QScopedPointer<IconPackPrivate> d_ptr;
};

#endif // ICONPACK_H
