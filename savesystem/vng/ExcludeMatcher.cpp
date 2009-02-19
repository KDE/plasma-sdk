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

#include "ExcludeMatcher.h"
#include "GitRunner.h"
#include "Logger.h"

#include <QDebug>

ExcludeMatcher::ExcludeMatcher(const Configuration &configuration)
    : m_configuration(&configuration)
{
}

bool ExcludeMatcher::isExcluded(const QString &entry)
{
    Q_ASSERT(QDir::current() == m_configuration->repository());
    Q_ASSERT(! entry.startsWith('/')); // assert relative dir
    QString relativePath = "./"+ entry;
    if (m_baseMatches.isEmpty()) {
        // TODO call git config --get core.excludesfile
        QString path = m_configuration->repositoryMetaDir().path() + "/info/exclude";
        readExcludes(path);
        m_baseMatches << m_cachedMatches[path];
    }

    // split entry into separate dirs.
    QStringList pathSegments = entry.split(QDir::separator(), QString::SkipEmptyParts);
    QString fileName = pathSegments.takeLast(); // remove last entry (the actual filename);

    if (isExcluded(m_baseMatches, relativePath, fileName))
        return true;

    pathSegments.insert(0, ".");
    QString path;
    foreach(QString segment, pathSegments) {
        path = path + segment + QDir::separator();
        QString ignoreFile = path + ".gitignore";
        readExcludes(ignoreFile);
        if (isExcluded(m_cachedMatches[ignoreFile], relativePath, fileName))
            return true;
    }
    return false;
}

void ExcludeMatcher::readExcludes(const QString &fileName)
{
    if (m_cachedMatches.contains(fileName))
        return;
    QList<IgnoreLine> items;
    QFile ignoreFile(fileName);
    if (ignoreFile.exists()) {
        if (ignoreFile.open(QFile::ReadOnly)) {
            QTextStream file(&ignoreFile);
            int line=0;
            while(true) {
                ++line;
                QString line = file.readLine();
                if (line.isNull())
                    break;
                if (line.isEmpty() || line[0] == '#')
                    continue;
                IgnoreLine ignoreLine;
                ignoreLine.inverted = line[0] == '!';
                if (ignoreLine.inverted)
                    line = line.mid(1);
                ignoreLine.matchOnFileOnly = !line.contains('/');
                if (! ignoreLine.matchOnFileOnly && !line.startsWith('*'))
                    line = "*" + line;
                QRegExp re(line, Qt::CaseInsensitive, QRegExp::Wildcard);
                ignoreLine.regExp = re;
                if (re.isValid())
                    items << ignoreLine;
                else
                    Logger::debug() << "invalid entry in " << ignoreFile.fileName() << " line " << line;
            }
            ignoreFile.close();
        }
    }
    m_cachedMatches.insert(fileName, items);
}

bool ExcludeMatcher::isExcluded(const QList<IgnoreLine> &ignores, const QString &path, const QString &fileName)
{
    //qDebug() << "isExcluded" << path << fileName;
    foreach (IgnoreLine il, ignores) {
        bool matches;
        if (il.matchOnFileOnly)
            matches = il.regExp.exactMatch(fileName);
        else
            matches = il.regExp.exactMatch(path);
        //qDebug() << "trying on " << il.regExp.pattern() << "fileOnly:" << il.matchOnFileOnly << "inverted:" << il.inverted << (matches?" *** Match!":"");
        if (matches != il.inverted)
            return true;
    }
    return false;
}
