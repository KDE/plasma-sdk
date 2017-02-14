/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the
    Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "plasmateextention.h"

#include <QStringList>
#include <KLocalizedString>

#include <QApplication>

PlasmateExtension::PlasmateExtension()
    : ShellExtension()
{
}

void PlasmateExtension::init()
{
    s_instance = new PlasmateExtension();
}

QString PlasmateExtension::xmlFile()
{
    return "plasmateui.rc";
}

#if KDEVPLATFORM_VERSION >= QT_VERSION_CHECK(5, 1, 40)
QString PlasmateExtension::executableFilePath()
#else
QString PlasmateExtension::binaryPath()
#endif
{
    return QApplication::applicationDirPath() + "/plasmate";
}

KDevelop::AreaParams PlasmateExtension::defaultArea()
{
    KDevelop::AreaParams params = {"code", i18n("Code")};
    return params;
}

QString PlasmateExtension::projectFileExtension()
{
    return "plasmate";
}

QString PlasmateExtension::projectFileDescription()
{
    return i18n("Plasmate Project Files");
}

QStringList PlasmateExtension::defaultPlugins()
{
    return QStringList();
}

