add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()


plasma_install_package(package org.kde.plasma.cuttlefish packages package)
install(FILES org.kde.plasma.cuttlefish.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.plasma.cuttlefish.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})


