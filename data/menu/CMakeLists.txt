set(DIRECTORIES
    liri-entertainment.directory
    liri-internet.directory
    liri-main.directory
    liri-system.directory
    liri-work.directory
)
install(FILES ${DIRECTORIES}
        DESTINATION ${XDG_DIRECTORY_INSTALL_DIR})

install(FILES liri-applications.menu
        DESTINATION ${SYSCONF_INSTALL_DIR}/xdg/menus)
