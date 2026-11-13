# SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>
#
# SPDX-License-Identifier: BSD-3-Clause

kcoreaddons_add_plugin(gxdewmshim
    SOURCES
        gxdewmshim.cpp
        main.cpp
    INSTALL_NAMESPACE "${KWIN_PLUGINDIR}/plugins"
)
target_compile_definitions(gxdewmshim PRIVATE
    GXDE_WM_DBUS_HELPER="${KDE_INSTALL_FULL_LIBEXECDIR}/${GXDE_KWIN_NAME}-wm-dbus"
)
target_link_libraries(gxdewmshim kwin Qt::DBus)
