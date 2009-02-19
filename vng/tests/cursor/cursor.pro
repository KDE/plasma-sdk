load(qttest_p4)
DEPENDPATH = ../../src
INCLUDEPATH += ../../src

HEADERS += TestCursor.h \
           hunks/HunksCursor.h \
           hunks/ChangeSet.h \
           hunks/File.h \
           hunks/Hunk.h \
           patches/Commit.h \
           patches/Branch.h \
           InterviewCursor.h \
           Configuration.h \
           Vng.h \
           Logger.h \
           Logger_p.h \
           CommandLineParser.h \
           GitRunner.h \


SOURCES += TestCursor.cpp \
           hunks/HunksCursor.cpp \
           hunks/ChangeSet.cpp \
           hunks/File.cpp \
           hunks/Hunk.cpp \
           patches/Commit.cpp \
           patches/Branch.cpp \
           InterviewCursor.cpp \
           Configuration.cpp \
           Vng.cpp \
           Logger.cpp \
           CommandLineParser.cpp \
           GitRunner.cpp \

