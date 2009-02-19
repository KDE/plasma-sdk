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
#include "WhatsNew.h"
#include "hunks/ChangeSet.h"
#include "Logger.h"
#include "GitRunner.h"
#include "CommandLineParser.h"

#include <QTextStream>
#include <QProcess>

static const CommandLineOption options[] = {
    {"-s, --summary", "summarize changes"},
    {"--no-summary", "don't summarize changes"},
    {"-u, --unified", "output patch in a format similar to diff -u"},
    {"-l, --look-for-adds", "In addition to modifications, look for files that are not boring, and thus are potentially pending addition"},
    {"--dont-look-for-adds", "Don't look for any files or directories that could be added, and don't add them automatically"},
    //{"--boring", "don't skip boring files"},
    CommandLineLastOption
};

WhatsNew::WhatsNew()
    : AbstractCommand("whatsnew"),
    m_excludeMatcher(m_config)
{
    CommandLineParser::addOptionDefinitions(options);
    CommandLineParser::setArgumentDefinition("whatsnew [FILE or DIRECTORY]" );
}

AbstractCommand::ReturnCodes WhatsNew::run()
{
    if (! checkInRepository())
        return NotInRepo;
    moveToRoot( static_cast <RebaseOptions> (CheckFileSystem | PrintError) ); // XXX why is the static cast needed :(

    CommandLineParser *args = CommandLineParser::instance();
    const bool lookForAdds = (m_config.contains("look-for-adds") && !args->contains("dont-look-for-adds-summary"))
        || args->contains("look-for-adds");
    const bool printSummary = lookForAdds
        || (m_config.contains("summary") && !args->contains("no-summary"))
        || args->contains("summary");
    const bool unified = m_config.contains("unified") || args->contains("unified");

    if (shouldUsePager())
        Logger::startPager();
    ChangeSet changeSet;
    changeSet.fillFromCurrentChanges(rebasedArguments());
    changeSet.waitForFinishFirstFile();

    QTextStream &out = Logger::standardOut();
    if (lookForAdds) {
        QProcess git;
        QStringList arguments;
        arguments << "ls-files" << "-o" << "--directory" << "--no-empty-directory" << "--exclude-standard";

        GitRunner runner(git, arguments);
        ReturnCodes rc = runner.start(GitRunner::WaitForStandardOutput);
        if (rc != Ok)
            return rc;

        char buf[1024];
        while(true) {
            qint64 lineLength = Vng::readLine(&git, buf, sizeof(buf));
            if (lineLength == -1)
                break;
            QString line = QString::fromUtf8(buf, lineLength);
            if (!m_excludeMatcher.isExcluded(line))
                out << "a " << line;
        }
    }
    else if (changeSet.count() == 0) {
        out << "No Changes!" << endl;
        return Ok;
    }
    if (!printSummary && !unified) {
        m_config.colorize(out);
        out << "{\n";
    }
    for (int i=0; i < changeSet.count(); ++i ) {
        File file = changeSet.file(i);
        file.outputWhatsChanged(out, m_config, printSummary, unified);
        file.cleanHunksData();
        Logger::flushPager();
    }
    if (!printSummary && !unified) {
        m_config.colorize(out);
        out << "}\n";
        m_config.normalColor(out);
    }

    return AbstractCommand::Ok;
}

QString WhatsNew::argumentDescription() const
{
    return QString("[FILE or DIRECTORY]");
}

QString WhatsNew::commandDescription() const
{
    return "Whatsnew gives you a view of what changes you've made in your working\n"
        "copy that haven't yet been recorded.  The changes are displayed in\n"
        "vng patch format.\n";
}
