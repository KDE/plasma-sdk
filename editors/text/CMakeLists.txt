project( plasmate_text_editor )

find_package(KDE4)
include(KDE4Defaults)

set(text_editor_SRCS
    texteditor.cpp)

#kde4_add_ui_files (text_editor_SRCS
#)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

kde4_add_plugin(plasmate_text_editor ${text_editor_SRCS})
target_link_libraries(plasmate_text_editor ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

set( testtext_SRCS tests/test_text.cpp
                       texteditor.cpp
      )

# kde4_add_ui_files (textdata_SRCS
#    metadata.ui
# )

kde4_add_executable( testtext TEST ${testtext_SRCS})
target_link_libraries( testtext ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} )

