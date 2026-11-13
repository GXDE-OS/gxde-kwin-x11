var registeredBorders = [];

function init() {
    for (var i in registeredBorders) {
        unregisterScreenEdge(registeredBorders[i]);
    }

    registeredBorders = [];

    if (readConfig("Enabled", "true").toString() === "false") {
        return;
    }

    var borders = readConfig("BorderActivate", "").toString().split(",");
    for (var i in borders) {
        var border = parseInt(borders[i]);

        if (isFinite(border)) {
            var onBorderActive = function () {
                var window = workspace.activeWindow;
                // 热区快捷方式只允许关闭最大化窗口
                // KWin 6: maximizeMode 3 is MaximizeFull (vertical | horizontal).
                if (window && window.closeable && window.maximizeMode === 3)
                    window.closeWindow();
            }

            registerScreenEdge(border, onBorderActive)
        }
    }
}

options.configChanged.connect(init);
init();

