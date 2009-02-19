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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QDir>
#include <QHash>
#include <QTextStream>
#include <QMutex>

#include "patches/Branch.h"

class Configuration {
public:
    /// creates a simple configuration object which does not check for section specific settings.
    Configuration();
    /**
     * Creates a Configuration object which is initialized with settings from for the section.
     * If the user has settings in his user-wide-profile for the section passed, these will
     * be read and made available using the contains() and optionArgument() methods.
     * @param section the command-name which this configuration object will link to.
     */
    Configuration(const QString &section);

    QString section() const { return m_section; }

    /**
     * returns true if the option was found in the configuration files.
     * @see optionArgument()
     */
    bool contains(const QString & key) const;
    /**
     * Return the argument passed to an option.
     * @see contains()
     */
    QString optionArgument(const QString &optionName, const QString &defaultValue = QString()) const;

    /// return the base directory of this repository.
    QDir repository() const;
    /// returns the directory where the database and other meta data is stored.
    QDir repositoryMetaDir() const;

    /**
     * Returns true if the current terminal supports colors.
     * You normally want to just call colorize() and related methods.
     * @see colorize(), colorize2(), normalColor()
     */
    bool colorTerm() const;


    /**
     * In case the current terminal is a colorTerm() this method will make the stream use a color.
     * @see normalColor()
     */
    inline void colorize(QTextStream &stream) const {
        if (colorTerm())
            stream << QChar(27) << '[' << 34 << 'm'; // to blue
    }

    /**
     * In case the current terminal is a colorTerm() this method will make the stream use a strong color.
     * @see normalColor()
     */
    inline void colorize2(QTextStream &stream) const {
        if (colorTerm())
            stream << QChar(27) << '[' << 31 << 'm'; // to red
    }

    /**
     * Reset the terminal color to normal.
     * @see colorize()
     */
    inline void normalColor(QTextStream &stream) const {
        if (colorTerm())
            stream << QChar(27) << '[' << 0 << 'm'; // reset
    }

    /// returns true when there is a repository, but it has not had its initial commit yet.
    bool isEmptyRepo() const;
    /// return the branches in the current repo.
    QList<Branch> allBranches();
    /// return the branches in the current repo.
    QList<Branch> branches();
    /// return the branches in the current repo.
    QList<Branch> remoteBranches();

private:
    void readConfig();
    void fetchBranches();

    QDir m_repoDir;
    QDir m_repoMetaDataDir;
    QString m_section;
    bool m_dirty, m_emptyRepo, m_fetchedBranches;
    QHash<QString, QString> m_options;
    QList<Branch> m_localBranches;
    QList<Branch> m_remoteBranches;
};

#endif
