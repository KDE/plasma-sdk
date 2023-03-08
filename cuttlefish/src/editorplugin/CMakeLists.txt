add_definitions(-DTRANSLATION_DOMAIN=\"cuttlefish_editorplugin\")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set(cuttlefishplugin_PART_SRCS
    cuttlefishplugin.cpp
)

add_library (cuttlefishplugin MODULE ${cuttlefishplugin_PART_SRCS})

target_link_libraries(cuttlefishplugin
    KF6::TextEditor
    KF6::IconThemes
    KF6::I18n
    KF6::Service
)

install(TARGETS cuttlefishplugin  DESTINATION ${KDE_INSTALL_PLUGINDIR}/ktexteditor)

