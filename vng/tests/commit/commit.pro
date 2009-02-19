load(qttest_p4)
DEPENDPATH += ../../src
INCLUDEPATH += ../../src

HEADERS += TestReadCommit.h \
    patches/Commit.h \
    patches/Branch.h \
    GitRunner.h \
    Logger.h \
    Logger_p.h \
    Configuration.h \
    Vng.h \
    hunks/ChangeSet.h \
    hunks/File.h \
    hunks/Hunk.h \


SOURCES += TestReadCommit.cpp \
    patches/Commit.cpp \
    patches/Branch.cpp \
    GitRunner.cpp \
    Logger.cpp \
    Configuration.cpp \
    Vng.cpp \
    hunks/ChangeSet.cpp \
    hunks/File.cpp \
    hunks/Hunk.cpp \

