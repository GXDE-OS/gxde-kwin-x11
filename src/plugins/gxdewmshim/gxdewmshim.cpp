/*
    SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gxdewmshim.h"

#include <QDBusConnection>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QProcess>
#include <QTimer>

Q_LOGGING_CATEGORY(KWIN_GXDEWMSHIM, "kwin_gxdewmshim", QtWarningMsg)

namespace KWin
{

static const QString s_dbusPath = QStringLiteral("/GxdeWmShim");

// deepin-wm-dbus returns -1 when com.deepin.wm already has an owner.
static constexpr int s_helperNameTakenExitCode = 255;
static constexpr int s_maxHelperRestarts = 5;

GxdeWmShim::GxdeWmShim()
{
    QDBusConnection::sessionBus().registerObject(s_dbusPath, this, QDBusConnection::ExportScriptableSlots);

    if (isEnabledForSession()) {
        startHelper();
    }
}

GxdeWmShim::~GxdeWmShim()
{
    QDBusConnection::sessionBus().unregisterObject(s_dbusPath);

    if (m_helper) {
        m_helper->disconnect(this);
        if (m_helper->state() != QProcess::NotRunning) {
            m_helper->terminate();
            if (!m_helper->waitForFinished(2000)) {
                m_helper->kill();
                m_helper->waitForFinished(1000);
            }
        }
    }
}

bool GxdeWmShim::isEnabledForSession()
{
    // GXDE_KWIN_X11_WM_SHIM=1 forces the helper on, =0 turns it off.
    const QByteArray override = qgetenv("GXDE_KWIN_X11_WM_SHIM");
    if (!override.isEmpty()) {
        return override != "0";
    }

    const QStringList desktops = qEnvironmentVariable("XDG_CURRENT_DESKTOP").split(QLatin1Char(':'), Qt::SkipEmptyParts);
    for (const QString &desktop : desktops) {
        if (desktop.compare(QLatin1String("GXDE"), Qt::CaseInsensitive) == 0
            || desktop.compare(QLatin1String("Deepin"), Qt::CaseInsensitive) == 0
            || desktop.compare(QLatin1String("DDE"), Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

void GxdeWmShim::startHelper()
{
    const QString program = QStringLiteral(GXDE_WM_DBUS_HELPER);
    if (!QFileInfo(program).isExecutable()) {
        qCWarning(KWIN_GXDEWMSHIM) << "Window manager compatibility helper not found:" << program;
        return;
    }

    if (!m_helper) {
        m_helper = new QProcess(this);
        m_helper->setProcessChannelMode(QProcess::ForwardedChannels);
        connect(m_helper, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
            handleHelperFinished(exitCode, exitStatus);
        });
    }
    m_helper->start(program, {});
}

void GxdeWmShim::handleHelperFinished(int exitCode, int exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == s_helperNameTakenExitCode) {
        qCWarning(KWIN_GXDEWMSHIM) << "com.deepin.wm is owned by another process, not starting the compatibility helper";
        return;
    }
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        return;
    }
    if (m_restarts >= s_maxHelperRestarts) {
        qCWarning(KWIN_GXDEWMSHIM) << "Window manager compatibility helper keeps failing, giving up";
        return;
    }

    ++m_restarts;
    qCWarning(KWIN_GXDEWMSHIM) << "Window manager compatibility helper exited unexpectedly, restarting" << exitCode << exitStatus;
    QTimer::singleShot(1000 * m_restarts, this, &GxdeWmShim::startHelper);
}

bool GxdeWmShim::StartDetached(const QString &program, const QVariantList &arguments)
{
    if (program.isEmpty()) {
        return false;
    }

    if (arguments.isEmpty()) {
        // Like deepin-kwin's startDetached(command): the hot zone value can be
        // a whole command line.
        QStringList command = QProcess::splitCommand(program);
        if (command.isEmpty()) {
            return false;
        }
        const QString executable = command.takeFirst();
        return QProcess::startDetached(executable, command);
    }

    QStringList args;
    args.reserve(arguments.size());
    for (const QVariant &argument : arguments) {
        args << argument.toString();
    }
    return QProcess::startDetached(program, args);
}

} // namespace KWin

#include "moc_gxdewmshim.cpp"
