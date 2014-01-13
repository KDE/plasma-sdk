/*

Copyright 2014 Antonis Tsiapaliokas <kok3rs@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "projecthandlertest.h"
#include "../packagehandler.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTest>
#include <QStandardPaths>

ProjectHandlerTest::ProjectHandlerTest(QObject *parent)
        : QObject(parent)
{
}

ProjectHandlerTest::~ProjectHandlerTest()
{
}

void ProjectHandlerTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);
}

void ProjectHandlerTest::createTestProject()
{
    // Here we create a test package in case it
    // doesn't exist.
    const QString projectTestPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    Q_ASSERT(!projectTestPath.isEmpty());
    PackageHandler packageHandler;

    // If our project package exists, then we are removing it.
    // In order to ensure that it has been created properly
    QDir invalidTestPackageDir(projectTestPath + "/invalidtestpackage");
    if (invalidTestPackageDir.exists()) {
        invalidTestPackageDir.removeRecursively();
    }

    QDir externalInvalidTestPackageDir(QDir::tempPath() + "/invalidtestpackageexternal");
    if (externalInvalidTestPackageDir.exists()) {
        externalInvalidTestPackageDir.removeRecursively();
    }

    packageHandler.setPackagePath(projectTestPath + "/testpackage");
    packageHandler.setPackagePath(invalidTestPackageDir.path());

    // Extrenal project
    packageHandler.setPackagePath(QDir::tempPath() + "/externaltestpackage");
    packageHandler.setPackagePath(externalInvalidTestPackageDir.path());

    // Our external package is valid so we must make it invalid.
    QFile invalidPackageExternalMetadata(externalInvalidTestPackageDir.path() + "/metadata.desktop");
    Q_ASSERT(invalidPackageExternalMetadata.remove());

    // Our package is valid so we must make it invalid.
    QFile invalidPackageMetadata(invalidTestPackageDir.path() + "/metadata.desktop");
    Q_ASSERT(invalidPackageMetadata.remove());

    m_projectHandler.addProject(QDir::tempPath() + "/externaltestpackage");
    m_projectHandler.addProject(QDir::tempPath() + "/newtestpackage");
}

void ProjectHandlerTest::projectsList()
{
    QStringList list = m_projectHandler.loadProjectsList();
    const QString projectTestPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);
    const QString testPackagePath = projectTestPath + "/testpackage";
    const QString externalTestPackagePath = QDir::tempPath() + "/externaltestpackage";
    const QString newTestPackage = QDir::tempPath() + "/newtestpackage";

    Q_ASSERT(list.length() > 0);
    Q_ASSERT(list.count() == 3);
    Q_ASSERT(list.contains(testPackagePath));
    Q_ASSERT(list.contains(externalTestPackagePath));
    Q_ASSERT(list.contains(newTestPackage));

    m_projectHandler.removeProject(QDir::tempPath() + "/externaltestpackage");
    list = m_projectHandler.loadProjectsList();
    Q_ASSERT(list.length() > 0);
    Q_ASSERT(list.count() == 2);
    Q_ASSERT(list.contains(testPackagePath));
}

QTEST_GUILESS_MAIN(ProjectHandlerTest)

