#include "dynamicicon.h"
#include "iconprovider.h"
#include "iconupdater.h"
#include "svgiconrender.h"
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QTimer>

class CalendarIconProvider : public IconProvider
{
    Q_OBJECT

public:
    CalendarIconProvider(QObject *parent)
        : IconProvider(parent)
    {
        QFile file(QStringLiteral(ASSETS_PATH "/icon-launcher-calendar.svg"));

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open" << file.fileName() << "for reading";
            return;
        }

        m_asset = file.readAll();
    }

    QImage requestImage(const QSize &requestedSize) override
    {
        return renderSvgIcon(getSvgData(), requestedSize);
    }

private:
    QByteArray getSvgData()
    {
        // It might be better to parse DOM when more features will be added.
        // But at current state it is just unnecessarily increases complexity.

        QByteArray asset(m_asset);

        const auto date = QDate::currentDate();
        const auto day = ">" + QString::number(date.day()) + "<";

        asset.replace(">31<", day.toLatin1());

        return asset;
    }

private:
    QByteArray m_asset;
};

class CalendarIconUpdater : public IconUpdater
{
    Q_OBJECT

public:
    CalendarIconUpdater(IconProvider *provider, const QString &desktopPath, QObject *parent)
        : IconUpdater(provider, desktopPath, parent)
        , m_timer(new QTimer(this))
    {
        connect(m_timer, &QTimer::timeout, this, &CalendarIconUpdater::update);

        // Update icon each hour
        m_timer->start(60 * 60 * 1000);
    }

private:
    QTimer *m_timer;
};

class CalendarDynamicIcon : public DynamicIcon
{
    Q_OBJECT

public:
    Q_INVOKABLE
    CalendarDynamicIcon(QObject *parent = nullptr)
        : DynamicIcon(QStringLiteral("jolla-calendar"),
                      QStringLiteral("clockwork-jolla-calendar"),
                      parent)
    {
    }

protected:
    IconProvider *iconProvider(QObject *parent) override
    {
        return new CalendarIconProvider(parent);
    }

    IconUpdater *iconUpdater(IconProvider *provider,
                             const QString &desktopPath,
                             QObject *parent) override
    {
        return new CalendarIconUpdater(provider, desktopPath, parent);
    }
};

REGISTER_DYNAMIC_ICON(CalendarDynamicIcon)

#include "calendar.moc"
