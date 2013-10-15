set(plasmaengineexplorer_SRCS
    engineexplorer.cpp
    ktreeviewsearchline.cpp
    main.cpp
    serviceviewer.cpp
)

kde4_add_ui_files(plasmaengineexplorer_SRCS engineexplorer.ui serviceviewer.ui)

add_executable(plasmaengineexplorer ${plasmaengineexplorer_SRCS})

target_link_libraries(plasmaengineexplorer
    KF5::KIconThemes
    KF5::KI18n
    KF5::KService
    KF5::Plasma
    KF5::KDE4Attic
)

install(TARGETS plasmaengineexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})

# FIXME: needs replacement
#kde4_create_manpage(man-plasmaengineexplorer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
