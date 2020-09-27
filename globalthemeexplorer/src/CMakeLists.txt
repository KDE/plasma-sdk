set(globalthemeexplorer_SRCS
    main.cpp
    globaltheme.cpp
    globalthememodel.cpp
    resources.qrc
)

add_executable(globalthemeexplorer ${globalthemeexplorer_SRCS})
target_compile_definitions(globalthemeexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(globalthemeexplorer
 Qt5::Gui
 Qt5::Quick
 Qt5::DBus
 KF5::Archive
 KF5::I18n
 KF5::Package
 KF5::KIOCore
 KF5::KIOWidgets
 KF5::QuickAddons
)

install(TARGETS globalthemeexplorer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


