add_definitions(-DTRANSLATION_DOMAIN=\"iconexplorer_editorplugin\")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

kcoreaddons_add_plugin(
    iconexplorerplugin
    INSTALL_NAMESPACE "kf6/ktexteditor"
    SOURCES iconexplorerplugin.cpp
)

target_link_libraries(iconexplorerplugin
    KF6::TextEditor
    KF6::IconThemes
    KF6::I18n
)
