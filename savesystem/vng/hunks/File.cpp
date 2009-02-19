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

#include "File.h"
#include "ChangeSet.h"
#include "../GitRunner.h"
#include "../AbstractCommand.h"

#include <QProcess>
#include <QDebug>

class File::Private
{
public:
    Private()
        : ref(1),
        renameAcceptance(Vng::Undecided),
        protectionAcceptance(Vng::Undecided),
        binChangeAcceptance(Vng::Undecided),
        isBinaryFile(false)
    {
    }

    QString protection, oldProtection;
    QString sha1, oldSha1;
    QByteArray filename;
    QByteArray oldFilename;
    QList<Hunk> hunks;
#if QT_VERSION >= 0x040400
    QAtomicInt ref;
#else
    int ref;
#endif
    Vng::Acceptance renameAcceptance;
    Vng::Acceptance protectionAcceptance;
    Vng::Acceptance binChangeAcceptance;
    bool isBinaryFile;
};

File::File()
    : d(new Private())
{
}

File::File(const File &other)
    : d(other.d)
{
#if QT_VERSION >= 0x040400
    d->ref.ref();
#else
    d->ref++;
#endif
}

File::~File()
{
#if QT_VERSION >= 0x040400
    if (! d->ref.deref())
#else
    if (--d->ref == 0)
#endif
        delete d;
}

void File::addHunk(const Hunk &hunk)
{
    if (! hunk.isEmpty())
        d->hunks << hunk;
}

QList<Hunk> File::hunks() const
{
    return d->hunks;
}

bool File::isValid() const
{
    return !(d->filename.isEmpty() && d->oldFilename.isEmpty());
}

void File::setFileName(const QByteArray &filename)
{
    d->filename = filename;
    d->renameAcceptance = d->oldFilename == d->filename ? Vng::Accepted : Vng::Undecided;
}

QByteArray File::fileName() const
{
    return d->filename;
}

File & File::operator=(const File &other)
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

bool File::operator==(const File &other) const
{
    return other.d == d;
}

bool File::operator!=(const File &other) const
{
    return ! (other.d == d);
}

int File::count() const
{
    return d->hunks.count();
}

void File::setOldFileName(const QByteArray &filename)
{
    d->oldFilename = filename;
    d->renameAcceptance = d->oldFilename == d->filename ? Vng::Accepted : Vng::Undecided;
}

QByteArray File::oldFileName() const
{
    return d->oldFilename;
}

void File::setProtection(const QString &string)
{
    d->protection = string;
    d->protectionAcceptance = string == d->oldProtection ? Vng::Accepted : Vng::Undecided;
}

void File::setOldProtection(const QString &string)
{
    d->oldProtection = string;
    d->protectionAcceptance = string == d->protection ? Vng::Accepted : Vng::Undecided;
}

QString File::protection() const
{
    return d->protection;
}

QString File::oldProtection() const
{
    return d->oldProtection;
}


void File::setOldSha1(const QString &string)
{
    d->oldSha1 = string;
}

void File::setSha1(const QString &string)
{
    d->sha1 = string;
}

QString File::oldSha1() const
{
    return d->oldSha1;
}

QString File::sha1() const
{
    return d->sha1;
}

bool File::hasChanged() const
{
    return count() || d->protection != d->oldProtection ||
        d->filename != d->oldFilename;
}

void File::setRenameAcceptance(Vng::Acceptance accepted)
{
    if (d->filename != d->oldFilename)
        d->renameAcceptance = accepted;
}

void File::setProtectionAcceptance(Vng::Acceptance accepted)
{
    if (d->protection != d->oldProtection)
        d->protectionAcceptance = accepted;
}

Vng::Acceptance File::renameAcceptance() const
{
    return d->renameAcceptance;
}

Vng::Acceptance File::protectionAcceptance() const
{
    return d->protectionAcceptance;
}

void File::setBinaryChangeAcceptance(Vng::Acceptance accepted)
{
    if (d->isBinaryFile)
        d->binChangeAcceptance = accepted;
}

Vng::Acceptance File::binaryChangeAcceptance() const
{
    return d->binChangeAcceptance;
}

QFile::Permissions File::permissions() const
{
    QFile::Permissions perms;
    if (d->protection.length() != 6)
        return perms;
    Q_ASSERT(d->protection.length() == 6);
    const int user = d->protection.mid(3,1).toInt();
    if (user & 1) perms |= QFile::ExeOwner;
    if (user & 2) perms |= QFile::WriteOwner;
    if (user & 4) perms |= QFile::ReadOwner;
    const int group = d->protection.mid(4,1).toInt();
    if (group & 1) perms |= QFile::ExeGroup;
    if (group & 2) perms |= QFile::WriteGroup;
    if (group & 4) perms |= QFile::ReadGroup;
    const int other = d->protection.mid(5,1).toInt();
    if (other & 1) perms |= QFile::ExeOther;
    if (other & 2) perms |= QFile::WriteOther;
    if (other & 4) perms |= QFile::ReadOther;
    return perms;
}

int File::linesAdded() const
{
    int total = 0;
    foreach(Hunk hunk, d->hunks)
        total += hunk.linesAdded();
    return total;
}

int File::linesRemoved() const
{
    int total = 0;
    foreach(Hunk hunk, d->hunks)
        total += hunk.linesRemoved();
    return total;
}

void File::setBinary(bool binary)
{
    d->isBinaryFile = binary;
}

bool File::isBinary() const
{
    return d->isBinaryFile;
}

void File::fetchHunks(bool againstHead)
{
    if (d->hunks.count())
        return;
    if (d->oldSha1 == d->sha1)
        return; // no change.

    class DiffCreator {
      public:
        DiffCreator(File file) : m_file(file) { }
        void wholeFileAsDiff(bool added)
        {
            QProcess git;
            QStringList arguments;
            arguments << "cat-file" << "blob";
            if (added)
                 arguments << m_file.sha1();
            else
                 arguments << m_file.oldSha1();
            GitRunner runner(git, arguments);
            AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
            const char * prefix = (added ? "+" : "-");
            if (rc == AbstractCommand::Ok) {
                char buf[1024];
                Hunk hunk; // its one hunk.
                QByteArray header("@@ -1,0 +1,0 @@", 15);
                hunk.addLine(header);
                while(true) {
                    qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
                    if (lineLength == -1)
                        break;
                    QByteArray array(prefix, 1);
                    array.append(buf);
                    hunk.addLine(array);
                }
                m_file.addHunk(hunk);
            }
        }

      private:
        File m_file;
    };

    if (fileName().isEmpty()) { // deleted file.
        DiffCreator dc(*this);
        dc.wholeFileAsDiff(false);
        return;
    }

    QStringList arguments;
    if (sha1().startsWith("0000000") || oldSha1().startsWith("0000000")) {
        // if this is changed without ever being added to the index, sha1() is 000.
        // if added as a new file oldSha1() is 000
        if (againstHead)
            arguments << "diff-index" << "-p" << "HEAD" << fileName();
        else {
            DiffCreator dc(*this);
            dc.wholeFileAsDiff(true);
            return;
        }
    }
    else
        arguments << "diff" << oldSha1() << sha1();
    QProcess git;
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc)
        return;
    ChangeSet::readGitDiff(git, this);
}

void File::outputWhatsChanged(QTextStream &out, Configuration &config, bool printSummary, bool unified)
{
    const bool deleted = d->filename.isEmpty() && !d->oldFilename.isEmpty();
    const bool added = !deleted && !d->filename.isEmpty() && d->oldFilename.isEmpty();
    const bool renamed = !deleted && !added && d->filename != d->oldFilename;
    if (printSummary) {
        if (deleted)
            out <<"D ";
        else if (added)
            out <<"A ";
        else if (renamed)
            out << "R ";
        else
            out << "M ";

        if (deleted || renamed)
            out << d->oldFilename;
        else
            out << d->filename;
        if (renamed)
            out << " => " << d->filename;

        int removed = linesRemoved();
        int added = linesAdded();
        if (removed)
            out << " -" << QString::number(removed);
        if (added)
            out << " +" << QString::number(added);
        out << endl;
        return;
    }

    if (deleted) { // file deleted.
        if (!unified) {
            config.colorize(out);
            out <<"rmfile ";
            config.normalColor(out);
            out << d->oldFilename << endl;
        }
    }
    else if (added) { // file added.
        if (!unified) {
            config.colorize(out);
            out <<"addfile ";
            config.normalColor(out);
            out << d->filename << endl;
        }
    }
    else {
        if(renamed) { // rename
            if (!unified) config.colorize(out);
            out <<"move ";
            if (!unified) config.normalColor(out);
            out << "`" << d->oldFilename << "' `" << d->filename << "'" << endl;
        }
        if (oldProtection() != protection()) {
            if (!unified) config.colorize(out);
            out <<"mode change ";
            if (!unified) config.normalColor(out);
            out << d->filename << "  " << oldProtection() << " => " << protection() << endl;
        }
    }
    if (isBinary()) { // will not have any hunks
        if (!unified) config.colorize(out);
        out << "binary modification ";
        if (!unified) config.normalColor(out);
        out << d->filename << endl;
        return;
    }
    if (unified) {
        out << "--- ";
        if (d->oldFilename.isEmpty())
           out  << "/dev/null\n";
        else
           out  << d->oldFilename << endl;
        out << "+++ ";
        if (d->filename.isEmpty())
           out  << "/dev/null\n";
        else
           out  << d->filename << endl;
    }
    foreach(Hunk hunk, hunks()) {
        if (unified) {
            out << QString(hunk.header());
            out << QString(hunk.patch());
            continue;
        }
        for (int i = 0; i < hunk.subHunkCount(); i++) {
            config.colorize(out);
            out <<"hunk ";
            QByteArray patch = hunk.subHunk(i);
            config.normalColor(out);
            out << d->filename <<" "<< QString::number(hunk.lineNumber(i)) << endl;
            if (patch.contains((char) 0)) { // binary
                config.colorize(out);
                out << "binary data\n";
                config.normalColor(out);
            }
            else {
                QString string = QString::fromLocal8Bit(patch);
                const bool trailingLinefeed = string.length() > 1 && string[string.length()-1].unicode() == '\n';
                if (config.colorTerm() && trailingLinefeed)
                    string = string.left(string.length()-1);
                out << string;
                if (config.colorTerm() && (!trailingLinefeed || string[string.length()-1].isSpace())) {
                    config.colorize2(out);
                    out << "$\n";
                    config.normalColor(out);
                }
                else
                    out << "\n";
            }
        }
    }
}

void File::cleanHunksData()
{
    d->hunks.clear();
}

Vng::Acceptance File::changesAcceptance()
{
    class AccaptenceManager {
      public:
        AccaptenceManager() : m_init(false), m_result(Vng::Accepted) { }
        void add(Vng::Acceptance a) {
            if (!m_init) {
                m_result = a;
                m_init = true;
                return;
            }
            if (m_result != a)
                m_result = Vng::MixedAcceptance;
            // do I need something special for undecided?
        }
        Vng::Acceptance result() { return m_result; }
      private:
        bool m_init;
        Vng::Acceptance m_result;
    };
    AccaptenceManager am;

    if (d->protection != d->oldProtection)
        am.add(d->protectionAcceptance);
    if (d->filename != d->oldFilename)
        am.add(d->renameAcceptance);
    if (d->isBinaryFile)
        am.add(d->binChangeAcceptance);
    foreach (Hunk hunk, d->hunks) {
        am.add(hunk.acceptance());
        if (am.result() == Vng::MixedAcceptance)
            break;
    }
    return am.result();
}

// static
bool File::fileKnownToGit(const QString &path)
{
    return fileKnownToGit(QFileInfo(path));
}

// static
bool File::fileKnownToGit(const QFileInfo &path)
{
    QStringList arguments;
    arguments << "ls-files" << path.filePath();
    QProcess git;
    GitRunner runner(git, arguments);
    AbstractCommand::ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
    if (rc != AbstractCommand::Ok)
        return false;
    bool unknownFile = true;
    char buf[1024];
    while(true) {
        qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
        if (lineLength == -1)
            break;
        unknownFile = false; // lets assume here that the script just doesn't print anything if its not added yet.
        break;
    }
    git.waitForFinished();
    return !unknownFile;
}

