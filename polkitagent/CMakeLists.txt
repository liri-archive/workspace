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

add_executable(hawaii-polkit-agent ${SOURCES})
target_link_libraries(hawaii-polkit-agent
                      Qt5::Core
                      Qt5::Gui
                      Qt5::DBus
                      Qt5::Quick
                      ${POLKITQT-1_CORE_LIBRARY}
                      ${POLKITQT-1_AGENT_LIBRARY})
if(systemd_FOUND)
    target_link_libraries(hawaii-polkit-agent ${systemd_LIBRARIES})
endif()

install(TARGETS hawaii-polkit-agent
        DESTINATION ${CMAKE_INSTALL_BINDIR})
