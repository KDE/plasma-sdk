# Find needed packages

# No need to find(Qt4), since QT4_FOUND it's already set by finding kde4
add_feature_info(QT_QTWEBKIT_FOUND "QtWebKit" "Opensource web browser engine" "http://trac.webkit.org/wiki/QtWebKit" TRUE "" "Needed to display the online API and documentation.")

#TODO this dependency is inside from the KdepimLibs which has not
#been ported yet. So we are disabling it.
#find_package(QGpgme REQUIRED) # Maybe check for GPGME_PTHREAD_FOUND ?
#find_package(KdepimLibs REQUIRED)

# Find optional executables
find_program(GIT_EXECUTABLE_FOUND git)
add_feature_info(GIT_EXECUTABLE_FOUND "Git" "Distributed Revision Control System" "http://git-scm.com/" FALSE "" "Runtime dependency needed to use the TimeLine feature.")

set (PlasMate_SRC
    editors/editpage.cpp
    #editors/metadata/metadataeditor.cpp
    editors/imageviewer/imageloader.cpp
    editors/imageviewer/imageviewer.cpp
    #editors/text/texteditor.cpp
    editors/kconfigxt/kconfigxteditor.cpp
    editors/kconfigxt/kconfigxtreader.cpp
    editors/kconfigxt/kconfigxtwriter.cpp
    editors/metadata/metadatahandler.cpp
    main.cpp
    sidebaritem.cpp
    sidebardelegate.cpp
    sidebartablewidget.cpp
    sidebar.cpp
    #savesystem/dvcsjob.cpp
    #savesystem/gitrunner.cpp
    #savesystem/commitdialog.cpp
    #savesystem/timelineitem.cpp
    #savesystem/tabledelegate.cpp
    #savesystem/tablewidget.cpp
    #savesystem/timeline.cpp
    #mainwindow.cpp
    #startpage.cpp
    #packagemodel.cpp
    #previewer/plasmoid/plasmoidpreviewer.cpp
    #previewer/plasmoid/plasmoidview.cpp
    #previewer/runner/runnerpreviewer.cpp
    #previewer/windowswitcher/windowswitcher.cpp
    #previewer/windowswitcher/thumbnailitem.cpp
    #previewer/windowswitcher/tabboxpreviewer.cpp
    #previewer/previewer.cpp
    # publisher/signingwidget.cpp
    #publisher/signingdialog.cpp
    publisher/publisher.cpp
    #publisher/remoteinstaller/remoteinstaller.cpp
    #publisher/remoteinstaller/remoteinstallerdialog.cpp
    #konsole/konsolepreviewer.cpp
    #docbrowser/docbrowser.cpp
    #projectmanager/projectmanager.cpp
)

#if (DEBUG_MODEL)
#      set (PlasMate_SRC
#         ${PlasMate_SRC}
#         modeltest/dynamictreemodel.cpp
#         modeltest/modeltest.cpp
#         )
#endif (DEBUG_MODEL)

kde4_add_ui_files (PlasMate_SRC
    #startpage.ui
   #editors/metadata/metadata.ui
   publisher/publisher.ui
   #publisher/remoteinstaller/remoteinstaller.ui
   editors/kconfigxt/kconfigxteditor.ui
)

include_directories(${QT_QTWEBKIT_INCLUDE_DIR}
                    #${QGPGME_INCLUDE_DIR}
                    #${GPGME_INCLUDES}
)

#add_subdirectory( editors )
#add_subdirectory( previewer/plasmoid )
#add_subdirectory( icons )

add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate
                      #KF5::KTextEditor
                      KF5::Plasma
                      KF5::NewStuff
                      Qt5::Xml
                      #${QGPGME_LIBRARIES}
                      KF5::Declarative
                      Qt5::Quick
                      Qt5::Svg
                      KF5::PlasmaQuick
                      ${LibAttica_LIBRARIES}
                      #Qt5::WebKitWidgets
                      KF5::KIOCore
                      KF5::ItemViews
                      KF5::ConfigCore
                      KF5::IconThemes
                      KF5::I18n
                      KF5::KIOWidgets
                      KF5::Service
                      KF5::Completion
                      KF5::WidgetsAddons
                      #${KDEPIMLIBS_KPIMUTILS_LIBS}
)

#if (DEBUG_MODEL)
#   target_link_libraries(plasmate Qt5::Test)
#endif (DEBUG_MODEL)

install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS plasmate.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES plasmate.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

#install( FILES previewer/windowswitcher/thumbnails/konqueror.png
#previewer/windowswitcher/thumbnails/kmail.png
#previewer/windowswitcher/thumbnails/systemsettings.png
#previewer/windowswitcher/thumbnails/dolphin.png
#DESTINATION ${DATA_INSTALL_DIR}/plasmate)
#install( FILES previewer/windowswitcher/tabboxdelegate.qml DESTINATION ${DATA_INSTALL_DIR}/plasmate)

##standalone applications
#plasmaremoteinstaller
#set (PlasmaRemoteInstaller_SRC
#    publisher/remoteinstaller/remoteinstallerdialog.cpp
#    publisher/remoteinstaller/remoteinstaller.cpp
#    publisher/remoteinstaller/standalone/main.cpp
#    publisher/remoteinstaller/standalone/plasmaremoteinstaller.cpp
#)

#kde4_add_ui_files (PlasmaRemoteInstaller_SRC
#   publisher/remoteinstaller/remoteinstaller.ui
#)

#add_executable(plasmaremoteinstaller ${PlasmaRemoteInstaller_SRC})
#target_link_libraries(plasmaremoteinstaller
#                      ${KDE4_KDEUI_LIBS}
#                      ${KDE4_KIO_LIBRARY}
#)

#install(TARGETS plasmaremoteinstaller ${INSTALL_TARGETS_DEFAULT_ARGS})

#kwin-windowswitcherpreviewer
#set (KWin-WindowSwitcherPreviewer_SRC
#    previewer/windowswitcher/windowswitcher.cpp
#    previewer/windowswitcher/thumbnailitem.cpp
#    previewer/windowswitcher/tabboxpreviewer.cpp
#    previewer/previewer.cpp
#    previewer/windowswitcher/standalone/main.cpp
#    previewer/windowswitcher/standalone/windowswitcherpreviewer.cpp
#)

#add_executable(kwin-windowswitcherpreviewer ${KWin-WindowSwitcherPreviewer_SRC})
#target_link_libraries(kwin-windowswitcherpreviewer
#                      KF5::KDeclarative
#                      Qt5::Quick
#                      Qt5::KIO
#)

#install(TARGETS kwin-windowswitcherpreviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

#plasmakconfigxteditor
set(plasmakconfigxt_SRCS
     editors/kconfigxt/kconfigxteditor.cpp
     editors/kconfigxt/kconfigxtreader.cpp
     editors/kconfigxt/kconfigxtwriter.cpp
     editors/kconfigxt/standalone/plasmakconfigxteditor.cpp
     editors/kconfigxt/standalone/main.cpp
)

kde4_add_ui_files (plasmakconfigxt_SRCS
    editors/kconfigxt/kconfigxteditor.ui
)

add_executable(plasmakconfigxteditor ${plasmakconfigxt_SRCS})
target_link_libraries(plasmakconfigxteditor
                        Qt5::Widgets
                        KF5::CoreAddons
                        KF5::ConfigCore
                        KF5::KIOCore
                        KF5::I18n
                        Qt5::Xml
                        KF5::Completion
                        KF5::WidgetsAddons
                        KF5::KIOWidgets
)

install(TARGETS plasmakconfigxteditor ${INSTALL_TARGETS_DEFAULT_ARGS})

install(DIRECTORY templates DESTINATION ${DATA_INSTALL_DIR}/plasmate)

add_subdirectory(tests)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
