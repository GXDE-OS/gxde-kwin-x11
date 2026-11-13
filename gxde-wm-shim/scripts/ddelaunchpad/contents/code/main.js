// SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Ported to gxde-kwin_x11 (KWin 6 scripting API): geometry is frameGeometry,
// clientAdded/clientList are windowAdded/windowList, and geometries are
// value types that must not be modified in place.
function rectContains(rect1, rect2)
{
    if (rect1.x != rect2.x)
        return false;
    if (rect1.y != rect2.y)
        return false;
    if (rect1.x + rect1.width != rect2.x + rect2.width)
        return false;
    if (rect1.y + rect1.height != rect2.y + rect2.height)
        return false;

    return true;
}

function forceFullScreen(client) {
    var newGeometry = client.frameGeometry;
    var screenGeometry = workspace.clientArea(KWin.ScreenArea, client);

    if (!rectContains(screenGeometry, newGeometry)) {
        client.frameGeometry = screenGeometry;
    }
}

function setupConnection(client) {
    if (client.caption == "Fullscreen Launchpad") {
        forceFullScreen(client)
        client.frameGeometryChanged.connect(function () {
            forceFullScreen(client);
        });
    }
}

workspace.windowAdded.connect(setupConnection);
// connect all existing clients
var clients = workspace.windowList();
for (var i=0; i<clients.length; i++) {
    setupConnection(clients[i]);
}
