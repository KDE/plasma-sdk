add_definitions(-DTRANSLATION_DOMAIN=\"org.kde.plasma.globalthemeexplorer\")

add_subdirectory(src)
install(FILES org.kde.globalthemeexplorer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
