set(plasma-remote_widgets_browser_SRCS
    main.cpp
    plasmafiltermodel.cpp
)

qt4_add_resources(plasma-remote_widgets_browser_SRCS resources.qrc)

kde4_add_executable(plasma-remote-widgets-browser ${plasma-remote_widgets_browser_SRCS})
target_link_libraries(plasma-remote-widgets-browser KF5::Plasma KF5::KIOCore KF5::KIOWidgets ${KDECLARATIVE_LIBRARIES} ${QT_QTDECLARATIVE_LIBRARY})

install(TARGETS plasma-remote-widgets-browser ${INSTALL_TARGETS_DEFAULT_ARGS})

