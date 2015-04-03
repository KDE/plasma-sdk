

set(plasmathemeexplorer_SRCS
    main.cpp
    thememodel.cpp
    themelistmodel.cpp
)

add_executable(plasmathemeexplorer ${plasmathemeexplorer_SRCS})
target_compile_definitions(plasmathemeexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

#find_package(ActiveApp REQUIRED)

target_link_libraries(plasmathemeexplorer
 Qt5::Gui
 Qt5::Quick
 Qt5::Widgets
 Qt5::Xml
 KF5::Archive
 KF5::Declarative
 KF5::I18n
 KF5::Package
 KF5::Plasma
 KF5::KIOCore
 KF5::KIOWidgets
 KF5::QuickAddons
)

install(TARGETS plasmathemeexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})
