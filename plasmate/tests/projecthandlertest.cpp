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
#include "../packagehandler/plasmoidhandler.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTest>
#include <QStandardPaths>

static const QString testPackage(QStringLiteral("testPackage"));
static const QString externalTestPackage(QStringLiteral("external") + testPackage);

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

    QString projectTestPath = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);
    Q_ASSERT(!projectTestPath.isEmpty());

    if (!projectTestPath.endsWith(QLatin1Char('/'))) {
        projectTestPath += QLatin1Char('/');
    }

    const QString tempPath = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0) + QLatin1Char('/');

    m_testPackagePath = projectTestPath + testPackage;
    m_externalTestPackagePath = tempPath + externalTestPackage;
}

void ProjectHandlerTest::createTestProject()
{
    PackageHandler *packageHandler = new PlasmoidHandler(this);

    packageHandler->setPackagePath(m_testPackagePath);
    packageHandler->createPackage(QStringLiteral("author"), QStringLiteral("email"),
                                  QStringLiteral("Plasma/Applet"), testPackage);

    packageHandler->setPackagePath(m_externalTestPackagePath);
    packageHandler->createPackage(QStringLiteral("author"), QStringLiteral("email"),
                                  QStringLiteral("Plasma/Applet"), externalTestPackage);

    m_projectHandler.addProject(m_externalTestPackagePath);
    m_projectHandler.addProject(m_testPackagePath);
}

void ProjectHandlerTest::projectsList()
{
    QStringList list = m_projectHandler.loadProjectsList();

    QCOMPARE(list.count(),  2);

    Q_ASSERT(list.contains(m_testPackagePath));
    Q_ASSERT(list.contains(m_externalTestPackagePath));

    m_projectHandler.removeProject(m_testPackagePath);
    m_projectHandler.removeProjectFromDisk(m_externalTestPackagePath);

    list = m_projectHandler.loadProjectsList();
    QCOMPARE(list.count(), 0);

    Q_ASSERT(!list.contains(m_testPackagePath));
    Q_ASSERT(!list.contains(m_externalTestPackagePath));

    Q_ASSERT(!QDir(m_externalTestPackagePath).exists());
    Q_ASSERT(QDir(m_testPackagePath).exists());
}

void ProjectHandlerTest::cleanupTestCase()
{
    QDir(m_testPackagePath).removeRecursively();
}

QTEST_GUILESS_MAIN(ProjectHandlerTest)

