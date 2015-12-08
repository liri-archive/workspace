include_directories(
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_BINARY_DIR}/headers
)

if(systemd_FOUND)
    include_directories(${systemd_INCLUDE_DIRS})
endif()

set(SOURCES
    main.cpp
)

add_executable(hawaii-powermanager ${SOURCES})
target_link_libraries(hawaii-powermanager
                      Qt5::Core
                      Qt5::DBus)
if(systemd_FOUND)
    target_link_libraries(hawaii-powermanager ${systemd_LIBRARIES})
endif()

install(TARGETS hawaii-powermanager
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES hawaii-powermanager.desktop
        DESTINATION ${AUTOSTART_INSTALL_DIR})
