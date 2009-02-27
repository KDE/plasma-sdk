project( plasmate-kconfigxt-editor )

find_package(KDE4)
include(KDE4Defaults)

set(kconfigxt_editor_SRCS
    kconfigxteditor.cpp
    kconfigxtparser.cpp)

kde4_add_ui_files (kconfigxt_editor_SRCS
   kconfigxteditor.ui
)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

kde4_add_plugin(plasmate_kconfigxt_editor ${kconfigxt_editor_SRCS})
target_link_libraries(plasmate_kconfigxt_editor ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

set( testkconfigxt_SRCS tests/test_kconfigxteditor.cpp
     kconfigxteditor.cpp
     kconfigxtparser.cpp
      )

kde4_add_ui_files (testkconfigxt_SRCS
   kconfigxteditor.ui
)

kde4_add_executable( testkconfigxt TEST ${testkconfigxt_SRCS})
target_link_libraries( testkconfigxt ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} )
