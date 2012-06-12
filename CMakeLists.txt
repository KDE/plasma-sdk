project(PlasMate)

# Find needed packages
find_package(KDE4 REQUIRED)
include(MacroLibrary)
find_package(KDeclarative REQUIRED)
set(LIBATTICA_MIN_VERSION "0.1.4")
find_package(LibAttica REQUIRED)
macro_log_feature(LIBATTICA_FOUND "libattica" "A library to access Open Collaboration Service providers" "http://websvn.kde.org/trunk/kdesupport/attica/" TRUE "${LIBATTICA_MIN_VERSION}" "Required uploading and downloading plasmoids using Get Hot New Stuff.")

# No need to find(Qt4), since QT4_FOUND it's already set by finding kde4
macro_log_feature(QT_QTWEBKIT_FOUND "QtWebKit" "Opensource web browser engine" "http://trac.webkit.org/wiki/QtWebKit" TRUE "" "Needed to display the online API and documentation.")

find_package(QGpgme REQUIRED) # Maybe check for GPGME_PTHREAD_FOUND ?

# Find optional executables
find_program(GIT_EXECUTABLE_FOUND git)
macro_log_feature(GIT_EXECUTABLE_FOUND "Git" "Distributed Revision Control System" "http://git-scm.com/" FALSE "" "Runtime dependency needed to use the TimeLine feature.")

include(KDE4Defaults)

string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
if(CMAKE_BUILD_TYPE_TOLOWER MATCHES debugfull)
   add_definitions(-DDEBUG_MODEL)
   set (DEBUG_MODEL 1)
endif()

set (PlasMate_SRC
    editors/editpage.cpp
    editors/metadata/metadataeditor.cpp
    editors/imageviewer/imageloader.cpp
    editors/imageviewer/imageviewer.cpp
    editors/text/texteditor.cpp
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
    previewer/windowswitcher/windowswitcher.cpp
    previewer/windowswitcher/thumbnailitem.cpp
    previewer/windowswitcher/tabboxpreviewer.cpp
    previewer/previewer.cpp
    publisher/signingwidget.cpp
    publisher/signingdialog.cpp
    publisher/publisher.cpp
    docbrowser/docbrowser.cpp
    projectmanager/projectmanager.cpp
    )

set (WindowSwitcherPreviewer_SRC
    previewer/windowswitcher/windowswitcher.cpp
    previewer/windowswitcher/thumbnailitem.cpp
    previewer/windowswitcher/tabboxpreviewer.cpp
    previewer/previewer.cpp
    previewer/windowswitcher/standalone/main.cpp
    previewer/windowswitcher/standalone/windowswitcherpreviewer.cpp
)
if (DEBUG_MODEL)
      set (PlasMate_SRC
         ${PlasMate_SRC}
         modeltest/dynamictreemodel.cpp
         modeltest/modeltest.cpp
         )
endif (DEBUG_MODEL)

kde4_add_ui_files (PlasMate_SRC
   startpage.ui
   editors/metadata/metadata.ui
   publisher/publisher.ui
)

include_directories(${KDE4_INCLUDES}
                    ${QT_QTWEBKIT_INCLUDE_DIR}
                    ${QGPGME_INCLUDE_DIR}
                    ${GPGME_INCLUDES}
                    ${KDECLARATIVE_INCLUDE_DIR}
)

add_subdirectory( editors )
add_subdirectory( previewer/plasmoid )
add_subdirectory( templates )

kde4_add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KTEXTEDITOR_LIBS}
                      ${KDE4_KPARTS_LIBS}
                      ${KDE4_KIO_LIBRARY}
                      ${KDE4_PLASMA_LIBS}
                      ${KDE4_KNEWSTUFF3_LIBRARY}
                      ${QT_QTWEBKIT_LIBRARY}
                      ${LIBATTICA_LIBRARY_DIRS}
                      ${QGPGME_LIBRARIES}
                      ${QT_QTDECLARATIVE_LIBRARIES}
                      ${KDECLARATIVE_LIBRARIES}
)

kde4_add_executable(windowswitcherpreviewer ${WindowSwitcherPreviewer_SRC})
target_link_libraries(windowswitcherpreviewer
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBRARY}
                      ${QT_QTDECLARATIVE_LIBRARIES}
                      ${KDECLARATIVE_LIBRARIES}
)



if (DEBUG_MODEL)
   target_link_libraries(plasmate ${QT_QTTEST_LIBRARY})
endif (DEBUG_MODEL)

install(TARGETS windowswitcherpreviewer ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES plasmate.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES plasmate.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

install( FILES previewer/windowswitcher/tabboxpreviewer.h DESTINATION ${INCLUDE_INSTALL_DIR})
install( FILES previewer/windowswitcher/thumbnails/konqueror.png
previewer/windowswitcher/thumbnails/kmail.png
previewer/windowswitcher/thumbnails/systemsettings.png
previewer/windowswitcher/thumbnails/dolphin.png
DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install( FILES previewer/windowswitcher/tabboxdelegate.qml DESTINATION ${DATA_INSTALL_DIR}/plasmate)
macro_display_feature_log()
