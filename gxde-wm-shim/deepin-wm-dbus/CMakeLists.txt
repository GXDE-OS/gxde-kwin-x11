set(TARGET_PATH "${CMAKE_INSTALL_PREFIX}/bin")
set(TARGET_NAME "deepin-wm-dbus")

find_package(Qt6Core REQUIRED)
find_package(Qt6DBus REQUIRED)
find_package(X11 REQUIRED)
find_package(KF6Config REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(KF6GlobalAccel REQUIRED)
find_package(PkgConfig REQUIRED)



option(DISABLE_DEEPIN_WM "disable compatibility with deepin-wm" OFF)

if (DISABLE_DEEPIN_WM)
    set(DEEPIN_WM_PREFIX "<!-- By \"CONFIG+=DISABLE_DEEPIN_WM\"")
    set(DEEPIN_WM_SUFFIX "-->")
    add_definitions(-DDISABLE_DEEPIN_WM)
else()
    # 明确使用 Qt6 版本的 gsettings-qt
    pkg_search_module(GSETTINGS REQUIRED gsettings-qt6)
endif()

configure_file(com.deepin.wm.xml.in com.deepin.wm.xml)
configure_file(com.deepin.wm.service.in com.deepin.wm.service)

install_files(
    "/share/dbus-1/interfaces"
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/com.deepin.wm.xml
)
install_files(
    "/share/dbus-1/services"
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/com.deepin.wm.service
)

qt6_add_dbus_adaptor(
    wm_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/com.deepin.wm.xml
    deepinwmfaker.h
    DeepinWMFaker
)

add_executable(
    ${TARGET_NAME}
    deepinwmfaker.cpp
    main.cpp
    ${wm_SRCS}
)

target_include_directories(
    ${TARGET_NAME}
    PRIVATE
    ${GSETTINGS_INCLUDE_DIRS}
)

target_link_libraries(
    ${TARGET_NAME}
    Qt6::Core
    Qt6::DBus
    KF6::ConfigCore
    KF6::WindowSystem
    KF6::GlobalAccel
    ${GSETTINGS_LDFLAGS}
)

install_targets("/bin" deepin-wm-dbus)
