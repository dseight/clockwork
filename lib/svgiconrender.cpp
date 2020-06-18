#include "svgiconrender.h"
#include <silicatheme.h>
#include <QDebug>
#include <QPainter>
#include <QSvgRenderer>

QImage renderSvgIcon(const QByteArray &data, const QSize &requestedSize)
{
    QSize size = requestedSize;

    if (!size.isValid()) {
        const auto defaultSize = qRound(Silica::Theme::instance()->iconSizeLauncher());
        size = {defaultSize, defaultSize};
        qInfo() << "Invalid size provided, falling back to" << size;
    }

    QSvgRenderer renderer(data);
    if (!renderer.isValid()) {
        qWarning() << "Invalid SVG data, rendering aborted";
        qDebug() << "Data:" << data;
        return {};
    }

    QImage image(size, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    renderer.render(&painter);

    return image;
}
