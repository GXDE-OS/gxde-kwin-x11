add_subdirectory(icons)

########### next target ###############
add_executable(gxde-kwin_x11_update_default_rules update_default_rules.cpp)
target_link_libraries(gxde-kwin_x11_update_default_rules
    KF6::ConfigCore
    Qt::Core
    Qt::DBus
)
install(TARGETS gxde-kwin_x11_update_default_rules DESTINATION ${KDE_INSTALL_LIBDIR}/kconf_update_bin/)

########### install files ###############

install(FILES org_kde_kwin.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR} RENAME org_gxde_kwin_x11.categories)
# Default settings, following deepin-kwin.
install(FILES gxde-kwin_x11rc DESTINATION ${KDE_INSTALL_CONFDIR} RENAME ${KWIN_CONFIG})
