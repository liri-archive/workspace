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

qt5_add_resources(RESOURCES hawaii-screenshot.qrc)

hawaii_translate_ts(QM_FILES
                    SOURCES ${SOURCES} ${QML_SOURCES}
                    TEMPLATE hawaii-screenshot
                    INSTALL_DIR ${DATA_INSTALL_DIR}/hawaii-workspace/translations
                    COMPONENT "Runtime")
hawaii_translate_desktop(DESKTOP_FILE SOURCES org.hawaiios.Screenshot.desktop.in)
hawaii_translation_loader(SOURCES
                          TRANSLATIONS_DIR hawaii-workspace/translations
                          CATALOG_NAME hawaii-screenshot)

add_executable(hawaii-screenshot ${SOURCES} ${RESOURCES} ${QM_FILES} ${DESKTOP_FILE})
target_link_libraries(hawaii-screenshot
                      Qt5::QuickControls2
                      GreenIsland::Client)

install(TARGETS hawaii-screenshot
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${DESKTOP_FILE}
        DESTINATION ${XDG_APPS_INSTALL_DIR})
