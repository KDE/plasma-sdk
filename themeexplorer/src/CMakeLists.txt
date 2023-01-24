

set(plasmathemeexplorer_SRCS
    main.cpp
    thememodel.cpp
    themelistmodel.cpp
    coloreditor.cpp
)

add_executable(plasmathemeexplorer ${plasmathemeexplorer_SRCS})
target_compile_definitions(plasmathemeexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

#find_package(ActiveApp REQUIRED)

target_link_libraries(plasmathemeexplorer
 Qt::Gui
 Qt::Quick
 Qt::Widgets
 Qt::Xml
 KF5::Archive
 KF5::Declarative
 KF5::I18n
 KF5::Package
 KF5::Plasma
 KF5::KIOCore
 KF5::KIOWidgets
 KF5::QuickAddons
)

if (TARGET Qt6::Core5Compat)
    target_link_libraries(plasmathemeexplorer Qt6::Core5Compat)
endif()

install(TARGETS plasmathemeexplorer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
