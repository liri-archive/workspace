include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

add_definitions(-Wno-deprecated-declarations)

set(SOURCES
    main.cpp
)

greenisland_add_client_protocol(SOURCES
    PROTOCOL ${CMAKE_CURRENT_SOURCE_DIR}/greenisland-screenshooter.xml
    BASENAME greenisland-screenshooter
    PREFIX greenisland_
)

qt5_add_resources(RESOURCES hawaii-screenshot.qrc)

add_executable(hawaii-screenshot ${SOURCES} ${RESOURCES})
target_link_libraries(hawaii-screenshot
                      Qt5::Qml
                      Qt5::Quick
                      Wayland::Client)

install(TARGETS hawaii-screenshot
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES org.hawaiios.screenshot.desktop
        DESTINATION ${XDG_APPS_INSTALL_DIR})
