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
#ifndef RECORD_H
#define RECORD_H

#include "AbstractCommand.h"

class ChangeSet;

/// The command to record a set of changes into a patch.
class Record : public AbstractCommand
{
public:
    enum UsageMode {
        Unset,                  ///< Use the command line options for usage mode
        InteractiveSelection,   ///< give the user a prompt to select which hunks to record
        AllChanges,             ///< Add all changes to the patch
        Index                   ///< Use the git index without change
    };

    Record();

    /**
     * Set the message that will be placed in the recorded patch.
     */
    void setPatchName(const QByteArray &patchName);
    /// returns the message
    QByteArray patchName() const;

    /**
     * Set the way that the patches to be recorded will be decided based on the usage mode.
     */
    void setUsageMode(UsageMode mode);
    /// retuns the sha1 of the recorded patch after run() has successfully concluded.
    QString sha1() const;

    // is set to true, force the editor to be started, if false use the command line options
    void setEditComment(bool on) { m_editComment = on; }

    void setRecordAll(bool all) { m_all = all; }
    bool recordAll() const { return m_all; }

    void setAuthor(const QString &author) { m_author = author; }
    QString author() const { return m_author; }

    // actually records
    ReturnCodes record();

protected:
    // this method just relays to record();
    virtual ReturnCodes run();
    virtual QString argumentDescription() const;
    virtual QString commandDescription() const;

private:
    ReturnCodes addFilesPerAcceptance(const ChangeSet &changeSet, bool allChanges);

    UsageMode m_mode;
    QByteArray m_patchName;
    QString m_sha1;
    bool m_editComment;
    bool m_all;
    QString m_author;
};

#endif
