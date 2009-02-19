project( plasmate-savesystem )

set (savesystem_SRC
	author.cpp
	changeset.cpp
	commit.cpp
	file.cpp
	gitrunner.cpp
	hunk.cpp
	logger.cpp
	savesystem.cpp
	vng.cpp
    )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRC})


