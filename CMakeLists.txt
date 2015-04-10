project(Hawaii)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Options
option(ENABLE_SYSTEMD "Enables systemd support" ON)

# Set version
set(PROJECT_VERSION "0.0.0")

# ECM
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Installation directories
include(KDEInstallDirs)

# Other macros
include(KDECMakeSettings)
include(KDECompilerSettings)

# Macro to print feature summary
include(FeatureSummary)

# Find pkg-config here so that it can be referenced soon
find_package(PkgConfig)

# systemd user unit directory
if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemduserunitdir systemd OUTPUT_VARIABLE SYSTEMD_USER_UNIT_DIR)
    string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_USER_UNIT_DIR ${SYSTEMD_USER_UNIT_DIR})
endif()
message(STATUS "systemd user unit directory: ${SYSTEMD_USER_UNIT_DIR}")

# Add custom uninstall target
if ("${ECM_VERSION}" VERSION_LESS "1.7.0")
    # ECM 1.7.0 provides this target, so we can't roll our own (see issue #172)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
    add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# Set up packaging
set(CPACK_PACKAGE_NAME "hawaii-baseapps")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SET_DESTDIR FALSE)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Minimum version requirements
set(QT_MIN_VERSION "5.4.0")

# Find Qt5
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             Core
             DBus
             Gui
             Qml
             Quick)

# FInd PolicyKit Qt5
find_package(PolkitQt5-1 REQUIRED 0.103.0)

# Find systemd
if(ENABLE_SYSTEMD)
    pkg_check_modules(systemd libsystemd-daemon)
    if(systemd_FOUND)
        set(HAVE_SYSTEMD 1)
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

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
