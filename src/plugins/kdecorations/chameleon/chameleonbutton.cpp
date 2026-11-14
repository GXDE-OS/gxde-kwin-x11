/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "chameleonbutton.h"
#include "chameleon.h"

#include <KDecoration3/DecoratedWindow>

#include <QPainter>

using KDecoration3::DecorationButtonType;

ChameleonButton::ChameleonButton(DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
    KDecoration3::DecoratedWindow *w = decoration->window();

    switch (type) {
    case DecorationButtonType::Menu:
        break;
    case DecorationButtonType::Minimize:
        setVisible(w->isMinimizeable());
        connect(w, &KDecoration3::DecoratedWindow::minimizeableChanged, this, &ChameleonButton::setVisible);
        break;
    case DecorationButtonType::Maximize:
        setVisible(w->isMaximizeable());
        connect(w, &KDecoration3::DecoratedWindow::maximizeableChanged, this, &ChameleonButton::setVisible);
        break;
    case DecorationButtonType::Close:
        setVisible(w->isCloseable());
        connect(w, &KDecoration3::DecoratedWindow::closeableChanged, this, &ChameleonButton::setVisible);
        break;
    default:
        // The chameleon themes only provide the buttons above.
        setVisible(false);
        break;
    }
}

KDecoration3::DecorationButton *ChameleonButton::create(DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
{
    return new ChameleonButton(type, decoration, parent);
}

void ChameleonButton::paint(QPainter *painter, const QRectF &repaintArea)
{
    Q_UNUSED(repaintArea)

    auto *deco = qobject_cast<Chameleon *>(decoration());
    if (!deco || !isVisible()) {
        return;
    }

    QIcon::Mode mode = QIcon::Normal;
    if (!isEnabled()) {
        mode = QIcon::Disabled;
    } else if (isPressed()) {
        mode = QIcon::Selected;
    } else if (isHovered()) {
        mode = QIcon::Active;
    }

    // Icons are square and centred in the button.
    const qreal length = std::min(geometry().width(), geometry().height());
    QRectF rect(0, 0, length, length);
    rect.moveCenter(geometry().center());

    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    switch (type()) {
    case DecorationButtonType::Menu: {
        // Application icon, a bit smaller than the button.
        const qreal iconSize = std::min<qreal>(length, deco->menuIconSize());
        QRectF iconRect(0, 0, iconSize, iconSize);
        iconRect.moveCenter(geometry().center());
        deco->window()->icon().paint(painter, iconRect.toAlignedRect());
        break;
    }
    case DecorationButtonType::Minimize:
        deco->minimizeIcon().paint(painter, rect.toAlignedRect(), Qt::AlignCenter, mode);
        break;
    case DecorationButtonType::Maximize:
        if (isChecked()) {
            deco->unmaximizeIcon().paint(painter, rect.toAlignedRect(), Qt::AlignCenter, mode);
        } else {
            deco->maximizeIcon().paint(painter, rect.toAlignedRect(), Qt::AlignCenter, mode);
        }
        break;
    case DecorationButtonType::Close:
        deco->closeIcon().paint(painter, rect.toAlignedRect(), Qt::AlignCenter, mode);
        break;
    default:
        break;
    }

    painter->restore();
}

#include "moc_chameleonbutton.cpp"
