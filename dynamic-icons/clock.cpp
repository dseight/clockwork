#include "dynamicicon.h"
#include "iconprovider.h"
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
        connect(&m_timer, &QTimer::timeout, this, &IconProvider::imageUpdated);

        // Update icon each minute
        // TODO: use timed as source of update events, look at
        // nemo-qml-plugin-time-qt5 implementation
        m_timer.start(60 * 1000);

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
    QTimer m_timer;
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
};

REGISTER_DYNAMIC_ICON(ClockDynamicIcon)

#include "clock.moc"
