project(PlasMate)

find_package(KDE4)
include(KDE4Defaults)

set (PlasMate_SRC
    main.cpp
    mainwindow.cpp
    startpage.cpp
    newproject.cpp
    )

kde4_add_ui_files (PlasMate_SRC
    newproject.ui
)

include_directories(${KDE4_INCLUDES})

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBRARY})

install(TARGETS plasmate DESTINATION ${BIN_INSTALL_DIR})
# install(FILES PlasMateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
