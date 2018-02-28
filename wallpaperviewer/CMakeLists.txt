set(plasmawallpaperviewer_SRCS
    main.cpp
    wallpaperwidget.cpp
)

kde4_add_executable(plasmawallpaperviewer ${plasmawallpaperviewer_SRCS})

target_link_libraries(plasmawallpaperviewer KF5::Plasma)

install(TARGETS plasmawallpaperviewer ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

