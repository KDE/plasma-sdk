project(PlasMate)

find_package(KDE4)
include(KDE4Defaults)

set (PlasMate_SRC
    main.cpp
    mainwindow.cpp
    savesystem.cpp
    sidebar.cpp
    startpage.cpp
    packagemodel.cpp
    previewer/previewer.cpp
    )

kde4_add_ui_files (PlasMate_SRC
   mainwindow.ui
   startpage.ui
)

include_directories(${KDE4_INCLUDES})

add_subdirectory( editors )
add_subdirectory( previewer )

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KIO_LIBRARY} ${KDE4_PLASMA_LIBS})

install(TARGETS plasmate DESTINATION ${BIN_INSTALL_DIR})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
