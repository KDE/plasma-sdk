add_definitions(-DTRANSLATION_DOMAIN=\"cuttlefish_editorplugin\")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(cuttlefishplugin_PART_SRCS
    cuttlefishplugin.cpp cuttlefishplugin.h
)

add_library (cuttlefishplugin MODULE ${cuttlefishplugin_PART_SRCS})

target_link_libraries(cuttlefishplugin
    KF5::TextEditor
    KF5::IconThemes
    KF5::I18n
    KF5::Service
)

install(TARGETS cuttlefishplugin  DESTINATION ${KDE_INSTALL_PLUGINDIR}/ktexteditor)

kcoreaddons_desktop_to_json (cuttlefishplugin cuttlefishplugin.desktop)
