include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    liritheme.cpp
    liritheme_p.cpp
    hintssettings.cpp
    resourcehelper.cpp
)

ecm_qt_declare_logging_category(SOURCES
                                HEADER logging_p.h
                                IDENTIFIER gLcPlatformTheme
                                CATEGORY_NAME liri.platformtheme)

add_library(liriplatformtheme SHARED ${SOURCES})
set_target_properties(liriplatformtheme PROPERTIES PREFIX "")
target_link_libraries(liriplatformtheme
                      Qt5::Widgets
                      Qt5::QuickControls2
                      Vibe::Settings)

install(TARGETS liriplatformtheme LIBRARY
    DESTINATION ${QT_PLUGIN_INSTALL_DIR}/platformthemes)
