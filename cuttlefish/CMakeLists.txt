project(cuttlefish)


add_subdirectory(src)

plasma_install_package(package org.kde.plasma.cuttlefish packages package)
# install(FILES cuttlefish.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})


