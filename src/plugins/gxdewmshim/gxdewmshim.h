/*
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "plugin.h"

#include <QObject>
#include <QVariantList>

class QProcess;

namespace KWin
{

/**
 * Integrates gxde-wm-shim (util/gxde-wm-shim/ in this source tree) with gxde-kwin_x11.
 *
 * - Starts the com.deepin.wm compatibility helper in GXDE sessions and stops it
 *   together with KWin. The helper is deliberately not D-Bus activated, so it
 *   can coexist with the deepin-wm-dbus of the existing GXDE window manager.
 * - Exports StartDetached() for the ported screen edge scripts, replacing the
 *   workspace.__dde__.startDetached() helper that only deepin-kwin provided.
 */
class GxdeWmShim : public Plugin
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.gxde.KWinX11.WmShim")

public:
    explicit GxdeWmShim();
    ~GxdeWmShim() override;

public Q_SLOTS:
    // Signature "sav": KWin's callDBus() passes JavaScript arrays as QVariantList.
    Q_SCRIPTABLE bool StartDetached(const QString &program, const QVariantList &arguments);

private:
    static bool isEnabledForSession();
    void startHelper();
    void handleHelperFinished(int exitCode, int exitStatus);

    QProcess *m_helper = nullptr;
    int m_restarts = 0;
};

} // namespace KWin
