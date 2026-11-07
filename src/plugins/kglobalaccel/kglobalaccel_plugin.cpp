/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2015 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "kglobalaccel_plugin.h"

#include "input.h"

#include <QDebug>

KGlobalAccelImpl::KGlobalAccelImpl(QObject *parent)
    : KGlobalAccelInterface(parent)
{
}

KGlobalAccelImpl::~KGlobalAccelImpl() = default;

bool KGlobalAccelImpl::grabKey(int key, bool grab)
{
    return true;
}

#if HAVE_KGLOBALACCELD_INTERFACE_METHOD
bool KGlobalAccelImpl::checkKeyPressed(int keyQt, KWin::KeyboardKeyState state)
{
    switch (state) {
    case KWin::KeyboardKeyState::Pressed:
        return keyEvent(keyQt, ShortcutKeyState::Pressed);
    case KWin::KeyboardKeyState::Repeated:
        return keyEvent(keyQt, ShortcutKeyState::Repeated);
    case KWin::KeyboardKeyState::Released:
        return keyEvent(keyQt, ShortcutKeyState::Released);
    }

    return false;
}
#else
void KGlobalAccelImpl::setEnabled(bool enabled)
{
    if (m_shuttingDown) {
        return;
    }
    static KWin::InputRedirection *input = KWin::InputRedirection::self();
    if (!input) {
        qFatal("This plugin is intended to be used with KWin and this is not KWin, exiting now");
    } else if (!m_inputDestroyedConnection) {
        m_inputDestroyedConnection = connect(input, &QObject::destroyed, this, [this] {
            m_shuttingDown = true;
        });
    }
    input->registerGlobalAccel(enabled ? this : nullptr);
}

bool KGlobalAccelImpl::checkKeyPressed(int keyQt)
{
    return keyPressed(keyQt);
}

bool KGlobalAccelImpl::checkKeyReleased(int keyQt)
{
    return keyReleased(keyQt);
}
#endif

bool KGlobalAccelImpl::checkPointerPressed(Qt::MouseButtons buttons)
{
    return pointerPressed(buttons);
}

bool KGlobalAccelImpl::checkAxisTriggered(int axis)
{
    return axisTriggered(axis);
}

#include "moc_kglobalaccel_plugin.cpp"
