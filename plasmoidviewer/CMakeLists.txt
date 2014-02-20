set(plasmoidviewer_SRCS
    main.cpp
    view.cpp
)

add_executable(plasmoidviewer ${plasmoidviewer_SRCS})

target_link_libraries(plasmoidviewer
    Qt5::Quick
    KF5::Plasma
    KF5::PlasmaQuick
    KF5::I18n
)

install(TARGETS plasmoidviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

#FIXME
#kde4_create_manpage(man-plasmoidviewer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})

#qml
plasma_install_package(qmlpackages/shell org.kde.plasma.plasmoidviewershell plasma/shells shell)
