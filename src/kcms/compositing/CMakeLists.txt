#########################################################################
# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcmkwincompositing_x11\")

################# configure checks and create the configured files #################

set(kwincompositing_SRC
    main.cpp
    kwincompositingdata.cpp
)

kconfig_add_kcfg_files(kwincompositing_SRC kwincompositing_setting.kcfgc GENERATE_MOC)

qt_add_dbus_interface(kwincompositing_SRC
    ${PROJECT_SOURCE_DIR}/src/org.kde.kwin.Compositing.xml kwin_compositing_interface
)

ki18n_wrap_ui(kwincompositing_SRC compositing.ui)

kcoreaddons_add_plugin(gxde-kwincompositing_x11 SOURCES ${kwincompositing_SRC} INSTALL_NAMESPACE "plasma/kcms/systemsettings_qwidgets")
gxde_generate_kcm_desktop_file(gxde-kwincompositing_x11 kwincompositing.json)

target_link_libraries(gxde-kwincompositing_x11
    Qt::DBus
    Qt::Widgets

    KF6::ConfigCore
    KF6::CoreAddons
    KF6::I18n
    KF6::KCMUtils
    KF6::WindowSystem
)
