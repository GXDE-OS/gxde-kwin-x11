option(USE_KWIN_NO_SCALE "use deepin LD_PRELOAD kwin_no_scale" ON) # deepin v25 don't need kwin_no_scale

if (INSTALL_KWIN_CONFIG)
    install(FILES kwinrc DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/xdg" RENAME "${KWIN_CONFIG_FILE}")
endif()
install(FILES kwinrulesrc DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/xdg" RENAME "${KWIN_RULES_CONFIG_FILE}")

if (INSTALL_KDE_GLOBAL_CONFIGS)
    install(
        FILES
        kglobalshortcutsrc
        klaunchrc
        kdeglobals
        DESTINATION
        "${CMAKE_INSTALL_SYSCONFDIR}/xdg"
    )
endif()

# Default shortcuts reported by deepin-wm-dbus (GetDefaultAccel).
install(FILES kglobalshortcutsrc DESTINATION "${DEEPIN_WM_DBUS_SHORTCUTS_DIR}")

if (INSTALL_DEEPIN_KWIN_DCONFIG)
    file(GLOB KWIN_DCONFIG_ITEMS CONFIGURE_DEPENDS *.json)

    install(
        FILES
        ${KWIN_DCONFIG_ITEMS}
        DESTINATION
        "${CMAKE_INSTALL_DATADIR}/dsg/configs/org.kde.kwin"
    )

    install(
        FILES
        org.kde.kwin.decoration.titlebar.json
        DESTINATION
        "${CMAKE_INSTALL_DATADIR}/dsg/configs/org.kde.kwin.decoration"
    )

    install(
        FILES
        org.deepin.kwin.json
        DESTINATION
        "${CMAKE_INSTALL_DATADIR}/deepin-log-viewer/deepin-log.conf.d/"
    )
endif()

if (USE_KWIN_NO_SCALE)
    configure_file(kwin_no_scale.in kwin_no_scale)
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/kwin_no_scale DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
