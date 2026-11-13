// Ported to gxde-kwin_x11 (KWin 6): deepin-kwin's workspace.__dde__.kwinUtils is
// not available, so maximize through the KWin scripting API directly.
registerShortcut("Window Absolute Maximize", "Window Absolute Maximize", "Meta+Up", function() {
    var window = workspace.activeWindow;
    if (window && window.maximizable)
        window.setMaximize(true, true);
})
registerShortcut("Window Unmaximize", "Window Unmaximize", "Meta+Down", function() {
    var window = workspace.activeWindow;
    if (window && window.maximizable)
        window.setMaximize(false, false);
})
