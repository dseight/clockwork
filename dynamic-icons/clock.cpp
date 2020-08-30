#include "devicelockstatus.h"
#include "dynamicicon.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include "iconprovider.h"
#include "svgiconrender.h"
#include <MGConfItem>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QTime>
#include <QTimer>

namespace {

class ClockIconRenderer
{
public:
    virtual ~ClockIconRenderer() = default;
    virtual QImage requestImage(int hoursAngle, int minutesAngle, const QSize &requestedSize) = 0;
};

class DefaultClockIconRenderer : public ClockIconRenderer
{
public:
    DefaultClockIconRenderer()
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

    QImage requestImage(int hoursAngle, int minutesAngle, const QSize &requestedSize) override
    {
        return renderSvgIcon(getSvgData(hoursAngle, minutesAngle), requestedSize);
    }

private:
    QByteArray getSvgData(int hoursAngle, int minutesAngle)
    {
        QByteArray asset(m_asset);

        const auto hoursRotate = "rotate(" + QString::number(hoursAngle);
        asset.replace("rotate(125", hoursRotate.toLatin1());

        const auto minutesRotate = "rotate(" + QString::number(minutesAngle);
        asset.replace("rotate(0", minutesRotate.toLatin1());

        return asset;
    }

private:
    QByteArray m_asset;
};

class IconpackClockIconRenderer : public ClockIconRenderer
{
public:
    explicit IconpackClockIconRenderer(const QString &name)
    {
        const auto iconPacks = IconPackFactory::loadIconPacks();
        for (auto iconPack : iconPacks) {
            if (iconPack->name() == name) {
                m_iconPack = iconPack;
            }
        }

        if (m_iconPack == nullptr)
            qDebug() << "No icon pack with name" << name
                     << "found. Dynamic clock icon will not work.";
    }

    QImage requestImage(int hoursAngle, int minutesAngle, const QSize &requestedSize) override
    {
        if (!m_iconPack)
            return {};

        // Size will not change frequently, so it's worth to cache images
        if (m_lastRequestedSize != requestedSize) {
            m_lastRequestedSize = requestedSize;
            m_clockDial = m_iconPack->requestClockDialIcon(requestedSize);
            m_hoursHand = m_iconPack->requestHoursHandIcon(requestedSize);
            m_minutesHand = m_iconPack->requestMinutesHandIcon(requestedSize);
        }

        QImage image(m_clockDial);

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        QPoint center(image.width() / 2, image.height() / 2);

        painter.translate(center);

        painter.save();
        painter.rotate(hoursAngle);
        painter.drawImage(QPoint(-m_hoursHand.width() / 2, -m_hoursHand.height() / 2), m_hoursHand);
        painter.restore();

        painter.rotate(minutesAngle);
        painter.drawImage(QPoint(-m_minutesHand.width() / 2, -m_minutesHand.height() / 2),
                          m_minutesHand);

        return image;
    }

private:
    IconPack *m_iconPack;
    QSize m_lastRequestedSize;
    QImage m_clockDial;
    QImage m_hoursHand;
    QImage m_minutesHand;
};

class ClockIconProvider : public IconProvider
{
    Q_OBJECT

public:
    explicit ClockIconProvider(QObject *parent)
        : IconProvider(parent)
        // TODO: move currentIconPackConf to some common place
        , m_currentIconPackConf(QStringLiteral("/com/dseight/clockwork/icon-pack"))
    {
        updateRenderer();

        connect(&m_currentIconPackConf, &MGConfItem::valueChanged,
                this, &ClockIconProvider::updateRenderer);
        connect(&m_currentIconPackConf, &MGConfItem::valueChanged,
                this, &IconProvider::imageUpdated);
        connect(&m_timer, &QTimer::timeout, this, &IconProvider::imageUpdated);
        connect(&m_devicelockStatus, &DevicelockStatus::unlocked, this, &IconProvider::imageUpdated);

        // Update icon each minute
        // TODO: use timed as source of update events, look at
        // nemo-qml-plugin-time-qt5 implementation
        m_timer.start(60 * 1000);
    }

    QImage requestImage(const QSize &requestedSize) override
    {
        const auto time = QTime::currentTime();
        return m_renderer->requestImage(hoursAngle(time), minutesAngle(time), requestedSize);
    }

private slots:
    void updateRenderer()
    {
        const auto iconPackName = m_currentIconPackConf.value().toString();

        if (iconPackName.isEmpty()) {
            m_renderer.reset(new DefaultClockIconRenderer);
        } else {
            m_renderer.reset(new IconpackClockIconRenderer(iconPackName));
        }
    }

private:
    static int hoursAngle(const QTime &time)
    {
        const auto minutes = (time.hour() % 12) * 60 + time.minute();
        return 360 * minutes / (12 * 60);
    }

    static int minutesAngle(const QTime &time)
    {
        return 360 * time.minute() / 60;
    }

private:
    QTimer m_timer;
    DevicelockStatus m_devicelockStatus;
    MGConfItem m_currentIconPackConf;
    QScopedPointer<ClockIconRenderer> m_renderer;
};

} // namespace

class ClockDynamicIcon : public DynamicIcon
{
    Q_OBJECT

public:
    Q_INVOKABLE
    explicit ClockDynamicIcon(QObject *parent = nullptr)
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
