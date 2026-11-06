# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcm_kwin_effects_x11\")

########### next target ###############

set(kcm_kwin_effects_x11_PART_SRCS
    kcm.cpp
    effectsfilterproxymodel.cpp
    desktopeffectsdata.cpp
)

kcmutils_add_qml_kcm(gxde-kcm_kwin_effects_x11
    SOURCES ${kcm_kwin_effects_x11_PART_SRCS}
    DISABLE_DESKTOP_FILE_GENERATION)
gxde_generate_kcm_desktop_file(gxde-kcm_kwin_effects_x11 kcm_kwin_effects_x11.json)

target_link_libraries(gxde-kcm_kwin_effects_x11 PRIVATE
    Qt::DBus
    Qt::Quick

    KF6::KCMUtils
    KF6::I18n
    KF6::KCMUtils
    KF6::KCMUtilsQuick
    KF6::XmlGui

    kcmkwincommon
)

install(FILES kwineffect.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} RENAME gxde-kwineffect-x11.knsrc)
