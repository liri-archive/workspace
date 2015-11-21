project(HawaiiWorkspace)

cmake_minimum_required(VERSION 2.8.12)

# Silence CMake warnings
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

# Set version
set(PROJECT_VERSION "0.4.90")

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

# Require at least gcc 4.8
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8")
        message(SEND_ERROR "Version ${CMAKE_CXX_COMPILER_VERSION} of the ${CMAKE_CXX_COMPILER_ID} C++ compiler is not supported. Please use version 4.8 or later.")
    endif()
endif()

# Adjusting CMAKE_C_FLAGS to get Wayland protocols to compile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")

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

# Wayland
set(REQUIRED_WAYLAND_VERSION 1.4.0)
find_package(Wayland ${REQUIRED_WAYLAND_VERSION} COMPONENTS Client)
set_package_properties(Wayland PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required for Wayland protocols")
add_feature_info("Wayland-Client" Wayland_Client_FOUND "Required for Wayland protocols")

# Wayland and QtWayland scanner
find_package(QtWaylandScanner REQUIRED)

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
add_subdirectory(screenshoot)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
