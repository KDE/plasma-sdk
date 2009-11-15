project(PlasMate)

find_package(KDE4)
include(KDE4Defaults)

set (PlasMate_SRC
    editors/editpage.cpp
    editors/metadata/metadataeditor.cpp
    main.cpp
    sidebaritem.cpp
    sidebardelegate.cpp
    sidebarlistwidget.cpp
    sidebarprivatestorage.cpp
    sidebar.cpp
    savesystem/dvcsjob.cpp
    savesystem/gitrunner.cpp
    savesystem/branchdialog.cpp
    savesystem/commitdialog.cpp
    savesystem/timelineitem.cpp
    savesystem/timelinedelegate.cpp
    savesystem/timelinelistwidget.cpp
    savesystem/timelineprivatestorage.cpp
    savesystem/timeline.cpp
    mainwindow.cpp
    startpage.cpp
    packagemodel.cpp
    previewer/previewer.cpp
    publisher/publisher.cpp
    )

kde4_add_ui_files (PlasMate_SRC
   mainwindow.ui
   startpage.ui
   editors/metadata/metadata.ui
)

include_directories(${KDE4_INCLUDES})

add_subdirectory( editors )
add_subdirectory( previewer )
add_subdirectory( templates )
add_subdirectory( publisher )
#add_subdirectory( savesystem )

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KIO_LIBRARY} ${KDE4_PLASMA_LIBS})

install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
