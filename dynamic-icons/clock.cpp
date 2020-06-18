#include "dynamicicon.h"
#include "iconprovider.h"
#include "iconupdater.h"
#include "svgiconrender.h"
#include <QDebug>
#include <QFile>
#include <QTime>
#include <QTimer>

class ClockIconProvider : public IconProvider
{
    Q_OBJECT

public:
    ClockIconProvider(QObject *parent)
        : IconProvider(parent)
    {
        // TODO: add dark icon variant and make it possible for user to
        // choose icon variant from settings
        QFile file(QStringLiteral(ASSETS_PATH "/icon-launcher-clock.svg"));

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
        const auto time = QTime::currentTime();

        const auto hoursRotate = "rotate(" + QString::number(hoursAngle(time));
        asset.replace("rotate(125", hoursRotate.toLatin1());

        const auto minutesRotate = "rotate(" + QString::number(minutesAngle(time));
        asset.replace("rotate(0", minutesRotate.toLatin1());

        return asset;
    }

    int hoursAngle(const QTime &time)
    {
        const auto minutes = (time.hour() % 12) * 60 + time.minute();
        return 360 * minutes / (12 * 60);
    }

    int minutesAngle(const QTime &time)
    {
        return 360 * time.minute() / 60;
    }

private:
    QByteArray m_asset;
};

class ClockIconUpdater : public IconUpdater
{
    Q_OBJECT

public:
    ClockIconUpdater(IconProvider *provider, const QString &desktopPath, QObject *parent)
        : IconUpdater(provider, desktopPath, parent)
        , m_timer(new QTimer(this))
    {
        connect(m_timer, &QTimer::timeout, this, &ClockIconUpdater::update);

        // Update icon each minute
        m_timer->start(60 * 1000);

        // TODO: use timed as source of update events, look at
        // nemo-qml-plugin-time-qt5 implementation
    }

private:
    QTimer *m_timer;
};

class ClockDynamicIcon : public DynamicIcon
{
    Q_OBJECT

public:
    Q_INVOKABLE
    ClockDynamicIcon(QObject *parent = nullptr)
        : DynamicIcon(QStringLiteral("jolla-clock"), QStringLiteral("clockwork-jolla-clock"), parent)
    {
    }

protected:
    IconProvider *iconProvider(QObject *parent) override
    {
        return new ClockIconProvider(parent);
    }

    IconUpdater *iconUpdater(IconProvider *provider,
                             const QString &desktopPath,
                             QObject *parent) override
    {
        return new ClockIconUpdater(provider, desktopPath, parent);
    }
};

REGISTER_DYNAMIC_ICON(ClockDynamicIcon)

#include "clock.moc"
