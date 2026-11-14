/*
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "x11eventfilter.h"

#include <QObject>
#include <QVariantMap>

#include <memory>
#include <xcb/xcb.h>

class KSelectionWatcher;

namespace KWin
{

/**
 * Parses data in the XSETTINGS wire format
 * (https://specifications.freedesktop.org/xsettings-spec/0.5/). Integers map
 * to int, strings to QByteArray and colors to QColor.
 *
 * Besides the settings manager, dxcb uses the format for per-window settings:
 * DTK windows in no-titlebar mode publish their decoration theme (radius,
 * border, shadow...) in the _XSETTINGS_SETTINGS property of the window.
 */
QVariantMap parseXSettings(const QByteArray &data);

/**
 * Follows the settings of the XSETTINGS manager of screen 0, which GXDE uses
 * for desktop-wide values such as DTK/WindowRadius and Xft/DPI.
 */
class GxdeXSettingsWatcher : public QObject, public X11EventFilter
{
    Q_OBJECT
public:
    explicit GxdeXSettingsWatcher(QObject *parent = nullptr);
    ~GxdeXSettingsWatcher() override;

    QVariant value(const QByteArray &name) const;

    bool event(xcb_generic_event_t *event) override;

Q_SIGNALS:
    void changed();

private:
    void setOwner(xcb_window_t owner);
    void readSettings();

    std::unique_ptr<KSelectionWatcher> m_selectionWatcher;
    xcb_atom_t m_settingsAtom = XCB_ATOM_NONE;
    xcb_window_t m_owner = XCB_WINDOW_NONE;
    QVariantMap m_settings;
};

} // namespace KWin
