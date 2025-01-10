add_definitions(-DTRANSLATION_DOMAIN=\"org.kde.lookandfeelexplorer\")

add_subdirectory(src)

kpackage_install_package(package org.kde.lookandfeelexplorer genericqml)

