# Find needed packages

add_feature_info(QT_QTWEBKIT_FOUND "QtWebKit" "Opensource web browser engine" "http://trac.webkit.org/wiki/QtWebKit" TRUE "" "Needed to display the online API and documentation.")

#TODO this dependency is inside from the KdepimLibs which has not
#been ported yet. So we are disabling it.
#find_package(QGpgme REQUIRED) # Maybe check for GPGME_PTHREAD_FOUND ?
#find_package(KdepimLibs REQUIRED)

# Find optional executables
find_program(GIT_EXECUTABLE_FOUND git)
add_feature_info(GIT_EXECUTABLE_FOUND "Git" "Distributed Revision Control System" "http://git-scm.com/" FALSE "" "Runtime dependency needed to use the TimeLine feature.")

set( KDEVELOP_VERSION_MAJOR 4 )
set( KDEVELOP_VERSION_MINOR 90 )
set( KDEVELOP_VERSION_PATCH 60 )
# KDevplatform dependency version
set( KDEVPLATFORM_VERSION "1.${KDEVELOP_VERSION_MINOR}.${KDEVELOP_VERSION_PATCH}" )

find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)

#install( FILES previewer/windowswitcher/thumbnails/konqueror.png
#previewer/windowswitcher/thumbnails/kmail.png
#previewer/windowswitcher/thumbnails/systemsettings.png
#previewer/windowswitcher/thumbnails/dolphin.png
#DESTINATION ${DATA_INSTALL_DIR}/plasmate)
#install( FILES previewer/windowswitcher/tabboxdelegate.qml DESTINATION ${DATA_INSTALL_DIR}/plasmate)

##standalone applications
#plasmaremoteinstaller
set (PlasmaRemoteInstaller_SRC
    publisher/remoteinstaller/remoteinstallerdialog.cpp
    publisher/remoteinstaller/remoteinstaller.cpp
    publisher/remoteinstaller/standalone/main.cpp
    publisher/remoteinstaller/standalone/plasmaremoteinstaller.cpp
)

qt5_wrap_ui (PlasmaRemoteInstaller_SRC
   publisher/remoteinstaller/remoteinstaller.ui
)

add_executable(plasmaremoteinstaller ${PlasmaRemoteInstaller_SRC})
target_link_libraries(plasmaremoteinstaller
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::KDE4Support
)

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

qt5_wrap_ui (plasmakconfigxt_SRCS
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

set(packagehandler_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/packagehandler/packagehandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/packagehandler/plasmoidhandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/packagehandler/themehandler.cpp
)

include(ECMMarkAsTest)

if(BUILD_TESTING)
    include(ECMMarkAsTest)
    macro(PLASMATE_UNIT_TEST testname srcs libraries)
        set(testname_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${testname}.cpp
                          ${srcs}
        )

        add_executable(${testname} ${testname_SRCS})
        add_test(plasmate-${testname} ${testname})

        ecm_mark_as_test(${testname})

        target_link_libraries(${testname} ${libraries} Qt5::Test Qt5::Core)
    endmacro()

    add_subdirectory(tests)
endif()

add_subdirectory(app)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
