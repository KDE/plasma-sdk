add_definitions(-DTRANSLATION_DOMAIN=\"org.kde.themeexplorer\")

add_subdirectory(src)

kpackage_install_package(package org.kde.themeexplorer genericqml)

