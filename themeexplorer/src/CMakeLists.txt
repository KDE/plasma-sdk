

set(plasmathemeexplorer_SRCS
    main.cpp
    thememodel.cpp
    themelistmodel.cpp
)

add_executable(plasmathemeexplorer ${plasmathemeexplorer_SRCS})

#find_package(ActiveApp REQUIRED)

target_link_libraries(plasmathemeexplorer
 Qt5::Gui
 Qt5::Quick
 Qt5::Widgets
 KF5::Declarative
 KF5::I18n
 KF5::Package
 KF5::Plasma
 KF5::KIOCore
 KF5::KIOWidgets
 KF5::QuickAddons
)

install(TARGETS plasmathemeexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})
