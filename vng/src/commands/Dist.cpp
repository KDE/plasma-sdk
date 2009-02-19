/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 * Copyright (C) 2002-2004 David Roundy
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

#include "Dist.h"
#include "../CommandLineParser.h"
#include "../Logger.h"
#include "../GitRunner.h"
#include "../hunks/ChangeSet.h"

#include <QFile>
#include <QDir>
#include <QDebug>

static const CommandLineOption options[] = {
    {"-d, --dist-name DISTNAME", "name of version"},
    {"-j, --bzip2", "Create a tar-file with bzip2 compression"},
    CommandLineLastOption
};

Dist::Dist()
    : AbstractCommand("dist")
{
    CommandLineParser::addOptionDefinitions(options);
}

AbstractCommand::ReturnCodes Dist::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot();

    CommandLineParser *args = CommandLineParser::instance();
    QString name;
    if (args->contains("dist-name")) {
        name = args->optionArgument("dist-name");
    } else {
        QDir current = QDir::current();
        name = current.dirName();
    }

    if (dryRun()) // not much use this command then...
        return Ok;

    // deletes our temp dir on exit of scope
    struct Finalizer {
        Finalizer(const QString &dirName) : tmp(dirName)
        {
            m_current = QDir::current();
        }
        ~Finalizer()
        {
            QDir::setCurrent(m_current.absolutePath());
            rm(QFileInfo(m_current, tmp));
        }

        void rm(const QFileInfo &fi) {
            if (fi.isDir()) {
                QDir dir(fi.filePath());
                foreach (QFileInfo fileInfo, dir.entryInfoList(QDir::Hidden | QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
                    rm(fileInfo);
            }
            else
                QFile(fi.filePath()).remove();
            m_current.rmpath(fi.filePath());
        }

        QString tmp;
        QDir m_current;
    };

    QString tmpDirName;
    int i = 0;
    do {
        tmpDirName = QString("vng-tmp-%1").arg(i++);
    } while (QFile(tmpDirName).exists());

    if (! QDir::current().mkdir(tmpDirName) || ! QDir(tmpDirName).mkdir(name)) {
        Logger::error() << "vng-failed; could not make a temporary dir\n";
    }
    Finalizer finalizer(tmpDirName);

    QString basedir = tmpDirName + QDir::separator() + name + QDir::separator();
    QList<File> filesToCheckout;
    QList<File> allFiles = Commit::allFiles();
    ChangeSet changeSet;
    changeSet.fillFromCurrentChanges();
    for (int i=0; i < changeSet.count(); ++i) {
        File file = changeSet.file(i);
        foreach (File other, allFiles) {
            if (other.fileName() == file.fileName() || other.fileName() == file.oldFileName()) {
                filesToCheckout << other;
                allFiles.removeAll(other);
                break;
            }
        }
    }

    QByteArray path (basedir.toLocal8Bit());
    foreach (File file, allFiles) {
        QByteArray dest = path;
        QByteArray fileName = file.fileName();
        dest.append(fileName);
#if defined (Q_OS_UNIX)
        int rc = link(fileName.constData(), dest.constData());
        if (rc == -1) { // this probably means the dirs should be created first.
            int separator = dest.lastIndexOf(QDir::separator());
            Q_ASSERT(separator > 0);
            QDir::current().mkpath(QString::fromLocal8Bit(dest.constData(), separator));
            rc = link(fileName.constData(), dest.constData());
        }
        Q_ASSERT(rc == 0);
#else
        Q_ASSERT(0); // remove when this is actually implemented
        // Windows doesn't allow us to hardlink, so we are forced to copy the whole file.
        // TODO copy file.
        //bool success = Vng::copyFile(etc
#endif
    }

    foreach( File file, filesToCheckout) {
        // checkout (changed) files to the tmp dir
        QFile output(basedir + QString::fromLocal8Bit(file.fileName()));
        Q_ASSERT(! output.exists());
        QProcess git;
        QStringList arguments;
        arguments << "cat-file" << "blob" << file.sha1();
        GitRunner runner(git, arguments);
        ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
        if (rc == Ok) {
            bool success = Vng::copyFile(git, output);
            git.waitForFinished();
            output.close();
            if (!success)
                return WriteError;
        }
        output.setPermissions(file.permissions());
    }

    const bool bzip2 = args->contains("bzip2");
    QString outFileName;
    if (bzip2)
        outFileName = name + ".tar.bz2";
    else
        outFileName = name + ".tgz";
    Logger::debug() << "will write out " << outFileName << endl;

    // chdir
    QDir::setCurrent(tmpDirName);
    QProcess tar;
    QStringList arguments;
    arguments.append(QString("c") + (bzip2 ? 'j' : 'z') + "f");
    arguments << "../"+outFileName << name;
    tar.start("tar", arguments , 0);
    tar.waitForFinished(-1);
    if (tar.exitCode() != 0) {
        Logger::error() << "vng-failed:  The tar subprocess reported errorcode "
            << QString::number(tar.exitCode()) << endl;
        return OtherVngError;
    }
    Logger::standardOut() << "Created dist as " << outFileName << endl;

    return Ok;
}

QString Dist::argumentDescription() const
{
    return QString();
}

QString Dist::commandDescription() const
{
    return "Dist is a handy tool for implementing a \"make dist\" target in your\n"
    "makefile.  It creates a tarball of the recorded edition of your tree.\n";
}
