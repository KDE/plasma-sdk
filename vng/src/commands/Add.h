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

#ifndef ADD_H
#define ADD_H

#include "AbstractCommand.h"
#include "ExcludeMatcher.h"

class Add : public AbstractCommand
{
public:
    Add();

protected:
    virtual QString argumentDescription() const;
    virtual ReturnCodes run();
    virtual QString commandDescription() const;

private:
    /// schedule for addition.
    ReturnCodes addFile(const QFileInfo &path, bool warn);
    /// actually do the add to git.
    ReturnCodes flushAdds();
    void recurse(const QDir &dir);

    ExcludeMatcher m_excludeMatcher;
    QStringList m_filesToAdd;
    int m_argumentLength;
    bool m_fetchedTree;
    QStringList m_fileTree;
};

#endif
