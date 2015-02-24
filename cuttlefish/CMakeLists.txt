project(cuttlefish)
add_definitions(-DTRANSLATION_DOMAIN=\"cuttlefish\")

add_subdirectory(src)
add_subdirectory(autotests)

plasma_install_package(package org.kde.plasma.cuttlefish packages package)
# install(FILES cuttlefish.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})


