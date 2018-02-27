add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()


plasma_install_package(package org.kde.plasma.cuttlefish packages package)
install(FILES org.kde.cuttlefish.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.cuttlefish.appdata.xml DESTINATION  ${CMAKE_INSTALL_METAINFODIR})


