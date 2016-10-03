project(Workspace)

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
cmake_policy(VERSION 3.4.1)

# Instruct CMake to run moc and rrc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Apple-specific configuration
set(APPLE_SUPPRESS_X11_WARNING ON)

# Set version
set(PROJECT_VERSION "0.8.90")

if(NOT USE_SUPERBUILD)
    # Set up packaging
    set(CPACK_PACKAGE_NAME "liri-workspace")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SET_DESTDIR FALSE)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
    include(CPack)
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
endif()

# ECM
find_package(ECM 1.7.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH
    "${ECM_MODULE_PATH}"
    "${ECM_KDE_MODULE_DIR}"
)

# Macros
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

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

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -Wformat -Wswitch-default -Wunused-parameter -pedantic -std=c++11")

# Don't cast QString from/to ascii
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
)

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Find pkg-config here so that it can be referenced soon
find_package(PkgConfig)

# Find Qt5
set(REQUIRED_QT_VERSION "5.7.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
             DBus Widgets Qml Quick QuickControls2)

# Find Vibe
set(REQUIRED_VIBE_VERSION "0.8.90")
find_package(Vibe ${REQUIRED_VIBE_VERSION} REQUIRED)

# Find Green Island
set(REQUIRED_GREENISLAND_VERSION "0.8.0")
find_package(GreenIsland ${REQUIRED_GREENISLAND_VERSION} REQUIRED COMPONENTS Client)

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
add_subdirectory(powermanager)
if(Qt5GStreamer_FOUND)
    add_subdirectory(screencast)
endif()
add_subdirectory(screenshot)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
