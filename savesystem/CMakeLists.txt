project(plasmate-savesystem)

set (savesystem_SRCS
	author.cpp
	savesystem.cpp
    )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRCS})

target_link_libraries(plasmate_savesystem ${KDE4_KDEUI_LIBS})

