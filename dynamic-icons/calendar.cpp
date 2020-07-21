#include "dynamicicon.h"
#include "iconpack.h"
#include "iconpackfactory.h"
#include "iconprovider.h"
#include "svgiconrender.h"
#include <MGConfItem>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QTimer>

namespace {

class CalendarIconRenderer
{
public:
    virtual ~CalendarIconRenderer() = default;
    virtual QImage requestImage(const QDate &date, const QSize &requestedSize) = 0;
};

class DefaultCalendarIconRenderer : public CalendarIconRenderer
{
public:
    DefaultCalendarIconRenderer()
    {
        QFile file(QStringLiteral(ASSETS_PATH "/icon-launcher-calendar.svg"));

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open" << file.fileName() << "for reading";
            return;
        }

        m_asset = file.readAll();
    }

    QImage requestImage(const QDate &date, const QSize &requestedSize) override
    {
        return renderSvgIcon(getSvgData(date), requestedSize);
    }

private:
    QByteArray getSvgData(const QDate &date)
    {
        QByteArray asset(m_asset);
        const auto day = ">" + QString::number(date.day()) + "<";

        asset.replace(">31<", day.toLatin1());

        return asset;
    }

private:
    QByteArray m_asset;
};

class IconpackCalendarIconRenderer : public CalendarIconRenderer
{
public:
    explicit IconpackCalendarIconRenderer(const QString &name)
    {
        const auto iconPacks = IconPackFactory::loadIconPacks();
        for (auto iconPack : iconPacks) {
            if (iconPack->name() == name) {
                m_iconPack = iconPack;
            }
        }

        if (m_iconPack == nullptr)
            qDebug() << "No icon pack with name" << name
                     << "found. Dynamic calendar icon will not work.";
    }

    QImage requestImage(const QDate &date, const QSize &requestedSize) override
    {
        if (!m_iconPack)
            return {};

        return m_iconPack->requestCalendarIcon(date, requestedSize);
    }

private:
    IconPack *m_iconPack;
};

class CalendarIconProvider : public IconProvider
{
    Q_OBJECT

public:
    explicit CalendarIconProvider(QObject *parent)
        : IconProvider(parent)
        // TODO: move currentIconPackConf to some common place
        , m_currentIconPackConf(QStringLiteral("/com/dseight/clockwork/icon-pack"))
    {
        updateRenderer();

        connect(&m_currentIconPackConf, &MGConfItem::valueChanged,
                this, &CalendarIconProvider::updateRenderer);
        connect(&m_currentIconPackConf, &MGConfItem::valueChanged,
                this, &IconProvider::imageUpdated);
        connect(&m_timer, &QTimer::timeout, this, &IconProvider::imageUpdated);

        // Update icon each hour
        m_timer.start(60 * 60 * 1000);
    }

    QImage requestImage(const QSize &requestedSize) override
    {
        const auto date = QDate::currentDate();
        return m_renderer->requestImage(date, requestedSize);
    }

private slots:
    void updateRenderer()
    {
        const auto iconPackName = m_currentIconPackConf.value().toString();

        if (iconPackName.isEmpty()) {
            m_renderer.reset(new DefaultCalendarIconRenderer);
        } else {
            m_renderer.reset(new IconpackCalendarIconRenderer(iconPackName));
        }
    }

private:
    QTimer m_timer;
    MGConfItem m_currentIconPackConf;
    QScopedPointer<CalendarIconRenderer> m_renderer;
};

} // namespace

class CalendarDynamicIcon : public DynamicIcon
{
    Q_OBJECT

public:
    Q_INVOKABLE
    explicit CalendarDynamicIcon(QObject *parent = nullptr)
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
};

REGISTER_DYNAMIC_ICON(CalendarDynamicIcon)

#include "calendar.moc"
