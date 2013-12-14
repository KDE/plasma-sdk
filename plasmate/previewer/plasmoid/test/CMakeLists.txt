set(previewer_SRCS
    ../plasmoidview.cpp
    main.cpp
)

add_executable(plasma-previewer ${previewer_SRCS})
target_link_libraries(plasma-previewer ${KDE4_KDEUI_LIBS} KF5::Plasma)
#install(TARGETS plasma-previewer ${INSTALL_TARGETS_DEFAULT_ARGS})
