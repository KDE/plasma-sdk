project( plasmate_text_editor_test )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

set( testtext_SRCS tests/test_text.cpp
                        texteditor.cpp
)

kde4_add_executable( testtext TEST ${testtext_SRCS})
target_link_libraries( testtext ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_PLASMA_LIBS} )

