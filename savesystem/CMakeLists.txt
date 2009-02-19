project( plasmate-savesystem )

set (savesystem_SRC
	author.cpp
	commit.cpp
	gitrunner.cpp
	logger.cpp
	savesystem.cpp
	vng.cpp
    )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRC})


