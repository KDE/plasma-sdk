/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 * Copyright (C) 2002-2005 David Roundy
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
#include "VngCommandLine.h"
#include "CommandLineParser.h"
#include "Logger.h"

// commands
#include "commands/Add.h"
#include "commands/AmendRecord.h"
#include "commands/Branches.h"
#include "commands/Changes.h"
#include "commands/Dist.h"
#include "commands/Initialize.h"
#include "commands/Move.h"
#include "commands/Push.h"
#include "commands/Record.h"
#include "commands/Remove.h"
#include "commands/Revert.h"
#include "commands/UnRecord.h"
#include "commands/UnRevert.h"
#include "commands/WhatsNew.h"

#include <QTextStream>

#define VERSION "vng version 0.27 (>2008-10-10)"

VngCommandLine::VngCommandLine(int argc, char **argv)
    : QCoreApplication(argc, argv)
{
    if (argc > 1)
        m_command = QString(argv[1]);
    if (m_command == "help" && argc > 2) { // 'help foo' -> 'foo -h'
        argv[1] = argv[2];
        argv[2] = new char[3];// this is a memory leak, do we care?
        argv[2][0] = '-';
        argv[2][1] = 'h';
        argv[2][2] = 0;
        m_command = QString(argv[1]);
    }

    CommandLineParser::init(argc, argv);
}

VngCommandLine::~VngCommandLine()
{
    delete CommandLineParser::instance();
}

int VngCommandLine::run()
{
    if (m_command == "--extended-help") {
        printExtendedHelp();
        return 0;
    }
    if (m_command == "--version" || m_command == "-v"
#if defined(Q_OS_WIN)
        || m_command == "/V"
#endif
            ) {
        QTextStream out(stdout);
        out << VERSION << endl;
        return 0;
    }
    if (m_command.isEmpty()) {
        printHelp();
        return 0;
    }

    if (m_command == "--commands") { // for commandline completion.
        Configuration config("");
        QTextStream out(stdout);
        out << "help" << endl;
        out << "initialize" << endl;
        out << "--help" << endl;
        out << "--version" << endl;
        out << "--extended--help" << endl;
        if (QDir(config.repository().absolutePath() + "/.git").exists()) {
            out << "add" << endl;
            out << "amend-record" << endl;
            out << "branches" << endl;
            out << "changes" << endl;
            out << "dist" << endl;
            out << "log" << endl;
            out << "move" << endl;
            out << "mv" << endl;
            out << "push" << endl;
            out << "record" << endl;
            out << "remove" << endl;
            out << "revert" << endl;
            out << "rm" << endl;
            out << "unrecord" << endl;
            out << "unrevert" << endl;
            out << "whatsnew" << endl;
        }
        return 0;
    }

    // autoComplete command
    QStringList commands;
    // keep this list sorted in the sourcecode.    In vim just select the lines and do a :'<,'>!sort
    commands << "add";
    commands << "amend-record";
    commands << "branches";
    commands << "changes";
    commands << "dist";
    commands << "help";
    commands << "initialize";
    commands << "log";
    commands << "move";
    commands << "mv";
    commands << "push";
    commands << "record";
    commands << "remove";
    commands << "revert";
    commands << "rm";
    commands << "unrecord";
    commands << "unrevert";
    commands << "whatsnew";

    QString command;
    foreach(QString s, commands) {
        if (m_command == s) { // perfect match
            command = m_command;
            break;
        }
        if (! command.isEmpty()) {
            if (s.startsWith(m_command)) {
                QTextStream out(stdout);
                out << "vng failed: " << "Ambiguous command..." << endl;
                return -1;
            }
            else
                break; // found best match.
        }
        if (s.startsWith(m_command)) {
            command = s;
            continue;
        }
    }
    if (command.isEmpty()) {
        if (! m_command.isEmpty()) {
            QTextStream out(stdout);
            out << "vng failed: " << "Invalid command `" << m_command << "'\n\n";
        }
        printHelp();
        return 0;
    }
    m_command = command;

    AbstractCommand *ac = 0;
    if (m_command == "whatsnew")
        ac = new WhatsNew();
    else if (m_command == "add")
        ac = new Add();
    else if (m_command == "amend-record")
        ac = new AmendRecord();
    else if (m_command == "branches")
        ac = new Branches();
    else if (m_command == "changes" || m_command == "log")
        ac = new Changes();
    else if (m_command == "initialize")
        ac = new Initialize();
    else if (m_command == "move" || m_command == "mv")
        ac = new Move();
    else if (m_command == "push")
        ac = new Push();
    else if (m_command == "remove" || m_command == "rm")
        ac = new Remove();
    else if (m_command == "revert")
        ac = new Revert();
    else if (m_command == "unrecord")
        ac = new UnRecord();
    else if (m_command == "unrevert")
        ac = new UnRevert();
    else if (m_command == "dist")
        ac = new Dist();
    else if (command == "help") {
        printHelp();
        return 0;
    }
    else if (m_command == "record")
        ac = new Record();

    Q_ASSERT(ac);
    return ac->start();
}

void VngCommandLine::printHelp()
{
    QTextStream out(stdout);
    out << "Usage: vng COMMAND ...\n\n" << VERSION << endl;
    out << "Commands:" << endl;
    out << "  help          Display help for vng or a single commands." << endl;
    out << "Changing and querying the working copy:" << endl;
    Configuration config("");
    out << "  add           Add one or more new files or directories." << endl;
    out << "  remove        Remove one or more files or directories from the repository." << endl;
    out << "  mv            Move/rename one or more files or directories." << endl;
    out << "  revert        Revert to the recorded version (safe the first time only)." << endl;
    out << "  unrevert      Undo the last revert (may fail if changes after the revert)." << endl;
    out << "  whatsnew      Display unrecorded changes in the working copy." << endl;
    out << "Copying changes between the working copy and the repository:" << endl;
    out << "  record        Save changes in the working copy to the repository as a patch." << endl;
    out << "  unrecord      Remove recorded patches without changing the working copy." << endl;
    out << "  amend-record  Replace a patch with a better version before it leaves your repository." << endl;
    out << "Copying patches between repositories with working copy update:" << endl;
    out << "  push          Copy and apply patches from this repository to another one." << endl;
    out << "Querying the repository:" << endl;
    // out << "  diff          Create a diff between two versions of the repository." << endl;
    // out << "  branches      " << endl;
    out << "  dist          Create a distribution tarball." << endl;
    out << "  changes       Gives a changelog-style summary of the repository history." << endl;
    out << "Administrating repositories:" << endl;
    out << "  initialize    Initialize a new source tree as a vng repository." << endl;
    out << endl;
    out << "Use 'vng --extended-help' for more detailed help." << endl;
    out << "Use 'vng COMMAND --help' for help on a single command." << endl;
    out << "Use 'vng --version' to see the vng version number." << endl;
}

void VngCommandLine::printExtendedHelp()
{
    QTextStream out(stdout);
    out << VERSION << endl;
    out << "Usage: vng COMMAND ...\n\n";
    out << "Extended Help:\n\n";
    out << "A vng repository consists of:\n\n";
    out << "  - a set of PATCHES\n";
    out << "  - a WORKING directory\n\n";
    out << "Here is a description of which of these components is altered by each\n";
    out << "command, and how it is used or altered:\n\n";
    out << "  whatsnew      Show the differences between WORKING and the \"recorded\"\n";
    out << "                version, that is, the result of applying all PATCHES in the\n";
    out << "                repository.  This difference, we will call LOCAL CHANGES.\n\n";
    out << "  record        Add a patch to PATCHES representing the LOCAL CHANGES.\n\n";
    out << "  unrecord      Delete a patch from PATCHES, but *do not* alter WORKING.\n\n";
    out << "  revert        Remove LOCAL CHANGES.  Note that this command is interactive,\n";
    out << "                so you can use it to revert only some of these changes.\n\n";
    out << "  unrevert      Undo the last revert operation.\n\n";
//   out << "  unpull        Delete a patch from PATCHES and unapply it from WORKING.\n";
//   out << "                Note that this command works for any patch, not just one that\n";
//   out << "                was previously \"pull\"ed.  If there are no LOCAL CHANGES,\n";
//   out << "                this command is equivalent to \"vng unrecord; vng revert\"\n\n";
//   out << "  rollback      Create the inverse of a particular patch and add it to PATCHES,\n";
//   out << "                but DO NOT apply it to WORKING.  Note that this command is the\n";
//   out << "                only way to wind up with a patch in PATCHES which has not been\n";
//   out << "                applied to WORKING.\n\n";
}


// a C main function
int main(int argc, char **argv)
{
    VngCommandLine vng(argc, argv);
    return vng.run();
}
