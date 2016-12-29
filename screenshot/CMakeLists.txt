include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    imageprovider.cpp
    screenshooter.cpp
)

set(QML_SOURCES
    qml/main.qml
)

qt5_add_resources(RESOURCES screenshot.qrc)

vibe_translate_ts(QM_FILES
                  SOURCES ${SOURCES} ${QML_SOURCES}
                  TEMPLATE liri-screenshot
                  INSTALL_DIR ${DATA_INSTALL_DIR}/liri-workspace/translations
                  COMPONENT "Runtime")
vibe_translate_desktop(DESKTOP_FILE SOURCES io.liri.Screenshot.desktop.in)
vibe_translation_loader(SOURCES
                        TRANSLATIONS_DIR liri-workspace/translations
                        CATALOG_NAME liri-screenshot)

add_executable(liri-screenshot ${SOURCES} ${RESOURCES} ${QM_FILES} ${DESKTOP_FILE})
target_link_libraries(liri-screenshot
                      Qt5::QuickControls2
                      Liri::WaylandClient)

install(TARGETS liri-screenshot
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${DESKTOP_FILE}
        DESTINATION ${XDG_APPS_INSTALL_DIR})
