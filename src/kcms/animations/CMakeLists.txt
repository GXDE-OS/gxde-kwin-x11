add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcm_animations_x11\")

set(kcm_animations_x11_PART_SRCS
    kcm.cpp
    animationsdata.cpp
    effectssubsetmodel.cpp
)

kcmutils_add_qml_kcm(gxde-kcm_animations_x11
    SOURCES ${kcm_animations_x11_PART_SRCS}
    DISABLE_DESKTOP_FILE_GENERATION)
gxde_generate_kcm_desktop_file(gxde-kcm_animations_x11 kcm_animations_x11.json)

kconfig_add_kcfg_files(gxde-kcm_animations_x11 animationskdeglobalssettings.kcfgc GENERATE_MOC)

target_link_libraries(gxde-kcm_animations_x11 PRIVATE
    Qt::DBus
    Qt::Quick
    KF6::ConfigGui
    KF6::KCMUtils
    KF6::I18n
    KF6::KCMUtilsQuick
    KF6::Service
    kcmkwincommon
)
