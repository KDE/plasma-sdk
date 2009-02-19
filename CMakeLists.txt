project(PlasMate)

find_package(KDE4)
include(KDE4Defaults)

set (PlasMate_SRC
    main.cpp
    mainwindow.cpp
    newproject.cpp
    startpage.cpp
    packagemodel.cpp
    )

kde4_add_ui_files (PlasMate_SRC
   mainwindow.ui
   startpage.ui
   newproject.ui
)

add_subdirectory( editors )

include_directories(${KDE4_INCLUDES})

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBRARY} ${KDE4_PLASMA_LIBS})

install(TARGETS plasmate DESTINATION ${BIN_INSTALL_DIR})
# install(FILES PlasMateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
