include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

set(SOURCES
    main.cpp
    screenshooter.cpp
)

qt5_add_resources(RESOURCES hawaii-screenshot.qrc)

add_executable(hawaii-screenshot ${SOURCES} ${RESOURCES})
target_link_libraries(hawaii-screenshot
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Qml
                      Qt5::Quick
                      GreenIsland::Client)

install(TARGETS hawaii-screenshot
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES org.hawaiios.screenshot.desktop
        DESTINATION ${XDG_APPS_INSTALL_DIR})
