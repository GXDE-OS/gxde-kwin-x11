# SPDX-FileCopyrightText: 2026 CharOfString <root@charofstring.cc>
#
# SPDX-License-Identifier: BSD-3-Clause

set(GXDE_SESSION_NAME "gxde-experimental-x11")
set(GXDE_SESSION_LAUNCHER "start${GXDE_SESSION_NAME}")
set(GXDE_SESSION_WRAPPER_DIR "${CMAKE_INSTALL_PREFIX}/lib/${GXDE_KWIN_NAME}/experimental-session")
set(GXDE_SESSION_LAUNCHER_PATH "${KDE_INSTALL_FULL_BINDIR}/${GXDE_SESSION_LAUNCHER}")
set(GXDE_KWIN_BINARY "${KDE_INSTALL_FULL_BINDIR}/${GXDE_KWIN_NAME}")

configure_file(startgxde-experimental-x11.in "${CMAKE_CURRENT_BINARY_DIR}/${GXDE_SESSION_LAUNCHER}" @ONLY)
configure_file(kwin_no_scale.in "${CMAKE_CURRENT_BINARY_DIR}/kwin_no_scale" @ONLY)
configure_file(gxde-experimental-x11.desktop.in "${CMAKE_CURRENT_BINARY_DIR}/${GXDE_SESSION_NAME}.desktop" @ONLY)

install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${GXDE_SESSION_LAUNCHER}" DESTINATION "${KDE_INSTALL_BINDIR}")
install(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/kwin_no_scale" DESTINATION "${GXDE_SESSION_WRAPPER_DIR}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${GXDE_SESSION_NAME}.desktop" DESTINATION "${KDE_INSTALL_DATADIR}/xsessions")
