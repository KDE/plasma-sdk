project(themeexplorer)
add_definitions(-DTRANSLATION_DOMAIN=\"org.kde.plasma.themeexplorer\")

add_subdirectory(src)

kpackage_install_package(package org.kde.plasma.themeexplorer genericqml)
# install(FILES metadata.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

install(FILES package/metadata.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} RENAME org.kde.plasma.themeexplorer.desktop)

