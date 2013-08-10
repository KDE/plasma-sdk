set(plasmaengineexplorer_SRCS
    engineexplorer.cpp
    ktreeviewsearchline.cpp
    main.cpp
    serviceviewer.cpp
)

kde4_add_ui_files(plasmaengineexplorer_SRCS engineexplorer.ui serviceviewer.ui)

add_executable(plasmaengineexplorer ${plasmaengineexplorer_SRCS})

target_link_libraries(plasmaengineexplorer
    ${KDE4_KDECORE_LIBS}
    ${Plasma_LIBRARIES}
    ${KDE4Attic_LIBRARIES}
    ${KDE4Support_LIBRARIES}
)

install(TARGETS plasmaengineexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})

# FIXME: needs replacement
#kde4_create_manpage(man-plasmaengineexplorer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
