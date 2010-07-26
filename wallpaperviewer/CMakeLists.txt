set(plasmawallpaperviewer_SRCS
    main.cpp
    wallpaperwidget.cpp
)

kde4_add_executable(plasmawallpaperviewer ${plasmawallpaperviewer_SRCS})

target_link_libraries(plasmawallpaperviewer ${KDE4_PLASMA_LIBS})

install(TARGETS plasmawallpaperviewer ${INSTALL_TARGETS_DEFAULT_ARGS})

