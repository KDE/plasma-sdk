set(previewer_SRCS
    ../plasmoidview.cpp
    main.cpp
)

kde4_add_executable(plasma-previewer ${previewer_SRCS})
target_link_libraries(plasma-previewer ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})
#install(TARGETS plasma-previewer ${INSTALL_TARGETS_DEFAULT_ARGS})
