# needed for packagehandler/* and editors/metadata/metadatahandler.cpp
# in order to avoid a hell of "#include ../../foo.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(startpage_SRCS
    startpage/startpage.cpp
    startpage/projecthandler.cpp
    startpage/projectmanager.cpp
)

qt5_wrap_ui(startpage_SRCS
   startpage/startpage.ui
)

set (PlasMate_SRC
    plasmateextention.cpp
    plasmateapp.cpp
    main.cpp
    ../editors/metadata/metadatahandler.cpp
    ${packagehandler_SRCS}
    ${startpage_SRCS}
)

add_executable(plasmate ${PlasMate_SRC})
target_link_libraries(plasmate
                      KF5::NewStuff
                      Qt5::Gui
                      Qt5::Widgets
                      KF5::PlasmaQuick
                      KF5::Archive
                      #${KDEPIMLIBS_KPIMUTILS_LIBS}
                      ${KDEVPLATFORM_INTERFACES_LIBRARIES}
                      ${KDEVPLATFORM_SHELL_LIBRARIES}
)

install(TARGETS plasmate ${INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS plasmate.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES plasmateui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES katepartui.rc  DESTINATION ${DATA_INSTALL_DIR}/plasmate)
install(FILES plasmate.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

