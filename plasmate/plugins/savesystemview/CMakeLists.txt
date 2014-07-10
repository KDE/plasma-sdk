set(kdevsavesystemview_SRCS
    savesystemviewplugin.cpp
    savesystemview.cpp
    commitsmodel.cpp
    git.cpp
)

add_library(kdevsavesystemview MODULE ${kdevsavesystemview_SRCS})
target_link_libraries(kdevsavesystemview ${KDEVPLATFORM_INTERFACES_LIBRARIES}
                                         ${KDEVPLATFORM_PROJECT_LIBRARIES}
                                         ${KDEVPLATFORM_UTIL_LIBRARIES}
                                         ${KDEVPLATFORM_VCS_LIBRARIES}
                                         Qt5::QuickWidgets
)

install(TARGETS kdevsavesystemview DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

configure_file(kdevsavesystemview.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdevsavesystemview.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kdevsavesystemview.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install(FILES kdevsavesystemview.rc DESTINATION ${DATA_INSTALL_DIR}/kdevsavesystemview )
install(DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kdevsavesystemview)

