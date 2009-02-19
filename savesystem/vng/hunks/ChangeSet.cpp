/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ChangeSet.h"
#include "../GitRunner.h"
#include "../Logger.h" // for debugging level only
#include "../AbstractCommand.h"
#include "../Vng.h"

#include <QProcess>
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

class HunksFetcher : public QThread
{
public:
    HunksFetcher(const QList<File> &files, ChangeSet &changeSet, bool changeSetOnIndex)
        : m_files(files),
            m_changeSet(changeSet),
            m_changeSetOnIndex(changeSetOnIndex),
            m_interrupted(false)
    {
    }

    void run()
    {
        setPriority(QThread::LowPriority);
        foreach(File file, m_files) {
            if (m_interrupted)
                break;
            m_changeSet.lockFile(file);

            file.fetchHunks(m_changeSetOnIndex);

            if (!file.isBinary() && file.count() == 0 && !file.hasChanged()) { // No change in file at all.
                Logger::debug() << "file: `" << file.oldFileName() << "' => `" << file.fileName() << "'\n";
                Logger::debug() << "  +- Unchanged file, skipping\n";
                m_changeSet.removeFile(file);
                QProcess git;
                QStringList arguments;
                arguments << "update-index" << "-q" <<  "--refresh" << file.fileName();
                GitRunner runner(git, arguments);
                runner.start(GitRunner::WaitUntilFinished);
                continue;
            }
            if (file.fileName().isEmpty() || file.oldFileName().isEmpty())
                file.setProtectionAcceptance(Vng::Accepted);

            int i=0;
            if (Logger::verbosity() >= Logger::Debug) {
                Logger::debug() << "file: `" << file.oldFileName() << "' => `" << file.fileName() << "'\n";
                if (file.isBinary())
                    Logger::debug() << "  +-  is a binary file" << endl;
                Logger::debug() << "  +- " << file.oldProtection() << " => " << file.protection() << endl;
                foreach(Hunk h, file.hunks()) {
                    Logger::debug() << "  +-(" << i++ << ") @" << h.lineNumber() << "; " << h.patch().size() << " bytes\n";
                    for(int i = 0; i < h.subHunkCount(); i++) {
                        Logger::debug() << "       " << i <<"/"<< h.subHunkCount() <<"; "<< h.subHunk(i).size() <<" bytes\n";
                    }
                }
            }
        }
        m_changeSet.lockFile(File());
        m_changeSet.allHunksFetched();
    }

    void interrupt()
    {
        m_interrupted = true;
    }

private:
    QList<File> m_files;
    ChangeSet &m_changeSet;
    bool m_changeSetOnIndex, m_interrupted;
};

class ChangeSet::Private
{
public:
    Private() :
        changeSetOnIndex(false),
        hunksFetcher(0),
        ref(1),
        finishedOneHunk(true)
    {
    }

    ~Private() {
        if (hunksFetcher) {
            hunksFetcher->interrupt();
            hunksFetcher->wait();
            delete hunksFetcher;
        }
    }

    QList<File> files;
    bool changeSetOnIndex; // the changesSet shows the changes of the working dir
    QMutex fileAccessLock;
    QWaitCondition fileAccessWaiter;
    QWaitCondition cursorAccessWaiter;
    QMutex cursorAccessLock;
    File lockedFile;
    HunksFetcher *hunksFetcher;
#if QT_VERSION >= 0x040400
    QAtomicInt ref;
#else
    int ref;
#endif
    bool finishedOneHunk;
};

ChangeSet::ChangeSet()
    : d(new Private())
{
}

ChangeSet::~ChangeSet()
{
#if QT_VERSION >= 0x040400
    if (!d->ref.deref())
#else
    if (--d->ref == 0)
#endif
        delete d;
}

ChangeSet::ChangeSet(const ChangeSet &other)
    : d(other.d)
{
#if QT_VERSION >= 0x040400
    d->ref.ref();
#else
    d->ref++;
#endif
}

AbstractCommand::ReturnCodes ChangeSet::fillFromDiffFile(QIODevice &file)
{
    file.open(QIODevice::ReadOnly);

    foreach (File f, readGitDiff(file))
        addFile(f);

    if (Logger::verbosity() >= Logger::Debug) {
        foreach(File f, d->files) {
            Logger::debug() << "changes in file: " << f.fileName() << endl;
            int i=0;
            foreach(Hunk h, f.hunks()) {
                Logger::debug() << "  +-(" << i++ << ") @" << h.lineNumber() << "; " << h.patch().size() << " bytes\n";
                for(int i = 0; i < h.subHunkCount(); i++) {
                    Logger::debug() << "       " << i <<"/"<< h.subHunkCount() <<"; "<< h.subHunk(i).size() <<" bytes\n";
                }
            }
        }
        Logger::debug().flush();
    }
    return AbstractCommand::Ok;
}

AbstractCommand::ReturnCodes ChangeSet::fillFromCurrentChanges(const QStringList &paths, bool doGenerateHunks )
{
    d->changeSetOnIndex = true;
    QDir refs(".git/refs/heads");
    const bool emptyRepo = refs.count() == 2; // only '.' and '..'
    if (emptyRepo) { // all files added are new, just add all.
        QProcess git;
        QStringList arguments;
        arguments << "ls-files" << "-s";
        GitRunner runner(git, arguments);
        AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
        if (rc)
            return rc;
        char buf[1024];
        while(true) {
            qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
            if (lineLength == -1)
                break;
            File file;
            file.setProtection(QString::fromAscii(buf, 6));
            file.setSha1(QString::fromAscii(buf+7, 40));
            file.setFileName(QByteArray(buf + 50, lineLength - 51));
            d->files.append(file);
        }
        return AbstractCommand::Ok;
    }

    // TODO the below misses the usecase of vng add and then a filesystem rm. Use diff-index --cached to show those.
    QProcess git;
    QStringList arguments;
    arguments << "diff-index" << "-M" << "HEAD";
    if (! paths.isEmpty())
        arguments << "--" << paths;

    // for each line
    // new file:
    // :000000 100644 0000000000000000000000000000000000000000 8c3ae1d344f18b23c3bdde5d26658b70b03c65d9 A      bar
    // rename main.cpp => notmain.cpp
    // :100644 100644 e58cfe72cb7a9559a0090886bea5b0ce00db6b47 477c729e5abbd701eb708df563e7e4f749b50435 R074   main.cpp        notmain.cpp
    // normal change
    // :100644 100644 0bdd73e9ea0ba026f6796799946c4bfc9dd1b0b8 0000000000000000000000000000000000000000 M      test

    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc)
        return rc;
    char buf[1024];
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        if (lineLength > 0 && buf[0] != ':') // not a diff line, ignore.
            continue;

        File file;
        file.setOldProtection(QString::fromAscii(buf+1, 6));
        file.setProtection(QString::fromAscii(buf+8, 6));
        file.setOldSha1(QString::fromAscii(buf+15, 40));
        file.setSha1(QString::fromAscii(buf+56, 40));
        int offset = 98;
        while (buf[offset] != '\t' && offset < lineLength)
            offset++;
        if (buf[97] == 'R') { // rename
            int tab = offset + 1;
            while (buf[tab] != '\t' && tab < lineLength)
                tab++;
            file.setOldFileName(QByteArray(buf + offset + 1, tab - offset - 1));
            file.setFileName(QByteArray(buf + tab + 1, lineLength - tab - 2));
        }
        else if (buf[97] == 'C') { // Copied file
            int tab = offset + 1;
            while (buf[tab] != '\t' && tab < lineLength)
                tab++;
            QByteArray filename(buf + offset + 1, tab - offset - 1);
            file.setOldFileName(filename);
            file.setFileName(filename);
        }
        else {
            QByteArray filename(buf + offset + 1, lineLength - offset - 2);
            if (buf[97] != 'A') // Add
                file.setOldFileName(filename);
            if (buf[97] != 'D') // Delete
                file.setFileName(filename);
        }
        d->files.append(file);
    }
    if (doGenerateHunks)
        generateHunks();

    return AbstractCommand::Ok;
}

void ChangeSet::generateHunks()
{
    Q_ASSERT(d->hunksFetcher == 0);
    d->hunksFetcher = new HunksFetcher(d->files, *this, d->changeSetOnIndex);
    d->finishedOneHunk = false;
    d->hunksFetcher->start();
}

void ChangeSet::lockFile(const File &file)
{
    // qDebug() << "ChangeSet::lockFile";
    QMutexLocker ml(&d->fileAccessLock);
    d->lockedFile = file;
    if (d->files.count() == 0 || d->files.at(0) != file) { // as soon as we have done a file, we can start the interaction
        // qDebug() << "  unlock cursorAccessWaiter";
        d->cursorAccessLock.lock();
        d->finishedOneHunk = true;
        d->cursorAccessWaiter.wakeAll();
        d->cursorAccessLock.unlock();
    }

    d->fileAccessWaiter.wakeAll();
    // qDebug() << "~ChangeSet::lockFile";
}

void ChangeSet::removeFile(const File &file)
{
    QMutexLocker ml(&d->fileAccessLock);
    // TODO move the cursor if this file is the current file.
    d->files.removeAll(file);
    d->fileAccessWaiter.wakeAll();
}

void ChangeSet::allHunksFetched()
{
    // qDebug() << "ChangeSet::allHunksFetched";
    QMutexLocker ml(&d->fileAccessLock);
    d->lockedFile = File();
    d->fileAccessWaiter.wakeAll();
    // qDebug() << "~ChangeSet::allHunksFetched";
}

bool ChangeSet::hasAllHunks() const
{
    return d->hunksFetcher == 0 || d->hunksFetcher->isFinished();
}

// static
QList<File> ChangeSet::readGitDiff(QIODevice &git, File *fileToDiff)
{
    QList<File> filesInDiff;

    // parse the output and create objects for each hunk. (note how that can be made multi-threading)
    //  we have to have the filename in the hunk too to allow skipping a whole hunk
    char buf[1024];
    bool inPatch = false;
    // a diff can be multiple files, or just the one fileToDiff.  Lets keep one File object to point to the current file.
    File file;
    Hunk hunk;
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        QString line = QString::fromLocal8Bit(buf, lineLength);

        const bool newfile = line.startsWith("--- /dev/null");
        if (line.length() > 6 && (newfile || line.startsWith("--- a/"))) {
            file.addHunk(hunk);
            hunk = Hunk();
            if (file.isValid())
                filesInDiff << file;
            if (fileToDiff)
                file = File(*fileToDiff);
            else {
                file = File();
                if (!newfile) {
                    QByteArray array(buf + 6, strlen(buf) - 7);
                    file.setOldFileName(array);
                }
            }
            inPatch = false;
        }
        else if (fileToDiff == 0 && line.length() > 6 && line.startsWith("+++ b/")) {
            QByteArray array(buf + 6, strlen(buf) - 7);
            file.setFileName(array);
        }
        else if (line.length() > 5 && line.startsWith("@@ -")) {
            file.addHunk(hunk);
            hunk = Hunk();
            inPatch = true;
        }
        else if (inPatch && line.startsWith("diff --git "))
            inPatch = false;
        else if (line.startsWith("Binary files a/") && line.indexOf(" differ") > 0) { // TODO does git do translations?
            Q_ASSERT(fileToDiff);
            fileToDiff->setBinary(true);
            git.close();
            return filesInDiff;
        }
        if (inPatch) {
            QByteArray array(buf, lineLength);
            hunk.addLine(array);
        }
    }
    file.addHunk(hunk);
    if (fileToDiff == 0 && file.isValid())
        filesInDiff << file;
    git.close();
    return filesInDiff;
}

void ChangeSet::addFile(const File &file)
{
    if (file.isValid())
        d->files << file;
}

int ChangeSet::count() const
{
    return d->files.count();
}

void ChangeSet::writeDiff(QIODevice &file, ChangeSet::Selection selection) const
{
    waitFinishGenerateHunks();
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream diff(&file);
    foreach(File file, d->files) {
        bool fileHeaderWritten = false;
        foreach(Hunk hunk, file.hunks()) {
            if (selection == AllHunks
                || (selection == UserSelection
                  && (hunk.acceptance() == Vng::Accepted || hunk.acceptance() == Vng::MixedAcceptance))
                || (selection == InvertedUserSelection && hunk.acceptance() != Vng::Accepted)) {
                if (!fileHeaderWritten) {
                    diff.writeRawData("--- a/", 6);
                    diff.writeRawData(file.oldFileName().data(), file.oldFileName().size());
                    diff.writeRawData("\n+++ b/", 7);
                    diff.writeRawData(file.fileName().data(), file.fileName().size());
                    diff.writeRawData("\n", 1);
                    fileHeaderWritten = true;
                }
                QByteArray acceptedPatch;
                if (selection == InvertedUserSelection)
                    acceptedPatch =hunk.rejectedPatch();
                else if (selection == AllHunks) {
                    acceptedPatch = hunk.header();
                    acceptedPatch.append(hunk.patch());
                }
                else
                    acceptedPatch = hunk.acceptedPatch();
                diff.writeRawData(acceptedPatch.data(), acceptedPatch.size());
            }
        }
    }
    file.close();
}

bool ChangeSet::hasAcceptedChanges() const
{
    waitFinishGenerateHunks();
    foreach(File file, d->files) {
        if (file.renameAcceptance() == Vng::Accepted && file.fileName() != file.oldFileName())
            return true;
        if (file.protectionAcceptance() == Vng::Accepted && file.protection() != file.oldProtection())
            return true;
        if (file.isBinary() && file.binaryChangeAcceptance() == Vng::Accepted)
            return true;
        foreach(Hunk hunk, file.hunks()) {
            Vng::Acceptance a = hunk.acceptance();
            if (a == Vng::Accepted || a == Vng::MixedAcceptance)
                return true;
        }
    }
    return false;
}

File ChangeSet::file(int index) const
{
    // qDebug() << "ChangeSet::file" << index << d->finishedOneHunk;
    waitForFinishFirstFile();
    QMutexLocker ml2(&d->fileAccessLock);
    while (d->files.count() > index && d->files[index] == d->lockedFile)
// { qDebug() << "  waiting for file to be unlocked";
        d->fileAccessWaiter.wait(&d->fileAccessLock);
// }
    if (d->files.count() <= index)
        return File();

    // qDebug() << "ChangeSet::~file";
    return d->files[index];
}

ChangeSet &ChangeSet::operator=(const ChangeSet &other)
{
#if QT_VERSION >= 0x040400
    other.d->ref.ref();
    if (!d->ref.deref())
#else
    other.d->ref++;
    if (--d->ref == 0)
#endif
        delete d;
    d = other.d;
    return *this;
}

void ChangeSet::waitFinishGenerateHunks() const
{
    if (d->hunksFetcher)
        d->hunksFetcher->wait();
}

void ChangeSet::waitForFinishFirstFile() const
{
    d->cursorAccessLock.lock();
    if (! d->finishedOneHunk)
        d->cursorAccessWaiter.wait(&d->cursorAccessLock);
    d->cursorAccessLock.unlock();
}

