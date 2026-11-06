# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcm_kwin_scripts_x11\")

kcmutils_add_qml_kcm(gxde-kcm_kwin_scripts_x11
    INSTALL_NAMESPACE plasma/kcms/systemsettings
    DISABLE_DESKTOP_FILE_GENERATION)
gxde_generate_kcm_desktop_file(gxde-kcm_kwin_scripts_x11 kcm_kwin_scripts_x11.json)

target_sources(gxde-kcm_kwin_scripts_x11 PRIVATE
    module.cpp
    kwinscriptsdata.cpp
)

target_link_libraries(gxde-kcm_kwin_scripts_x11 PRIVATE
    Qt::DBus

    KF6::I18n
    KF6::KCMUtilsCore
    KF6::KCMUtils
    KF6::Package
    KF6::KCMUtilsQuick
)

install(FILES kwinscripts.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} RENAME gxde-kwinscripts-x11.knsrc)
