# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"gxde-kcm_kwinrules_x11\")
add_definitions(-DKCMRULES)

set(kwinrules_SRCS
    ../../rulebooksettings.cpp
    ../../rules.cpp
    ../../utils/common.cpp
    ../../virtualdesktopsdbustypes.cpp
    kwinsrc.cpp
    optionsmodel.cpp
    ruleitem.cpp
    rulesmodel.cpp
    rulebookmodel.cpp
)

kconfig_add_kcfg_files(kwinrules_SRCS ../../rulesettings.kcfgc)
kconfig_add_kcfg_files(kwinrules_SRCS ../../rulebooksettingsbase.kcfgc)

add_library(KWinRulesObjects STATIC ${kwinrules_SRCS})
set_property(TARGET KWinRulesObjects PROPERTY POSITION_INDEPENDENT_CODE ON)

if (KWIN_BUILD_X11)
    set(kwin_kcm_rules_XCB_LIBS
        XCB::CURSOR
        XCB::XCB
        XCB::XFIXES
    )
endif()

set(kcm_libs
    Qt::Quick

    KF6::KCMUtils
    KF6::I18n
    KF6::KCMUtilsQuick
    KF6::WindowSystem
    KF6::XmlGui
)

if (KWIN_BUILD_ACTIVITIES)
    set(kcm_libs ${kcm_libs} Plasma::Activities)
endif()
target_link_libraries(KWinRulesObjects ${kcm_libs} ${kwin_kcm_rules_XCB_LIBS})

kcmutils_add_qml_kcm(gxde-kcm_kwinrules_x11
    SOURCES kcmrules.cpp
    DISABLE_DESKTOP_FILE_GENERATION)
gxde_generate_kcm_desktop_file(gxde-kcm_kwinrules_x11 kcm_kwinrules_x11.json)
target_link_libraries(gxde-kcm_kwinrules_x11 PRIVATE KWinRulesObjects)
