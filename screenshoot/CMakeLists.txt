include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    main.cpp
)

qt5_add_resources(RESOURCES hawaii-screenshot.qrc)

add_executable(hawaii-screenshot ${SOURCES} ${RESOURCES})
target_link_libraries(hawaii-screenshot
                      Qt5::Qml
                      Qt5::Quick)

install(TARGETS hawaii-screenshot
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES org.hawaii.screenshot.desktop
        DESTINATION ${XDG_APPS_INSTALL_DIR})
