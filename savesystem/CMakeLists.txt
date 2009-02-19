project( plasmate-savesystem )

set (savesystem_SRCS
	author.cpp
	savesystem.cpp
    )

set(vng_SRCS
vng/GitRunner.cpp
vng/Configuration.cpp
vng/ExcludeMatcher.cpp
vng/InterviewCursor.cpp
vng/AbstractCommand.cpp
vng/Vng.cpp
vng/VngCommandLine.cpp
vng/Interview.cpp
vng/Logger.cpp
vng/CommandLineParser.cpp
vng/patches/CommitsMatcher.cpp
vng/patches/Commit.cpp
vng/patches/CommitsCursor.cpp
vng/patches/Branch.cpp
vng/hunks/ChangeSet.cpp
vng/hunks/HunksCursor.cpp
vng/hunks/File.cpp
vng/hunks/Hunk.cpp
vng/commands/UnRevert.cpp
vng/commands/Record.cpp
vng/commands/Dist.cpp
vng/commands/UnRecord.cpp
vng/commands/Remove.cpp
vng/commands/AmendRecord.cpp
vng/commands/WhatsNew.cpp
vng/commands/Push.cpp
vng/commands/Initialize.cpp
vng/commands/Revert.cpp
vng/commands/Branches.cpp
vng/commands/Move.cpp
vng/commands/Changes.cpp
vng/commands/Add.cpp
)

include_directories( vng/ )

kde4_add_plugin(plasmate_savesystem ${savesystem_SRCS} ${vng_SRCS})

target_link_libraries(plasmate_savesystem ${KDE4_KDEUI_LIBS})

