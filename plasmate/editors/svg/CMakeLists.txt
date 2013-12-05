project( plasmate-svg-editor )

#find_package(KDE4 REQUIRED)
#include(KDE4Defaults)

set(svg_viewer_SRCS
    svgviewer.cpp)

#kde4_add_ui_files (svg_viewer_SRCS
#   svg.ui
#)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${KDE4_INCLUDES} ../..)

add_library(plasmate_svg_viewer MODULE ${svg_viewer_SRCS})
target_link_libraries(plasmate_svg_viewer KF5::Plasma KF5::KIOCore KF5::KIOWidgets)

set( testsvg_SRCS tests/test_svg.cpp
     		       svgviewer.cpp
      )

#kde4_add_ui_files (testsvg_SRCS
#   svg.ui
#)

add_executable( testsvg ${testsvg_SRCS})
target_link_libraries( testsvg ${KDE4_KDEUI_LIBS} KF5::Plasma )
