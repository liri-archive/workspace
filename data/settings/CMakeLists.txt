configure_file(
    io.liri.desktop.background.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.background.gschema.xml
)
configure_file(
    io.liri.desktop.lockscreen.gschema.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.lockscreen.gschema.xml
)

set(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.background.gschema.xml
    ${CMAKE_CURRENT_BINARY_DIR}/io.liri.desktop.lockscreen.gschema.xml
    io.liri.desktop.interface.gschema.xml
    io.liri.desktop.peripherals.gschema.xml
    io.liri.desktop.panel.gschema.xml
    io.liri.desktop.keybindings.gschema.xml
    io.liri.desktop.session.gschema.xml
    io.liri.system.gschema.xml
)

install(FILES ${FILES} DESTINATION ${GSETTINGS_SCHEMA_DIR})
