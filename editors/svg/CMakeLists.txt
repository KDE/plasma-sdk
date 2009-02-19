project( plasmate-svg-editor )

find_package(KDE4)
include(KDE4Defaults)

set(svg_viewer_SRCS
    svgviewer.cpp)

#kde4_add_ui_files (svg_viewer_SRCS
#   svg.ui
#)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

kde4_add_plugin(plasmate_svg_viewer ${svg_viewer_SRCS})
target_link_libraries(plasmate_svg_viewer ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

set( testsvg_SRCS tests/test_svg.cpp
     		       svgviewer.cpp
      )

#kde4_add_ui_files (testsvg_SRCS
#   svg.ui
#)

kde4_add_executable( testsvg TEST ${testsvg_SRCS})
target_link_libraries( testsvg ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} )
