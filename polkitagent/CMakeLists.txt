include_directories(
    ${POLKITQT-1_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

set(SOURCES
    main.cpp
    policykitagent.cpp
)

set(QML_SOURCES
    qml/authdialog.qml
)

qt5_add_resources(RESOURCES polkitagent.qrc)

hawaii_translate_ts(QM_FILES
                    SOURCES ${SOURCES} ${QML_SOURCES}
                    TEMPLATE hawaii-polkit-agent
                    INSTALL_DIR ${DATA_INSTALL_DIR}/hawaii-workspace/translations
                    COMPONENT "Runtime")
hawaii_translate_desktop(DESKTOP_FILE SOURCES org.hawaiios.PolkitAgent.desktop.in)
hawaii_translation_loader(SOURCES
                          TRANSLATIONS_DIR hawaii-workspace/translations
                          CATALOG_NAME hawaii-polkit-agent)

add_executable(hawaii-polkit-agent ${SOURCES} ${RESOURCES} ${QM_FILES} ${DESKTOP_FILE})
target_link_libraries(hawaii-polkit-agent
                      Qt5::Core
                      Qt5::Gui
                      Qt5::DBus
                      Qt5::Quick
                      Qt5::QuickControls2
                      ${POLKITQT-1_CORE_LIBRARY}
                      ${POLKITQT-1_AGENT_LIBRARY})
if(systemd_FOUND)
    target_link_libraries(hawaii-polkit-agent ${systemd_LIBRARIES})
endif()

install(TARGETS hawaii-polkit-agent
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${DESKTOP_FILE}
        DESTINATION ${AUTOSTART_INSTALL_DIR})
