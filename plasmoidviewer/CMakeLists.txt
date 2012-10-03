set(plasmoidviewer_SRCS
    fullview.cpp
    main.cpp
)

kde4_add_executable(plasmoidviewer ${plasmoidviewer_SRCS})

target_link_libraries(plasmoidviewer
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${KDECLARATIVE_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARY}
)

install(TARGETS plasmoidviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

