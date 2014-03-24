set(plasmaengineexplorer_SRCS
    engineexplorer.cpp
    ktreeviewsearchline.cpp
    main.cpp
    serviceviewer.cpp
    modelviewer.cpp
)

qt5_wrap_ui(plasmaengineexplorer_SRCS engineexplorer.ui serviceviewer.ui)

add_executable(plasmaengineexplorer ${plasmaengineexplorer_SRCS})

target_link_libraries(plasmaengineexplorer
    KF5::IconThemes
    KF5::I18n
    KF5::Service
    KF5::Completion
    KF5::Plasma
    KF5::XmlGui
    KF5::I18n
    KF5::ConfigCore
    KF5::ConfigWidgets
)

install(TARGETS plasmaengineexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})

# FIXME: needs replacement
#kde4_create_manpage(man-plasmaengineexplorer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})
