/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "config-kwin.h"
#include "core/inputdevice.h"

#include <kglobalaccel_interface.h>

#include <QObject>

class KGlobalAccelImpl : public KGlobalAccelInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KGlobalAccelInterface_iid FILE "kwin.json")
    Q_INTERFACES(KGlobalAccelInterface)

public:
    KGlobalAccelImpl(QObject *parent = nullptr);
    ~KGlobalAccelImpl() override;

    bool grabKey(int key, bool grab) override;
#if !HAVE_KGLOBALACCELD_INTERFACE_METHOD
    void setEnabled(bool enabled) override;
#endif

public Q_SLOTS:
#if HAVE_KGLOBALACCELD_INTERFACE_METHOD
    bool checkKeyPressed(int keyQt, KWin::KeyboardKeyState state);
#else
    bool checkKeyPressed(int keyQt);
    bool checkKeyReleased(int keyQt);
#endif
    bool checkPointerPressed(Qt::MouseButtons buttons);
    bool checkAxisTriggered(int axis);

#if !HAVE_KGLOBALACCELD_INTERFACE_METHOD
private:
    bool m_shuttingDown = false;
    QMetaObject::Connection m_inputDestroyedConnection;
#endif
};
