

set(lookandfeelexplorer_SRCS
    main.cpp
    lnflogic.cpp
    lnflistmodel.cpp
)

add_executable(lookandfeelexplorer ${lookandfeelexplorer_SRCS})
target_compile_definitions(lookandfeelexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

#find_package(ActiveApp REQUIRED)

target_link_libraries(lookandfeelexplorer
 Qt5::Gui
 Qt5::Quick
 Qt5::Widgets
 Qt5::DBus
 KF5::Archive
 KF5::Declarative
 KF5::I18n
 KF5::Package
 KF5::Plasma
 KF5::KIOCore
 KF5::KIOWidgets
 KF5::QuickAddons
)

install(TARGETS lookandfeelexplorer ${INSTALL_TARGETS_DEFAULT_ARGS})
