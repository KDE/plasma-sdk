#include "TestReadCommit.h"

#include "patches/Commit.h"
#include "hunks/ChangeSet.h"
#include "hunks/File.h"

void TestReadCommit::testBasis()
{
    // command:  git cat-file commit 39fda993b682a9d90a8aa2f173f862adae983df1
    const char *diff =
        "tree d83e7cced91211b8a14097253782a8e1fbbd7273\n"
        "parent 5d2ba8270cfcbca6859331a0352c549db9fe623b\n"
        "author Thomas Zander <zander@kde.org> 1211012950 +0200\n"
        "committer Thomas Zander <zander@lacuna> 1211012950 +0200\n"
        "\n"
        "fix issue 2, initialize refers to 'darcs'\n";

    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QCOMPARE(commit.tree(), QString("d83e7cced91211b8a14097253782a8e1fbbd7273"));
    QCOMPARE(commit.isValid(), true);
    QCOMPARE(commit.previousCommitsCount(), 1);
    QCOMPARE(commit.author(), QString("Thomas Zander <zander@kde.org>"));
    QCOMPARE(commit.committer(), QString("Thomas Zander <zander@lacuna>"));
    QCOMPARE(commit.logMessage(), QByteArray("fix issue 2, initialize refers to 'darcs'\n"));
}

void TestReadCommit::testCommit()
{
    const char *diff =
        "commit b65b6f0254bf6766f0ac09ee888865df520d9d73\n"
        "tree 9d177b7ce70a8c19136078efe59b87462a80868d\n"
        "parent 09d7e027099f04b891bdccac86ced9544c2c4ddf\n"
        "author Thomas Zander <Thomas.t.Zander@nokia.com> 1224085484 +0200\n"
        "committer Thomas Zander <zander@kde.org> 1224085484 +0200\n"
        "\n"
        "Added braces in the wrong locations..\n"
        "\n"
        ":100644 100644 7875343e8aa07a6bbe3752e28a195c3f29e002d8 cee7da7f5af8b919aed2651b0ed2e2b3d4f38e42 M	AbstractCommand.cpp\n"
        ":100644 100644 02cef7556df8f20a6b64eedc4b0b1d05f8f67366 0d6a2df4e56a2a0489d67c56a110ed8fd7a3f253 M	commands/Changes.cpp\n"
        ":100644 100644 0000000000000000000000000000000000000000 05ca86ee49d6c4bd25122c302eaf4a13a9bae04a A	commands/Revert.cpp\n"
        ":100644 100644 d84f94310699a9968b0dd8515c4f87c83d021205 0000000000000000000000000000000000000000 D	commands/UnRevert.cpp\n"
        "\n";

    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QCOMPARE(commit.commitTreeIsm(), QString("b65b6f0254bf6766f0ac09ee888865df520d9d73"));
    QCOMPARE(commit.isValid(), true);
    QCOMPARE(commit.previousCommitsCount(), 1);
    QCOMPARE(commit.author(), QString("Thomas Zander <Thomas.t.Zander@nokia.com>"));
    QCOMPARE(commit.committer(), QString("Thomas Zander <zander@kde.org>"));
    QCOMPARE(commit.logMessage(), QByteArray("Added braces in the wrong locations..\n"));
    ChangeSet cs = commit.changeSet();
    QCOMPARE(cs.count(), 4);
    File file1 = cs.file(0);
    QCOMPARE(file1.fileName(), file1.oldFileName());
    QCOMPARE(file1.fileName(), QByteArray("AbstractCommand.cpp"));
    QCOMPARE(file1.oldSha1(), QString("7875343e8aa07a6bbe3752e28a195c3f29e002d8"));
    QCOMPARE(file1.sha1(), QString("cee7da7f5af8b919aed2651b0ed2e2b3d4f38e42"));
    File file2 = cs.file(1);
    QCOMPARE(file2.fileName(), file2.oldFileName());
    QCOMPARE(file2.fileName(), QByteArray("commands/Changes.cpp"));
    File file3 = cs.file(2);
    QVERIFY(file3.fileName() != file3.oldFileName());
    QCOMPARE(file3.fileName(), QByteArray("commands/Revert.cpp"));
    QVERIFY(file3.oldFileName().isEmpty());
    File file4 = cs.file(3);
    QCOMPARE(file4.oldFileName(), QByteArray("commands/UnRevert.cpp"));
    QVERIFY(file4.fileName().isEmpty());
}

void TestReadCommit::readFromCatFile()
{
    // No commit line; output from 'cat-file commit [f00ba5]
    const char *diff =
        "tree c51f5c18e0aaf97ae9ef38f59470847e5b809d51\n"
        "parent 4a186a6a296a486dd32b772f8167af0984fad0f6\n"
        "author Thomas Zander <Thomas.t.Zander@nokia.com> 1224060418 +0200\n"
        "committer Thomas Zander <Thomas.t.Zander@nokia.com> 1224063799 +0200\n"
        "\n"
        "Don't show compiler warning\n";
    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QCOMPARE(commit.commitTreeIsm(), QString());
    QCOMPARE(commit.tree(), QString("c51f5c18e0aaf97ae9ef38f59470847e5b809d51"));
    QCOMPARE(commit.isValid(), true);
    QCOMPARE(commit.author(), QString("Thomas Zander <Thomas.t.Zander@nokia.com>"));
    QCOMPARE(commit.committer(), QString("Thomas Zander <Thomas.t.Zander@nokia.com>"));
    QCOMPARE(commit.logMessage(), QByteArray("Don't show compiler warning\n"));
}

void TestReadCommit::twoParents()
{
    const char *diff =
        "tree d5cd5e398b2ae1142c77a06ca9c06cf319c7c5fe\n"
        "parent 93bfe525d72cfc977a83bee660d88714f5db952d\n"
        "parent 809d2f8d66d83a83a9fe5ded0f9218e936637b86\n"
        "author foo <foo@bar.com> 1224085203 +0200\n"
        "committer foo <foo@bar.com> 1224085203 +0200\n"
        "\n"
        "Merge commit 'qt-4.5/qt/4.5' into qt/4.5\n";
    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QCOMPARE(commit.commitTreeIsm(), QString());
    QCOMPARE(commit.tree(), QString("d5cd5e398b2ae1142c77a06ca9c06cf319c7c5fe"));
    QCOMPARE(commit.isValid(), true);
    QCOMPARE(commit.previousCommitsCount(), 2);
    QCOMPARE(commit.author(), QString("foo <foo@bar.com>"));
    QCOMPARE(commit.committer(), QString("foo <foo@bar.com>"));
    QCOMPARE(commit.logMessage(), QByteArray("Merge commit 'qt-4.5/qt/4.5' into qt/4.5\n"));
}

void TestReadCommit::justFiles()
{
    // command:  git diff-index b65b6f0254bf6766f0ac09ee888865df520d9d73
    const char *diff =
        ":100644 000000 cee7da7f5af8b919aed2651b0ed2e2b3d4f38e42 0000000000000000000000000000000000000000 D	AbstractCommand.cpp\n"
        ":000000 100644 0000000000000000000000000000000000000000 9457d8fc837268e3b4d4e4070640aa5a878cc197 A	GitRunner.2\n"
        ":100644 100644 c7eb8fcb9e61e97302cf4bd3fe94f0462563dc60 aa287dee7a0b0e5a12b59bed96aaabff16239f7d M	hunks/File.cpp\n";
    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QCOMPARE(commit.commitTreeIsm(), QString());
    QCOMPARE(commit.tree(), QString());
    QCOMPARE(commit.author(), QString());
    QCOMPARE(commit.committer(), QString());
    QCOMPARE(commit.logMessage(), QByteArray());
    QCOMPARE(commit.isValid(), false);
    ChangeSet cs = commit.changeSet();
    QCOMPARE(cs.count(), 3);
    File file1 = cs.file(0);
    QVERIFY(file1.fileName().isEmpty());
    QCOMPARE(file1.oldFileName(), QByteArray("AbstractCommand.cpp"));
    QCOMPARE(file1.oldSha1(), QString("cee7da7f5af8b919aed2651b0ed2e2b3d4f38e42"));
    QCOMPARE(file1.sha1(), QString("0000000000000000000000000000000000000000"));
    File file2 = cs.file(1);
    QVERIFY(file2.oldFileName().isEmpty());
    QCOMPARE(file2.fileName(), QByteArray("GitRunner.2"));
    QCOMPARE(file2.oldSha1(), QString("0000000000000000000000000000000000000000"));
    QCOMPARE(file2.sha1(), QString("9457d8fc837268e3b4d4e4070640aa5a878cc197"));
    File file3 = cs.file(2);
    QVERIFY(file3.fileName() == file3.oldFileName());
    QCOMPARE(file3.oldFileName(), QByteArray("hunks/File.cpp"));
    QCOMPARE(file3.oldSha1(), QString("c7eb8fcb9e61e97302cf4bd3fe94f0462563dc60"));
    QCOMPARE(file3.sha1(), QString("aa287dee7a0b0e5a12b59bed96aaabff16239f7d"));
}

void TestReadCommit::multipleCommits()
{
    const char *diff =
        "commit 09d7e027099f04b891bdccac86ced9544c2c4ddf\n"
        "tree bc7abb956f9bb42fa99433996937fd6424ef0198\n"
        "parent 9dad841e930e5d8c60d1848ce534395ffa8f5c4b\n"
        "author Thomas Zander <Thomas.Zander@nokia.com> 1224076590 +0200\n"
        "committer Thomas Zander <Thomas.Zander@nokia.com> 1224078885 +0200\n"
        "\n"
        "    Use full sha1s for the file diffs to avoid duplicates\n"
        "\n"
        ":100644 100644 8139400... 02cef75... M	commands/Changes.cpp\n"
        ":100644 100644 bf853f1... a1c7329... M	patches/Commit.cpp\n"
        "\n"
        "commit 9dad841e930e5d8c60d1848ce534395ffa8f5c4b\n"
        "tree bbca43c116b823734c10319ac0f3c058e3ce864e\n"
        "parent 6695aac24aa539e4819b28a24179bc1a5bd95d06\n"
        "author Thomas Zander <Thomas.Zander@nokia.com> 1224074466 +0200\n"
        "committer Thomas Zander <Thomas.Zander@nokia.com> 1224078885 +0200\n"
        "\n"
        "    don't print blank when there are no files to list\n"
        "\n"
        ":100644 100644 ca442de... 8139400... M	commands/Changes.cpp\n";

    QByteArray bytes(diff);
    QBuffer buffer(&bytes, 0);
    buffer.open(QBuffer::ReadOnly);

    Commit commit = Commit::createFromStream(&buffer);
    QVERIFY(commit.isValid());
    QCOMPARE(commit.commitTreeIsm(), QString("09d7e027099f04b891bdccac86ced9544c2c4ddf"));
    QCOMPARE(commit.tree(), QString("bc7abb956f9bb42fa99433996937fd6424ef0198"));
    QCOMPARE(commit.author(), QString("Thomas Zander <Thomas.Zander@nokia.com>"));
    QCOMPARE(commit.committer(), QString("Thomas Zander <Thomas.Zander@nokia.com>"));
    Commit commit2 = Commit::createFromStream(&buffer);
    QVERIFY(commit2.isValid());
    QCOMPARE(commit2.commitTreeIsm(), QString("9dad841e930e5d8c60d1848ce534395ffa8f5c4b"));
    QCOMPARE(commit2.tree(), QString("bbca43c116b823734c10319ac0f3c058e3ce864e"));
    QCOMPARE(commit2.author(), QString("Thomas Zander <Thomas.Zander@nokia.com>"));
    QCOMPARE(commit2.committer(), QString("Thomas Zander <Thomas.Zander@nokia.com>"));
    Commit commit3 = Commit::createFromStream(&buffer);
    QVERIFY(! commit3.isValid());
}

QTEST_MAIN(TestReadCommit)
