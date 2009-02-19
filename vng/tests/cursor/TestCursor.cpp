#include "TestCursor.h"
#include "hunks/HunksCursor.h"
#include "hunks/File.h"
#include "hunks/ChangeSet.h"

class MyHunksCursor : public HunksCursor {
public:
    MyHunksCursor(ChangeSet &changeSet)
        : HunksCursor(changeSet)
    {
    }

    int atFile() const
    {
        return m_fileIndex;
    }
    int atHunk() const
    {
        return m_hunkIndex;
    }

    int atSubhunk() const
    {
        return m_subHunkIndex;
    }

    void setPosition(int file, int hunk, int subhunk)
    {
        m_fileIndex = file;
        m_hunkIndex = hunk;
        m_subHunkIndex = subhunk;
    }

    int currentIndexOpen()
    {
        return currentIndex();
    }
};

void TestCursor::testBasis() {
    // these are all using the same backend to make moving around objects easier.
    File file1;
    File file2 = file1;
    File file3(file1);
    File file4; // a different one :)

    QByteArray fn("testfilename");
    file1.setFileName(fn);
    QCOMPARE(file1.fileName(), fn);
    QCOMPARE(file2.fileName(), fn);
    QCOMPARE(file3.fileName(), fn);
    QVERIFY(file4.fileName() != fn);

    Hunk hunk1;
    Hunk hunk2 = hunk1;
    Hunk hunk3(hunk1);
    Hunk hunk4;
    hunk1.addLine(fn);
    hunk1.addLine(fn);

    QCOMPARE(hunk1.header(), fn);
    QCOMPARE(hunk2.header(), fn);
    QCOMPARE(hunk3.header(), fn);
}

void TestCursor::testForward1() {
    ChangeSet changeSet;
    File file;
    file.setFileName("foo");
    file.setOldFileName("bar");
    file.setProtection("100755");
    file.setOldProtection("100755");
    changeSet.addFile(file);

    QCOMPARE(changeSet.count(), 1);

    MyHunksCursor cursor(changeSet);
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.currentText(), QString("move `bar' `foo'\n"));
    QCOMPARE(cursor.forward(), 2);
    QCOMPARE(cursor.atFile(), 1);
    QCOMPARE(cursor.atHunk(), 0);
    QCOMPARE(cursor.atSubhunk(), 0);
    QCOMPARE(cursor.isValid(), false);
    QCOMPARE(cursor.currentText(), QString());

    QCOMPARE(cursor.back(), 1);
    QCOMPARE(cursor.atFile(), 0);
    QCOMPARE(cursor.atHunk(), 0);
    QCOMPARE(cursor.atSubhunk(), 0);
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.back(), 1);
    QCOMPARE(cursor.isValid(), true);
}

void TestCursor::testForward2() {
    ChangeSet changeSet;
    File newFile;
    newFile.setFileName("foo");
    newFile.setProtection("100755");
    newFile.setOldProtection("000000");
    QVERIFY(newFile.isValid());
    changeSet.addFile(newFile);

    File file;
    file.setFileName("filename");
    file.setOldFileName("filename");
    file.setProtection("100644");
    file.setOldProtection("100644");
    Hunk hunk;
    hunk.addLine(QByteArray("@@ -1,4 +1,4 @@"));
    hunk.addLine(" foo");
    hunk.addLine("+bar");
    hunk.addLine(" separator");
    hunk.addLine("-another subhunk");
    QCOMPARE(hunk.subHunkCount(), 2);
    QCOMPARE(hunk.lineNumber(), 1);
    QCOMPARE(hunk.lineNumber(0), 2);
    QCOMPARE(hunk.lineNumber(1), 4);
    file.addHunk(hunk);
    QVERIFY(file.isValid());
    changeSet.addFile(file);

    QCOMPARE(changeSet.count(), 2);

    MyHunksCursor cursor(changeSet);
    cursor.forceCount();
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.count(), 3);
    QCOMPARE(cursor.back(), 1);
    QCOMPARE(cursor.atFile(), 0);
    QCOMPARE(cursor.atHunk(), 0);
    QCOMPARE(cursor.atSubhunk(), 0);
    QCOMPARE(cursor.forward(), 2);
    QCOMPARE(cursor.atFile(), 1);
    QCOMPARE(cursor.atHunk(), 2);
    QCOMPARE(cursor.atSubhunk(), 0);
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.forward(), 3);
    QCOMPARE(cursor.atFile(), 1);
    QCOMPARE(cursor.atHunk(), 2);
    QCOMPARE(cursor.atSubhunk(), 1);
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.forward(), 4);
    QCOMPARE(cursor.atFile(), 2); // non existing file.
    QCOMPARE(cursor.isValid(), false);

    QCOMPARE(cursor.back(InterviewCursor::FullRange), 1);
    QCOMPARE(cursor.isValid(), true);
    QCOMPARE(cursor.forward(InterviewCursor::FullRange), 4);
    QCOMPARE(cursor.isValid(), false);

    QCOMPARE(cursor.back(InterviewCursor::FullRange), 1);
    QCOMPARE(cursor.forward(InterviewCursor::BlockScope), 2);
    QCOMPARE(cursor.forward(InterviewCursor::BlockScope), 4);

    QCOMPARE(cursor.back(InterviewCursor::FullRange), 1);
    cursor.setResponse(true);
    QCOMPARE(cursor.forward(), 2);
    cursor.setResponse(true);
    QCOMPARE(cursor.forward(), 3);

    QCOMPARE(cursor.back(InterviewCursor::FullRange), 1);
    QCOMPARE(cursor.forward(), 3);

    QCOMPARE(cursor.back(InterviewCursor::FullRange), 1);
    QCOMPARE(cursor.forward(InterviewCursor::ItemScope, false), 2);
    QCOMPARE(cursor.forward(InterviewCursor::ItemScope, false), 3);
    QCOMPARE(cursor.forward(InterviewCursor::ItemScope, true), 4);

    File file2;
    file2.setFileName("document");
    file2.setOldFileName("document");
    file2.setProtection("100644");
    file2.setOldProtection("100644");
    hunk = Hunk();
    hunk.addLine(QByteArray("@@ -10,4 +10,4 @@"));
    hunk.addLine(" foo");
    hunk.addLine("+bar");
    hunk.addLine(" separator");
    hunk.addLine("-another subhunk");
    file2.addHunk(hunk);
    ChangeSet cs;
    cs.addFile(newFile);
    cs.addFile(file);
    cs.addFile(file2);
    MyHunksCursor myCursor(cs);

    QCOMPARE(myCursor.atFile(), 0);
    QCOMPARE(myCursor.atHunk(), 0);
    QCOMPARE(myCursor.atSubhunk(), 0);
    QCOMPARE(myCursor.back(InterviewCursor::FullRange), 1);
    QCOMPARE(myCursor.atFile(), 0);
    QCOMPARE(myCursor.atHunk(), 0);
    QCOMPARE(myCursor.atSubhunk(), 0);
    QCOMPARE(myCursor.forward(InterviewCursor::ItemScope, false), 2);
    QCOMPARE(myCursor.atFile(), 1);
    QCOMPARE(myCursor.atHunk(), 2);
    QCOMPARE(myCursor.atSubhunk(), 0);
    QCOMPARE(myCursor.forward(InterviewCursor::ItemScope, false), 3);
    QCOMPARE(myCursor.atFile(), 1);
    QCOMPARE(myCursor.atHunk(), 2);
    QCOMPARE(myCursor.atSubhunk(), 1);
    QCOMPARE(myCursor.forward(InterviewCursor::ItemScope, false), 4);
    QCOMPARE(myCursor.atFile(), 2);
    QCOMPARE(myCursor.atHunk(), 2);
    QCOMPARE(myCursor.atSubhunk(), 0);
    QCOMPARE(myCursor.forward(InterviewCursor::ItemScope, false), 5);
    QCOMPARE(myCursor.atFile(), 2);
    QCOMPARE(myCursor.atHunk(), 2);
    QCOMPARE(myCursor.atSubhunk(), 1);
    QCOMPARE(myCursor.forward(InterviewCursor::ItemScope, false), 6);
    QCOMPARE(myCursor.atFile(), 3);
    QCOMPARE(myCursor.atHunk(), 0);
    QCOMPARE(myCursor.atSubhunk(), 0);

    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 5);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 4);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 3);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 2);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 1);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 1);
}

void TestCursor::testPartialSelection()
{
    ChangeSet changeSet = createChangeSet();
    MyHunksCursor myCursor(changeSet);
    QCOMPARE( myCursor.atFile(), 0);
    QCOMPARE( myCursor.atHunk(), 2);
    QCOMPARE( myCursor.atSubhunk(), 0);
    QCOMPARE( myCursor.currentIndexOpen(), 1);
    myCursor.setResponse(false);
    QCOMPARE( myCursor.currentIndexOpen(), 1);
    QCOMPARE( myCursor.forward(), 2);
    QCOMPARE( myCursor.atFile(), 0);
    QCOMPARE( myCursor.atHunk(), 2);
    QCOMPARE( myCursor.atSubhunk(), 1);
    QCOMPARE( myCursor.currentIndexOpen(), 2);
    myCursor.setResponse(true, InterviewCursor::FullRange);

    File file = changeSet.file(0);
    bool first = true;
    int x = 0;
    foreach(Hunk hunk, file.hunks()) {
        for (int sh = 0; sh < hunk.subHunkCount(); ++sh) {
            //qDebug() << "acceptance"<< x  << sh << "is" << hunk.acceptance(sh);
            QCOMPARE(hunk.acceptance(sh), first ? Vng::Rejected : Vng::Accepted);
            first = false;
        }
        ++x;
    }
}

ChangeSet TestCursor::createChangeSet()
{
    ChangeSet changeSet;
    File file;
    file.setFileName("filename");
    file.setOldFileName("filename");
    file.setProtection("100644");
    file.setOldProtection("100644");
    QList<QByteArray> patch;
    patch.append(" foo\n");
    patch.append("+bar\n");
    patch.append("+b\n");
    patch.append("+c\n");
    patch.append(" separator\n");
    patch.append("-another subhunk\n");
    patch.append("-c\n");
    patch.append(" separator\n");
    patch.append("+another subhunk\n");

    Hunk hunk;
    hunk.addLine(QByteArray("@@ -1,4 +1,4 @@"));
    foreach(QByteArray s, patch) hunk.addLine(s);
    file.addHunk(hunk);

    Hunk hunk2;
    hunk2.addLine(QByteArray("@@ -30,4 +40,4 @@"));
    foreach(QByteArray s, patch) hunk2.addLine(s);
    file.addHunk(hunk2);
    changeSet.addFile(file);
    return changeSet;
}

void TestCursor::testBackwards()
{
    ChangeSet changeSet = createChangeSet();
    MyHunksCursor myCursor(changeSet);
    myCursor.setPosition(0, 3, 2);
    QCOMPARE(myCursor.currentIndexOpen(), 6);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 5);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 4);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 3);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 2);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 1);
    QCOMPARE(myCursor.back(InterviewCursor::ItemScope), 1);
}

QTEST_MAIN(TestCursor)
