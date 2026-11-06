# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcm_kwintabbox_x11\")

set(kcm_kwintabbox_x11_PART_SRCS
    ${PROJECT_SOURCE_DIR}/src/tabbox/tabboxconfig.cpp
    layoutpreview.cpp
    main.cpp
    thumbnailitem.cpp
    kwintabboxconfigform.cpp
    kwintabboxdata.cpp
    shortcutsettings.cpp
)

ki18n_wrap_ui(kcm_kwintabbox_x11_PART_SRCS main.ui)
qt_add_dbus_interface(kcm_kwintabbox_x11_PART_SRCS ${PROJECT_SOURCE_DIR}/src/org.kde.kwin.Effects.xml kwin_effects_interface)

kconfig_add_kcfg_files(kcm_kwintabbox_x11_PART_SRCS kwintabboxsettings.kcfgc kwinswitcheffectsettings.kcfgc)
kcoreaddons_add_plugin(gxde-kcm_kwintabbox_x11 SOURCES ${kcm_kwintabbox_x11_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")

gxde_generate_kcm_desktop_file(gxde-kcm_kwintabbox_x11 kcm_kwintabbox_x11.json)
target_link_libraries(gxde-kcm_kwintabbox_x11
    Qt::Quick
    KF6::GlobalAccel
    KF6::I18n
    KF6::KCMUtils
    KF6::NewStuffWidgets
    KF6::Package
    KF6::Service
    KF6::XmlGui # For kkeysequencewidget
    KF6::WidgetsAddons
)
if (KWIN_BUILD_X11)
    target_link_libraries(gxde-kcm_kwintabbox_x11
        XCB::XCB
    )
endif()

########### install files ###############
install(FILES thumbnails/falkon.png
              thumbnails/kmail.png
              thumbnails/systemsettings.png
              thumbnails/dolphin.png
              thumbnails/desktop.png
        DESTINATION ${KDE_INSTALL_DATADIR}/${KWIN_DATADIR}/kcm_kwintabbox)
install(FILES kwinswitcher.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} RENAME gxde-kwinswitcher-x11.knsrc)
