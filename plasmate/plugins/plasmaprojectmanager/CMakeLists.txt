set(kdevplasmamanager_SRCS
    plasmaprojectmanager.cpp
    plasmaitem.cpp
    ${packagehandler_SRCS}
)

kdevplatform_add_plugin(kdevplasmamanager JSON kdevplasmamanager.json SOURCES ${kdevplasmamanager_SRCS})
target_link_libraries(kdevplasmamanager
    KDev::Project
    KDev::Util
    KF5::KIOWidgets
    KF5::Plasma
)
