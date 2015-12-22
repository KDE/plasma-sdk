set(kdevsavesystemview_SRCS
    savesystemviewplugin.cpp
    savesystemview.cpp
    commitsmodel.cpp
    git.cpp
)

add_library(kdevsavesystemview MODULE ${kdevsavesystemview_SRCS})
target_link_libraries(kdevsavesystemview
    KDev::Interfaces
    KDev::Project
    KDev::Util
    KDev::Vcs
    Qt5::Quick
    KF5::Declarative
)

install(TARGETS kdevsavesystemview DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

configure_file(kdevsavesystemview.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdevsavesystemview.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdevsavesystemview.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install(FILES kdevsavesystemview.rc DESTINATION ${DATA_INSTALL_DIR}/kdevsavesystemview )
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kdevsavesystemview)

