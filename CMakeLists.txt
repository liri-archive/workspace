project(HawaiiWorkspace)

cmake_minimum_required(VERSION 2.8.12)

# Silence CMake warnings
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

# Set version
set(PROJECT_VERSION "0.7.0")

# Set up packaging
set(CPACK_PACKAGE_NAME "hawaii-workspace")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SET_DESTDIR FALSE)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# Options
option(ENABLE_SYSTEMD "Enables systemd support" ON)

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Enable QML debugger for debug or release with debug info builds
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb]")
    add_definitions(-DQT_QML_DEBUG)
endif()

# Minimum version requirements
set(ECM_MIN_VERSION "1.4.0")
set(QT_MIN_VERSION "5.6.0")
set(GREENISLAND_MIN_VERSION "0.7.90")
set(HAWAII_MIN_VERSION "0.6.90")
set(POLKITQT_MIN_VERSION "0.103.0")

# ECM
find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} "${CMAKE_SOURCE_DIR}/cmake")

# Macros
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

# Don't cast QString from/to ascii
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
)

# Find pkg-config here so that it can be referenced soon
find_package(PkgConfig)

# Add custom uninstall target
if ("${ECM_VERSION}" VERSION_LESS "1.7.0")
    # ECM 1.7.0 provides this target, so we can't roll our own (see issue #172)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
    add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# Find Qt5
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             Core
             DBus
             Gui
             Widgets
             Qml
             Quick)

# Find Hawaii
find_package(Hawaii ${HAWAII_MIN_VERSION} REQUIRED)

# Find Green Island
find_package(GreenIsland ${GREENISLAND_MIN_VERSION} REQUIRED COMPONENTS Client)

# Find PolicyKit Qt5
find_package(PolkitQt5-1 ${POLKITQT_MIN_VERSION} REQUIRED)

# Find Qt5GStreamer for hawaii-screencap
find_package(Qt5GStreamer)

# Find systemd
if(ENABLE_SYSTEMD)
    pkg_check_modules(systemd libsystemd-daemon)
    if(systemd_FOUND)
        set(HAVE_SYSTEMD 1)

        # systemd user unit directory
        if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
            execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemduserunitdir systemd OUTPUT_VARIABLE SYSTEMD_USER_UNIT_DIR)
            string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_USER_UNIT_DIR ${SYSTEMD_USER_UNIT_DIR})
        endif()
        message(STATUS "systemd user unit directory: ${SYSTEMD_USER_UNIT_DIR}")
    else()
        # libsystem-daemon was merged to libsystem on systemd 208->209
        pkg_check_modules(systemd libsystemd)
        if(systemd_FOUND)
            set(HAVE_SYSTEMD 1)
        endif()
    endif()
    add_feature_info("systemd" systemd_FOUND "Required for systemd integration")
else()
    set(HAVE_SYSTEMD 0)
endif()

# gio (for the settings)
pkg_check_modules(gio gio-2.0 REQUIRED)
if (NOT DEFINED GSETTINGS_SCHEMA_DIR)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=prefix gio-2.0 OUTPUT_VARIABLE _gio_prefix)
    string(REGEX REPLACE "[ \t\n]+" "" _gio_prefix ${_gio_prefix})
    set(GSETTINGS_SCHEMA_DIR "${_gio_prefix}/share/glib-2.0/schemas")
endif()
message(STATUS "GSettings schemas directory: ${GSETTINGS_SCHEMA_DIR}")

# Subdirectories
add_subdirectory(data)
add_subdirectory(headers)
add_subdirectory(platformtheme)
add_subdirectory(polkitagent)
add_subdirectory(powermanager)
if(Qt5GStreamer_FOUND)
    add_subdirectory(screencast)
endif()
add_subdirectory(screenshot)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
