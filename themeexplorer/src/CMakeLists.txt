add_executable(plasmathemeexplorer
    main.cpp
    thememodel.cpp
    themelistmodel.cpp
    coloreditor.cpp
)

ecm_add_qml_module(plasmathemeexplorer
    URI org.kde.plasma.themeexplorer
    GENERATE_PLUGIN_SOURCE
    QML_FILES
        qml/Main.qml
        qml/ColorButton.qml
        qml/ColorEditor.qml
        qml/FormLabel.qml
        qml/MetadataEditor.qml
        qml/delegates/actionbutton.qml
        qml/delegates/allframesvgs.qml
        qml/delegates/analog_meter.qml
        qml/delegates/busyindicator.qml
        qml/delegates/button.qml
        qml/delegates/checkmarks.qml
        qml/delegates/clock.qml
        qml/delegates/containment-controls.qml
        qml/delegates/dialog.qml
        qml/delegates/framesvg.qml
        qml/delegates/Hand.qml
        qml/delegates/icons.qml
        qml/delegates/listitem.qml
        qml/delegates/monitor.qml
        qml/delegates/panel.qml
        qml/delegates/progressbar.qml
        qml/delegates/scrollbar.qml
        qml/delegates/slider.qml
        qml/delegates/tabbar.qml
        qml/delegates/textfield.qml
        qml/fakecontrols/Button.qml
        qml/fakecontrols/CheckBox.qml
        qml/fakecontrols/LineEdit.qml
    RESOURCES
        themeDescription.json
)

target_compile_definitions(plasmathemeexplorer PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries(plasmathemeexplorer PRIVATE
    Qt::Gui
    Qt::Quick
    Qt::Widgets
    Qt::Xml
    KF6::Archive
    KF6::I18n
    KF6::I18nQml
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::Package
    KF6::Svg
    Plasma::PlasmaQuick
)

install(TARGETS plasmathemeexplorer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.plasma.themeexplorer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
