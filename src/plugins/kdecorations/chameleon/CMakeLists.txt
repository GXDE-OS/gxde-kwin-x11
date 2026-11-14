# SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>
#
# SPDX-License-Identifier: BSD-3-Clause

# Installed below the gxde-kwin_x11 plugin directory instead of the shared
# org.kde.kdecoration3 namespace: it links the GXDE build of KDecoration 3 and
# must not be picked up by Plasma's KWin, and Plasma's decorations must not be
# picked up by gxde-kwin_x11.
kcoreaddons_add_plugin(gxde_chameleon
    SOURCES
        chameleon.cpp
        chameleonbutton.cpp
        chameleonshadow.cpp
        chameleontheme.cpp
        main.cpp
        themes.qrc
    INSTALL_NAMESPACE "${KWIN_PLUGINDIR}/org.kde.kdecoration3"
)
set_target_properties(gxde_chameleon PROPERTIES OUTPUT_NAME "com.deepin.chameleon")
target_compile_definitions(gxde_chameleon PRIVATE
    CHAMELEON_KWIN_CONFIG="${KWIN_CONFIG}"
)
target_link_libraries(gxde_chameleon
    KDecoration3::KDecoration
    KF6::ConfigCore
    KF6::CoreAddons
    Qt::Gui
)
