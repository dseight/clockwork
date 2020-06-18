#ifndef DYNAMICICON_H
#define DYNAMICICON_H

#include "clockworkglobal.h"
#include <QObject>

class IconProvider;
class IconUpdater;
class DynamicIconPrivate;

class CLOCKWORK_EXPORT DynamicIcon : public QObject
{
    Q_OBJECT

public:
    /* When subclassing DynamicIcon, do not forget to add Q_INVOKABLE
     * to constructor. */
    DynamicIcon(const QString &packageName, const QString &name, QObject *parent = nullptr);

    /* Name of this application dynamic icon. Note that is *must* be unique,
     * because it's possible to have multiple dynamic icons for a single
     * application (only one of them may be enabled though). */
    QString name();

    /* Application package name (e.g. "jolla-clock"). */
    QString packageName();

    /* Application name, as displayed in launcher (e.g. "Clock"). */
    QString displayName();

    /* Returns whether application for this icon available or not.
     * Application considered unavailable when it is not installed
     * (e.g. there is no corresponding desktop file). */
    bool available();

    /* If dynamic icon enabled, corresponding icon updater will be started
     * on daemon launch. */
    bool enabled();

    /* Enable or disable this dynamic icon. */
    void setEnabled(bool enabled);

    /* Provider for this dynamic icon. */
    IconProvider *iconProvider();

    /* Updater for this dynamic icon. */
    IconUpdater *iconUpdater();

protected:
    /* Override this with custom icon provider class. */
    virtual IconProvider *iconProvider(QObject *parent) = 0;

    /* Override this with custom icon updater class.
     * Generic IconUpdater is used by default. */
    virtual IconUpdater *iconUpdater(IconProvider *provider,
                                     const QString &desktopPath,
                                     QObject *parent);

signals:
    void availableChanged();
    void enabledChanged();

private:
    DynamicIconPrivate *d_ptr;
};

CLOCKWORK_EXPORT
void registerDynamicIconMeta(const QMetaObject &meta);

#define REGISTER_DYNAMIC_ICON(dynamicIcon) \
    static void __attribute__((__constructor__)) __register_##dynamicIcon(void) \
    { \
        static_assert(std::is_constructible<dynamicIcon, QObject *>::value, \
                      #dynamicIcon " has no (QObject *) constructor"); \
        registerDynamicIconMeta(dynamicIcon::staticMetaObject); \
    }

#endif // DYNAMICICON_H
