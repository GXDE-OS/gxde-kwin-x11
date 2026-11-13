foreach(script ${KWIN_SCRIPTS})
    install(
        DIRECTORY
        ${script}
        DESTINATION
        ${KWIN_SCRIPTS_INSTALL_DIR}
    )
endforeach()
