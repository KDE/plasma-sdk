set(remote_widgets_browser_SRCS
    main.cpp
    plasmafiltermodel.cpp
)

qt4_add_resources(remote_widgets_browser_SRCS resources.qrc)

kde4_add_executable(remote-widgets-browser ${remote_widgets_browser_SRCS})
target_link_libraries(remote-widgets-browser ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} ${KDECLARATIVE_LIBRARIES} ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS remote-widgets-browser ${INSTALL_TARGETS_DEFAULT_ARGS})

