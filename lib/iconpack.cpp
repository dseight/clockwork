#include "iconpack.h"
#include "iconpack_p.h"
#include "iconpackupdater.h"
#include "svgiconrender.h"
#include <MDesktopEntry>
#include <MGConfItem>
#include <silicatheme.h>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QPainter>

namespace {

MGConfItem *currentIconPackConf()
{
    static const auto path = QStringLiteral("/com/dseight/clockwork/icon-pack");
    static MGConfItem *conf = new MGConfItem(path);
    return conf;
}

} // namespace

IconPackPrivate::IconPackPrivate(const QString &name)
    : name(name)
{
}

IconPack::IconPack(const QString &name, QObject *parent)
    : QObject(parent)
    , d_ptr(new IconPackPrivate(name))
{
    connect(currentIconPackConf(), &MGConfItem::valueChanged, this, &IconPack::enabledChanged);
}

IconPack::~IconPack() = default;

QString IconPack::name() const
{
    return d_ptr->name;
}

bool IconPack::enabled() const
{
    return currentIconPackConf()->value() == name();
}

void IconPack::setEnabled(bool enabled)
{
    currentIconPackConf()->set(enabled ? name() : QString());
}

QString IconPack::iconByDesktopPath(const QString &desktopPath)
{
    QFileInfo info(desktopPath);
    const auto desktopBaseName = info.baseName();

    if (desktopBaseName.startsWith("apkd_launcher")) {
        MDesktopEntry desktopEntry(desktopPath);
        const auto activityPath = desktopEntry.exec().section(' ', -1);
        return iconByActivity(activityPath);
    } else {
        return iconByPackageName(desktopBaseName);
    }
}

IconPackUpdater *IconPack::iconPackUpdater()
{
    return new IconPackUpdater(this);
}

QImage IconPack::loadImageFromFile(const QString &path, const QSize &requestedSize)
{
    if (!QFile::exists(path)) {
        qWarning() << "File" << path << "not found. Icon will not be loaded.";
        return {};
    }

    if (path.endsWith(QStringLiteral(".svg"))) {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open" << path << "for reading";
            return {};
        }

        return renderSvgIcon(file.readAll(), requestedSize);
    } else {
        return QImage(path);
    }
}
