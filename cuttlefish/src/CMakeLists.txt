
set(cuttlefish_SRCS
    main.cpp
    iconmodel.cpp
    colorschemes.cpp
    sortfiltermodel.cpp
)

add_executable(cuttlefish ${cuttlefish_SRCS})
target_compile_definitions(cuttlefish PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(cuttlefish
    Qt::Quick
    Qt::Gui
    Qt::Widgets # for QDirModel
    KF5::Plasma
    KF5::KIOWidgets
    KF5::ConfigWidgets
    KF5::I18n
    KF5::IconThemes
    KF5::Package
    Qt::DBus
)

install(TARGETS cuttlefish ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(editorplugin)
