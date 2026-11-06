foreach(size 16 32 48)
    configure_file(
        ${size}-apps-kwin-x11.png
        ${CMAKE_CURRENT_BINARY_DIR}/${size}-apps-${GXDE_KWIN_NAME}.png
        COPYONLY)
endforeach()
configure_file(
    sc-apps-kwin-x11.svgz
    ${CMAKE_CURRENT_BINARY_DIR}/sc-apps-${GXDE_KWIN_NAME}.svgz
    COPYONLY)

ecm_install_icons(
    ICONS
        ${CMAKE_CURRENT_BINARY_DIR}/16-apps-${GXDE_KWIN_NAME}.png
        ${CMAKE_CURRENT_BINARY_DIR}/32-apps-${GXDE_KWIN_NAME}.png
        ${CMAKE_CURRENT_BINARY_DIR}/48-apps-${GXDE_KWIN_NAME}.png
        ${CMAKE_CURRENT_BINARY_DIR}/sc-apps-${GXDE_KWIN_NAME}.svgz
    DESTINATION
        ${KDE_INSTALL_ICONDIR}
    THEME
        hicolor
)
