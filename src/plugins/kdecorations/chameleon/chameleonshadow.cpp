/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleonshadow.h"

#include <QPainter>
#include <QRadialGradient>

#include <cmath>

Q_GLOBAL_STATIC(ChameleonShadow, s_globalShadow)

ChameleonShadow *ChameleonShadow::instance()
{
    return s_globalShadow;
}

std::shared_ptr<KDecoration3::DecorationShadow> ChameleonShadow::getShadow(const ChameleonTheme::ThemeConfig &config, bool drawBorder)
{
    const QPointF windowRadius = config.radius;
    const QPointF shadowOffset = config.shadowConfig.shadowOffset;
    const QColor shadowColor = config.shadowConfig.shadowColor;
    const qreal borderWidth = drawBorder ? config.borderConfig.borderWidth : 0;
    const QColor borderColor = config.borderConfig.borderColor;

    const bool noShadow = shadowColor.alpha() == 0 || qFuzzyIsNull(config.shadowConfig.shadowRadius);
    const bool noBorder = borderColor.alpha() == 0 || qFuzzyIsNull(borderWidth);
    if (noShadow && noBorder) {
        return nullptr;
    }

    // The shadow cut-out removes the rounded window corners, so the tile has
    // to be larger than the shadow radius by the window radius.
    const int shadowSize = std::ceil(config.shadowConfig.shadowRadius + windowRadius.x() + windowRadius.y() + borderWidth);
    const QPointF shadowOverlap(qMax(windowRadius.x(), 3.0), qMax(windowRadius.y(), 3.0));
    const QMarginsF paddings(shadowSize - shadowOffset.x() - shadowOverlap.x(),
                             shadowSize - shadowOffset.y() - shadowOverlap.y(),
                             shadowSize - shadowOverlap.x(),
                             shadowSize - shadowOverlap.y());

    const QString key = QStringLiteral("%1,%2|%3|%4,%5|%6|%7|%8")
                            .arg(windowRadius.x())
                            .arg(windowRadius.y())
                            .arg(config.shadowConfig.shadowRadius)
                            .arg(shadowOffset.x())
                            .arg(shadowOffset.y())
                            .arg(shadowColor.name(QColor::HexArgb))
                            .arg(borderWidth)
                            .arg(borderColor.name(QColor::HexArgb));
    if (auto cached = m_shadowCache.value(key)) {
        return cached;
    }

    QImage image(2 * shadowSize, 2 * shadowSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (!noShadow) {
        // Gaussian falloff sampled into a radial gradient.
        const auto alpha = [](qreal x) {
            return std::exp(-x * x / 0.15);
        };
        const qreal strength = shadowColor.alpha();
        QRadialGradient gradient(shadowSize, shadowSize, shadowSize);
        for (int i = 0; i < 10; ++i) {
            const qreal x = qreal(i) / 9;
            QColor c = shadowColor;
            c.setAlpha(qBound(0, int(alpha(x) * strength * 0.6), 255));
            gradient.setColorAt(x, c);
        }
        QColor transparent = shadowColor;
        transparent.setAlpha(0);
        gradient.setColorAt(1, transparent);
        painter.fillRect(image.rect(), gradient);
    }

    // The window rectangle inside the tile.
    const QRectF innerRect(shadowSize - shadowOffset.x() - shadowOverlap.x(),
                           shadowSize - shadowOffset.y() - shadowOverlap.y(),
                           shadowOffset.x() + 2 * shadowOverlap.x(),
                           shadowOffset.y() + 2 * shadowOverlap.y());
    const bool rounded = windowRadius.x() > 0 && windowRadius.y() > 0;

    // Cut the window out of the shadow so translucent windows do not show it.
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    if (rounded) {
        painter.drawRoundedRect(innerRect, windowRadius.x(), windowRadius.y());
    } else {
        painter.drawRect(innerRect);
    }

    if (!noBorder) {
        // Border just outside the window edge.
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setPen(QPen(borderColor, borderWidth));
        painter.setBrush(Qt::NoBrush);
        const qreal half = borderWidth / 2;
        const QRectF borderRect = innerRect.adjusted(-half, -half, half, half);
        if (rounded) {
            painter.drawRoundedRect(borderRect, windowRadius.x() + half, windowRadius.y() + half);
        } else {
            painter.drawRect(borderRect);
        }
    }
    painter.end();

    auto shadow = std::make_shared<KDecoration3::DecorationShadow>();
    shadow->setPadding(paddings);
    shadow->setInnerShadowRect(QRectF(shadowSize, shadowSize, 1, 1));
    shadow->setShadow(image);

    m_shadowCache.insert(key, shadow);
    return shadow;
}

void ChameleonShadow::clearCache()
{
    m_shadowCache.clear();
}
