project(PlasMate)

find_package(KDE4 REQUIRED)
find_program(GIT_EXECUTABLE git)
if(NOT GIT_EXECUTABLE)
  message(WARNING "**Git executable not found.\n**You need to install git-core it in order to use the TimeLine!")
endif(NOT GIT_EXECUTABLE)

include(KDE4Defaults)

set (PlasMate_SRC
    editors/editpage.cpp
    editors/metadata/metadataeditor.cpp
    main.cpp
    sidebaritem.cpp
    sidebardelegate.cpp
    sidebartablewidget.cpp
    sidebar.cpp
    savesystem/dvcsjob.cpp
    savesystem/gitrunner.cpp
    savesystem/branchdialog.cpp
    savesystem/commitdialog.cpp
    savesystem/timelineitem.cpp
    savesystem/tabledelegate.cpp
    savesystem/tablewidget.cpp
    savesystem/timeline.cpp
    mainwindow.cpp
    startpage.cpp
    packagemodel.cpp
    previewer/previewer.cpp
    publisher/publisher.cpp
    docbrowser/docbrowser.cpp
    projectmanager/projectmanager.cpp
    )

kde4_add_ui_files (PlasMate_SRC
   mainwindow.ui
   startpage.ui
   editors/metadata/metadata.ui
)

include_directories(${KDE4_INCLUDES})

add_subdirectory( editors )
add_subdirectory( previewer )
add_subdirectory( savesystem )
add_subdirectory( templates )
#add_subdirectory( savesystem )

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KIO_LIBRARY} ${KDE4_PLASMA_LIBS} ${QT_QTWEBKIT_LIBRARY})

install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES plasmate.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

