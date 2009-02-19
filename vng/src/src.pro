TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
DESTDIR = ..
TARGET = vng
INCLUDEPATH += hunks patches commands
# this is a line we can enable for static linking
#LIBS += -Wl,-Bstatic -lQtCore -Wl,-Bdynamic

# Input
SOURCES += \
    Vng.cpp \
    CommandLineParser.cpp \
    AbstractCommand.cpp \
    ExcludeMatcher.cpp \
    GitRunner.cpp \
    Logger.cpp \
    Interview.cpp \
    InterviewCursor.cpp \
    Configuration.cpp \
    VngCommandLine.cpp \
    commands/Add.cpp  \
    commands/AmendRecord.cpp \
    commands/Branches.cpp  \
    commands/Changes.cpp  \
    commands/Dist.cpp \
    commands/Initialize.cpp  \
    commands/Move.cpp  \
    commands/Push.cpp \
    commands/Record.cpp \
    commands/Remove.cpp \
    commands/Revert.cpp \
    commands/UnRecord.cpp \
    commands/UnRevert.cpp \
    commands/WhatsNew.cpp \
    hunks/Hunk.cpp \
    hunks/File.cpp \
    hunks/ChangeSet.cpp \
    hunks/HunksCursor.cpp \
    patches/Commit.cpp \
    patches/Branch.cpp \
    patches/CommitsCursor.cpp \
    patches/CommitsMatcher.cpp \


HEADERS += \
    Vng.h \
    CommandLineParser.h \
    AbstractCommand.h \
    ExcludeMatcher.h \
    GitRunner.h \
    Logger_p.h \
    Logger.h \
    Interview.h \
    InterviewCursor.h \
    Configuration.h \
    VngCommandLine.h \
    commands/Add.h \
    commands/AmendRecord.h \
    commands/Branches.h  \
    commands/Changes.h  \
    commands/Dist.h \
    commands/Initialize.h  \
    commands/Move.h  \
    commands/Push.h \
    commands/Record.h \
    commands/Remove.h \
    commands/Revert.h \
    commands/UnRecord.h \
    commands/UnRevert.h \
    commands/WhatsNew.h \
    hunks/Hunk.h \
    hunks/File.h \
    hunks/ChangeSet.h \
    hunks/HunksCursor.h \
    patches/Commit.h \
    patches/Commit_p.h \
    patches/Branch.cpp \
    patches/CommitsCursor.h \
    patches/CommitsMatcher.h \


