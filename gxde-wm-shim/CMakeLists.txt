cmake_minimum_required(VERSION 3.16)

project(DDEKWin)

# When built as part of a KDE project such as gxde-kwin-x11, do not inherit its
# directory-wide definitions (QT_NO_URL_CAST_FROM_STRING, QT_USE_QSTRINGBUILDER,
# deprecation limits, ...): these sources are written for a plain Qt build.
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS "")
endif()

set(PROJECT_NAME "dde-kwin")

if (NOT PROJECT_VERSION)
    set(PROJECT_VERSION "0.0.1")
endif()

if (NOT PROJECT_VERSION_MAJOR)
    set(PROJECT_VERSION_MAJOR 0)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
    -DPROJECT_NAME=\\"${PROJECT_NAME}\\"
    -DPROJECT_VERSION=\\"${PROJECT_VERSION}\\"
)

option(USE_SCRIPTS "use scripts" ON)
option(USE_DEEPIN_WM_DBUS "use deepin-wm dbus service" ON)

# Install layout. The defaults keep the standalone dde-kwin layout. A parent
# project (gxde-kwin-x11) sets these before add_subdirectory() to install into
# its own namespace.
macro(dde_kwin_default var)
    if (NOT DEFINED ${var})
        set(${var} ${ARGN})
    endif()
endmacro()

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "mips64")
    set(CMAKE_CXX_FLAGS "-O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions  -mloongson-ext2 -mloongson-mmi -Wl,--as-needed,-O1 -fno-stack-protector")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-need -fPIE")
    # 1050 security testing,security hardening
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all -z relro -z now -z noexecstack -pie")
    set(CMAKE_EXE_LINKER_FLAGS "-pie")
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()
include(GNUInstallDirs)

dde_kwin_default(DEEPIN_WM_DBUS_NAME "deepin-wm-dbus")
dde_kwin_default(DEEPIN_WM_DBUS_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
dde_kwin_default(DEEPIN_WM_DBUS_INSTALL_SERVICE ON)
dde_kwin_default(DEEPIN_WM_DBUS_INTERFACE_FILE "com.deepin.wm.xml")
dde_kwin_default(DEEPIN_WM_DBUS_INTERFACE_DIR "${CMAKE_INSTALL_DATADIR}/dbus-1/interfaces")
# deepinwmfaker.cpp reads the default shortcuts from this directory.
dde_kwin_default(DEEPIN_WM_DBUS_SHORTCUTS_DIR "${CMAKE_INSTALL_DATADIR}/gxde-wm-dbus")
dde_kwin_default(KWIN_SCRIPTS_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/kwin/scripts")
dde_kwin_default(KWIN_SCRIPTS screenedge/runcommandaction screenedge/closewindowaction ddeshortcuts ddelaunchpad gtkframe)
dde_kwin_default(KWIN_CONFIG_FILE "kwinrc")
dde_kwin_default(KWIN_RULES_CONFIG_FILE "kwinrulesrc")
dde_kwin_default(INSTALL_KDE_GLOBAL_CONFIGS ON)
dde_kwin_default(INSTALL_DEEPIN_KWIN_DCONFIG ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (KWIN_VERSION)
    message("kwin version: ${KWIN_VERSION}")
    string(REPLACE "." ";" KWIN_VERSION_LIST ${KWIN_VERSION})
    list(APPEND KWIN_VERSION_LIST 0 0 0 0)

    if (NOT KWIN_VER_MAJ)
        list(GET KWIN_VERSION_LIST 0 KWIN_VER_MAJ)
    endif()

    if (NOT KWIN_VER_MIN)
        list(GET KWIN_VERSION_LIST 1 KWIN_VER_MIN)
    endif()

    if (NOT KWIN_VER_PAT)
        list(GET KWIN_VERSION_LIST 2 KWIN_VER_PAT)
    endif()

    if (NOT KWIN_VER_BUI)
        list(GET KWIN_VERSION_LIST 3 KWIN_VER_BUI)
    endif()

    add_definitions(
        -DKWIN_VERSION_STR=\\"${KWIN_VERSION}\\"
        -DKWIN_VERSION_MAJ=${KWIN_VER_MAJ}
        -DKWIN_VERSION_MIN=${KWIN_VER_MIN}
        -DKWIN_VERSION_PAT=${KWIN_VER_PAT}
        -DKWIN_VERSION_BUI=${KWIN_VER_BUI}
    )
endif()

add_subdirectory(configures)

if (USE_DEEPIN_WM_DBUS)
  add_subdirectory(deepin-wm-dbus)
endif()

if (USE_SCRIPTS)
  add_subdirectory(scripts)
endif()
