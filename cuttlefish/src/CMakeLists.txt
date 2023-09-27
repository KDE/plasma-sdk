set(cuttlefish_SRCS
    main.cpp
    iconmodel.cpp
    colorschemes.cpp
    sortfiltermodel.cpp
)

add_executable(cuttlefish ${cuttlefish_SRCS})
target_compile_definitions(cuttlefish PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(cuttlefish PRIVATE
    Qt::Quick
    Qt::Gui
    Qt::Widgets # for QDirModel
    KF6::Plasma
    KF6::KIOWidgets
    KF6::ConfigWidgets
    KF6::I18n
    KF6::IconThemes
    Qt::DBus
)

ecm_add_qml_module(cuttlefish URI org.kde.cuttlefish)

ecm_target_qml_sources(cuttlefish SOURCES
    qml/Actions.qml
    qml/Comparison.qml
    qml/Cuttlefish.qml
    qml/GlobalMenuBar.qml
    qml/IconGridDelegate.qml
    qml/IconGrid.qml
    qml/IconMontage.qml
    qml/Menu.qml
    qml/Preview.qml
    qml/ResponsivePreview.qml
    qml/Screenshot.qml
    qml/Tools.qml
    qml/ToolsResponsive.qml
)

install(TARGETS cuttlefish ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(editorplugin)
