#include "wmadaptor.h"
#include "deepinwmfaker.h"

#include <QDBusConnectionInterface>
#include <QGuiApplication>

#define Service "com.deepin.wm"
#define Path "/com/deepin/wm"
#define Interface "com.deepin.wm"


//在wayland下，kwindowsystem默认使用wayland平台的接口，但目前wayland的接口currentDesktop()未实现，所以设置环境变量转为使用xcb平台的接口
__attribute__((constructor))void init()
{
    qputenv("QT_QPA_PLATFORM", "xcb");
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("DeepinWMFaker");


    DeepinWMFaker facker;
    WmAdaptor adapter(&facker);
    Q_UNUSED(adapter)

    if (!QDBusConnection::sessionBus().registerObject(Path, Interface, &facker)) {
        return -2;
    }

    // --queue: wait for com.deepin.wm instead of giving up when it has an
    // owner. gxde-kwin_x11 uses it to take the name over from another
    // deepin-wm-dbus without a gap in which D-Bus could activate that one again.
    if (app.arguments().contains(QStringLiteral("--queue"))) {
        const QDBusReply<QDBusConnectionInterface::RegisterServiceReply> reply =
            QDBusConnection::sessionBus().interface()->registerService(Service, QDBusConnectionInterface::QueueService,
                                                                      QDBusConnectionInterface::DontAllowReplacement);
        if (!reply.isValid()) {
            return -1;
        }
    } else if (!QDBusConnection::sessionBus().registerService(Service)) {
        return -1;
    }

    return app.exec();
}
