project(cuttlefish)
add_definitions(-DTRANSLATION_DOMAIN=\"cuttlefish\")

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()


plasma_install_package(package org.kde.plasma.cuttlefish packages package)
install(FILES org.kde.cuttlefish.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})


