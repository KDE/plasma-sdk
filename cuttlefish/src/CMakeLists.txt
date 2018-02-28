
set(cuttlefish_SRCS
    main.cpp
    view.cpp
    iconmodel.cpp
)

add_executable(cuttlefish ${cuttlefish_SRCS})
target_compile_definitions(cuttlefish PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(cuttlefish
    Qt5::Quick
    Qt5::Gui
    Qt5::Widgets # for QDirModel
    KF5::Plasma
    KF5::KIOWidgets
    KF5::Declarative
    KF5::I18n
    KF5::IconThemes
    KF5::Package
)

install(TARGETS cuttlefish ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(editorplugin)
