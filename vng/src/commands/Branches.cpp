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

#include "Branches.h"
#include "CommandLineParser.h"
#include "Logger.h"

static const CommandLineOption options[] = {
    // TODO :)
    CommandLineLastOption
};

Branches::Branches()
    : AbstractCommand("branches")
{
    CommandLineParser::addOptionDefinitions(options);
}

AbstractCommand::ReturnCodes Branches::run()
{
    if (! checkInRepository())
        return NotInRepo;

    foreach(Branch branch, m_config.allBranches()) {
        Logger::standardOut() << branch.branchName() << endl;
    }
    return Ok;
}

QString Branches::argumentDescription() const
{
    return QString();
}

QString Branches::commandDescription() const
{
    return QString(); // TODO
}
