project(PlasMate)

# Find needed packages
find_package(KDE4 REQUIRED)
include(MacroLibrary)

set(LIBATTICA_MIN_VERSION "0.1.4")
find_package(LibAttica REQUIRED)
macro_log_feature(LIBATTICA_FOUND "libattica" "A library to access Open Collaboration Service providers" "http://websvn.kde.org/trunk/kdesupport/attica/" TRUE "${LIBATTICA_MIN_VERSION}" "Required uploading and downloading plasmoids using Get Hot New Stuff.")

find_package(QCA2 REQUIRED)
macro_log_feature(QCA2_FOUND "QCA2" "Qt Cryptographic Architecture" "http://delta.affinix.com/qca" TRUE "2.0.2" "Needed for plasmoid authentication.")

# Find optional executables
find_program(GIT_EXECUTABLE_FOUND git)
macro_log_feature(GIT_EXECUTABLE_FOUND "Git" "Distributed Revision Control System" "http://git-scm.com/" FALSE "" "Runtime dependency needed to use the TimeLine feature.")

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
    publisher/signingwidget.cpp
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

macro_display_feature_log()
