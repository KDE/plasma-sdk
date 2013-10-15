set(plasmoidviewer_SRCS
    #    fullview.cpp
    main.cpp
    view.cpp
)

add_executable(plasmoidviewer ${plasmoidviewer_SRCS})

target_link_libraries(plasmoidviewer
    KF5::plasma
    ${PlasmaQuick_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    KF5::KI18n
    ${KDE4Attic_LIBRARIES}
)

install(TARGETS plasmoidviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

#FIXME
#kde4_create_manpage(man-plasmoidviewer.1.docbook 1 INSTALL_DESTINATION ${MAN_INSTALL_DIR})

#qml
plasma_install_package(qmlpackages/shell org.kde.plasmoidviewershell plasma/shells shell)
plasma_install_package(qmlpackages/containment org.kde.plasmoidviewercontainment plasma/plasmoids containment)
