add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()


plasma_install_package(package org.kde.plasma.cuttlefish packages package)
install(FILES org.kde.cuttlefish.desktop DESTINATION ${KDE_INSTALL_APPDIR})


