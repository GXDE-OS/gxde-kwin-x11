var registeredBorders = [];

function runCommand(program, arguments) {
    print(program, typeof(program));
    print(arguments, typeof(arguments));

    // deepin-kwin's workspace.__dde__.startDetached() is provided on
    // gxde-kwin_x11 by the gxdewmshim KWin plugin. Without arguments the
    // program string is split like a command line, as before.
    callDBus("org.kde.KWin", "/GxdeWmShim", "com.gxde.KWinX11.WmShim", "StartDetached",
             program, (arguments && arguments.length > 0) ? arguments : []);
}

function parseCommand(border) {
    var border_string = String(border)
    var program = readConfig("Border" + border_string + "Program", "").toString();
    var args = readConfig("Border" + border_string + "Args", "").toString();

    if (args) { // 只分割非空字符
        args = args.split(",");
    } else {
        args = [];
    }

    if (!program) {
        return false;
    }

    if (!(args instanceof Array)) {
        if (args instanceof String) {
            if (args) // 只传入非空参数
                args = [args]
        }
    }

    var index = registeredBorders.push({
                                           border: border,
                                           program: program,
                                           args: args
                                       }) - 1;

    var onBorderActive = function () {
        var borderObj = registeredBorders[index];
        runCommand(borderObj.program, borderObj.args)
    }

    registerScreenEdge(border, onBorderActive);

    return true;
}

function init() {
    for (var i in registeredBorders) {
        unregisterScreenEdge(registeredBorders[i].border);
    }

    registeredBorders = [];

    if (readConfig("Enabled", "true").toString() === "false") {
        return;
    }

    var borders = readConfig("BorderActivate", "").toString().split(",");
    for (var i in borders) {
        var border = parseInt(borders[i]);

        if (isFinite(border) && !parseCommand(border)) {
            print("Invalid border set with:" + borders[i]);
        }
    }
}

options.configChanged.connect(init);
init();
