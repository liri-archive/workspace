project(HawaiiWorkspace)

cmake_minimum_required(VERSION 2.8.12)

# Silence CMake warnings
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

# Set version
set(PROJECT_VERSION "0.5.0")

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

# Enable QML debugger for debug or release with debug info builds
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb]")
    add_definitions(-DQT_QML_DEBUG)
endif()

# ECM
find_package(ECM 1.4.0 REQUIRED NO_MODULE)
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

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Find Green Island
find_package(GreenIsland 0.7.90 REQUIRED COMPONENTS Client Server)

# Minimum version requirements
set(QT_MIN_VERSION "5.4.0")

# Find Qt5
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             Core
             DBus
             Gui
             Qml
             Quick)

# Find PolicyKit Qt5
find_package(PolkitQt5-1 REQUIRED 0.103.0)

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

# Subdirectories
add_subdirectory(headers)
add_subdirectory(polkitagent)
add_subdirectory(powermanager)
if(Qt5GStreamer_FOUND)
    add_subdirectory(screencap)
endif()
add_subdirectory(screenshooter)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
