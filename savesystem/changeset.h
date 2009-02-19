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

#ifndef CHANGESET_H
#define CHANGESET_H

#include "file.h"
#include "gitrunner.h"

#include <QStringList>

class ChangeSet
{
public:
    enum Selection {
        UserSelection,
        InvertedUserSelection,
        AllHunks
    };
    ChangeSet();
    ChangeSet(const ChangeSet &other);
    ~ChangeSet();

    GitRunner::ReturnCodes fillFromCurrentChanges(const QStringList &paths = QStringList(), bool generateHunks = true);
    GitRunner::ReturnCodes fillFromDiffFile(QIODevice &file);

    void writeDiff(QIODevice &file, Selection selection) const;

    void addFile(const File &file);

    File file(int index) const;

    /// return filecount
    int count() const;

    bool hasAcceptedChanges() const;
    bool hasAllHunks() const;

    void generateHunks();
    void waitForFinishFirstFile() const;
    void waitFinishGenerateHunks() const;

    ChangeSet &operator=(const ChangeSet &other);

    static QList<File> readGitDiff(QIODevice &device, File *file = 0);

protected:
    friend class HunksFetcher;
    void lockFile(const File &file);
    void removeFile(const File &file);
    void allHunksFetched();

private:
    class Private;
    Private * d;
};

#endif
