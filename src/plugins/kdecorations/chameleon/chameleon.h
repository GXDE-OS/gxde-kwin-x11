/*
    SPDX-FileCopyrightText: 2017 - 2019 Deepin Technology Co., Ltd.
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
// KDecoration3 port of deepin-kwin's deepin-chameleon decoration. Unlike the
// deepin-kwin version it does not reach into KWin internals. gxde-kwin_x11
// hands it everything X11 specific as dynamic properties:
//
//  - on the decoration: gxdeDeepinNoTitlebar (_DEEPIN_NO_TITLEBAR) and
//    gxdeDeepinWindowTheme (the per-window theme dxcb publishes);
//  - on the shared DecorationSettings: gxdeDeepinWindowRadius (DTK/WindowRadius
//    of the XSETTINGS manager).
//
// Rounded corners use KDecoration3's border radius and outline, which KWin
// applies to the client while compositing; deepin-kwin needed its scissor
// effect for that.

#pragma once

#include "chameleontheme.h"

#include <KDecoration3/Decoration>

#include <QIcon>
#include <QVariantMap>

namespace KDecoration3
{
class DecorationButtonGroup;
}

class Chameleon : public KDecoration3::Decoration
{
    Q_OBJECT
public:
    explicit Chameleon(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Chameleon() override;

    bool init() override;
    void paint(QPainter *painter, const QRectF &repaintArea) override;
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    QIcon minimizeIcon() const;
    QIcon maximizeIcon() const;
    QIcon unmaximizeIcon() const;
    QIcon closeIcon() const;
    qreal menuIconSize() const;

private:
    UIWindowType windowType() const;
    qreal scale() const;
    bool noTitleBar() const;
    qreal titleBarHeight() const;
    qreal borderWidth() const;
    qreal windowRadius() const;
    bool windowNeedBorder() const;
    QColor textColor() const;
    QColor backgroundColor() const;

    void updateTheme();
    void updateActive();
    void applyWindowTheme(ChameleonTheme::ThemeConfig &config) const;
    void updateTitleBarArea();
    void updateButtonsGeometry();
    void positionButtons();
    void updateTitleGeometry();
    void updateBorderGeometry();
    void updateShadow();
    void updateResizeOnlyBorders();

    ChameleonTheme::ConfigGroup m_configGroup;
    ChameleonTheme::ThemeConfig m_config;
    QVariantMap m_windowTheme;

    KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
    KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;

    bool m_initialized = false;
    bool m_noTitleBar = false;
    QRectF m_titleArea;
    QString m_title;
};
