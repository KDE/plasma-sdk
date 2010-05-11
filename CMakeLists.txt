project(PlasMate)

# Find needed packages
find_package(KDE4 REQUIRED)
find_package(LibAttica REQUIRED)

# Find optional executables
find_program(GIT_EXECUTABLE git)

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
    previewer/plasmoid/plasmoidpreviewer.cpp
    previewer/plasmoid/plasmoidview.cpp
    previewer/runner/runnerpreviewer.cpp
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
add_subdirectory( previewer/plasmoid )
add_subdirectory( savesystem )
add_subdirectory( templates )
#add_subdirectory( savesystem )

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${KDE4_KPARTS_LIBS} ${KDE4_KIO_LIBRARY} ${KDE4_PLASMA_LIBS} ${KDE4_KNEWSTUFF3_LIBRARY} ${QT_QTWEBKIT_LIBRARY})

install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasmate.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES plasmate.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

message("\n******************************REQUIRED PACKAGES******************************")
if(NOT KDE4_FOUND)
  message("* KDE4 libraries and headers not foud! KDE libraries are required to build the whole app:")
  message("  double check your build environment, adjust the settings and then run cmake again.")
endif(NOT KDE4_FOUND)
if(NOT LIBATTICA_FOUND)
  message("* Attica library and headers not found! LibAttica is required in order to build the GHNS client.")
endif(NOT LIBATTICA_FOUND)

if(KDE4_FOUND)
  message("* KDE4 libraries and headers found!")
endif(KDE4_FOUND)
if(LIBATTICA_FOUND)
  message("* Attica library and headers found!")
endif(LIBATTICA_FOUND)

message("\n******************************OPTIONAL PACKAGES******************************")
if(NOT GIT_EXECUTABLE)
  message("* Git executable not found! You need to install git-core in order to use the TimeLine!")
endif(NOT GIT_EXECUTABLE)

if(GIT_EXECUTABLE)
  message("* Git executable found! The TimeLine feature will be available!")
endif(GIT_EXECUTABLE)

if(KDE4_FOUND AND LIBATTICA_FOUND)
  message("\nNow just run make and make install to enjoy PlasMate :)")
endif(KDE4_FOUND AND LIBATTICA_FOUND)

