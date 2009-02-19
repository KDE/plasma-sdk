set(previewer_SRCS
    previewer.cpp
    main.cpp
)

kde4_add_executable(previewer ${previewer_SRCS})
target_link_libraries(previewer ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS})
