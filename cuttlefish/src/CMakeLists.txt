
set(cuttlefish_SRCS
    main.cpp
    iconmodel.cpp
    colorschemes.cpp
    sortfiltermodel.cpp
    qml.qrc
)

add_executable(cuttlefish ${cuttlefish_SRCS})
target_compile_definitions(cuttlefish PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(cuttlefish
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

install(TARGETS cuttlefish ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(editorplugin)
