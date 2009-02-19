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

#ifndef FILE_H
#define FILE_H

#include "hunk.h"
#include <QList>
#include <QString>
#include <QFile>

class QTextStream;
class Configuration;
class QFileInfo;

/**
 * This class represents a single file in ChangeSet.
 */
class File
{
public:
    File();
    File(const File &other);
    ~File();

    /// return if the file is (still) present on the filesystem.
    bool isValid() const;

    void setFileName(const QByteArray &filename);
    void setOldFileName(const QByteArray &filename);
    QByteArray fileName() const;
    QByteArray oldFileName() const;

    void addHunk(const Hunk &hunk);
    QList<Hunk> hunks() const;

    void setProtection(const QString &string);
    void setOldProtection(const QString &string);
    QString protection() const;
    QString oldProtection() const;
    QFile::Permissions permissions() const;

    void setOldSha1(const QString &string);
    void setSha1(const QString &string);
    QString oldSha1() const;
    QString sha1() const;

    /// return hunk count
    int count() const;
    /// return the accumulated number of added lines in all the hunks
    int linesAdded() const;
    /// return the accumulated number of removed lines in all the hunks
    int linesRemoved() const;

    /// return true if this file has registered any changes.
    bool hasChanged() const;
    void setRenameAcceptance(Vng::Acceptance accepted);
    void setProtectionAcceptance(Vng::Acceptance accepted);
    void setBinaryChangeAcceptance(Vng::Acceptance accepted);
    Vng::Acceptance renameAcceptance() const;
    Vng::Acceptance protectionAcceptance() const;
    Vng::Acceptance binaryChangeAcceptance() const;
    /**
     * This method will iterate over all the hunks and subhunks to return the combined acceptance of the changes for this file.
     */
    Vng::Acceptance changesAcceptance();

    void setBinary(bool binary);
    bool isBinary() const;

    void outputWhatsChanged(QTextStream &out, Configuration &config, bool printSummary, bool unified);

    /// use the sha1s set to fetch the hunks.
    void fetchHunks(bool againstHead);
    /// remove the diff(s) stored in memory for this file, diff related method calls will no longer work after this.
    void cleanHunksData();

    File &operator=(const File &other);
    bool operator==(const File &other) const;
    bool operator!=(const File &other) const;

    /**
     * Calling this will check with the repo database if the path is registered in the repo.
     * @returns true if the current repo and current HEAD know this commit, false otherwise.
     */
    static bool fileKnownToGit(const QFileInfo &path);
    /// overloaded method provided for your convenience.
    static bool fileKnownToGit(const QString &path);

private:
    class Private;
    Private * d;
};

#endif
