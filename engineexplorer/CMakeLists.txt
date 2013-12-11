set(plasmaengineexplorer_SRCS
    engineexplorer.cpp
    ktreeviewsearchline.cpp
    main.cpp
    serviceviewer.cpp
)

kde4_add_ui_files(plasmaengineexplorer_SRCS engineexplorer.ui serviceviewer.ui)

add_executable(plasmaengineexplorer ${plasmaengineexplorer_SRCS})

target_link_libraries(plasmaengineexplorer
    KF5::IconThemes
    KF5::I18n
    KF5::Service
    KF5::KDE4Support
    KF5::Completion
    KF5::Plasma
)

install(TARGETS plasmaengineexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})

# FIXME: needs replacement
#kde4_create_manpage(man-plasmaengineexplorer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
