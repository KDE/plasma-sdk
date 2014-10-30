set(kdevplasmamanager_SRCS
    plasmaprojectmanager.cpp
    plasmaitem.cpp
    ${packagehandler_SRCS}
)

add_library(kdevplasmamanager MODULE ${kdevplasmamanager_SRCS})
target_link_libraries(kdevplasmamanager
    KF5::KIOWidgets
    KDev::Project
    KDev::Util
    KF5::Plasma
)

install(TARGETS kdevplasmamanager DESTINATION ${PLUGIN_INSTALL_DIR}/kdevplatform/${KDEV_PLUGIN_VERSION} )


########### install files ###############

configure_file(kdevplasmamanager.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdevplasmamanager.desktop)
kcoreaddons_desktop_to_json(kdevplasmamanager ${CMAKE_CURRENT_BINARY_DIR}/kdevplasmamanager.desktop)

