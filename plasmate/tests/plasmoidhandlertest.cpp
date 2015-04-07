/*

Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>

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

#include "plasmoidhandlertest.h"
#include "../packagehandler/plasmoidhandler.h"

#include <QDir>
#include <QTest>
#include <QString>
#include <QStandardPaths>

PlasmoidHandlerTest::PlasmoidHandlerTest(QObject *parent)
        : QObject(parent),
          m_packageHandler(new PlasmoidHandler())
{
}

PlasmoidHandlerTest::~PlasmoidHandlerTest()
{
}

void PlasmoidHandlerTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);
    connect(m_packageHandler, &PackageHandler::error, [=](const QString &error) {
        QFAIL(error.toLocal8Bit().data());
    });
}

void PlasmoidHandlerTest::createNewPlasmoid()
{
    QString newPlasmoidLocation = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    QDir templatesDir(newPlasmoidLocation);
    templatesDir.mkpath(QStringLiteral("templates"));
    templatesDir.cd(QStringLiteral("templates"));

    if (!newPlasmoidLocation.endsWith(QLatin1Char('/'))) {
        newPlasmoidLocation += QLatin1Char('/') + QStringLiteral("org.kde.packagehandlertest.newPlasmoid");
    }

    const QString mainPlasmoidFile = QFINDTESTDATA("testdata/mainPlasmoid.qml");
    Q_ASSERT(!mainPlasmoidFile.isEmpty());

    QFile f(mainPlasmoidFile);
    f.copy(templatesDir.path() + QLatin1Char('/') + QStringLiteral("mainPlasmoid.qml"));

    m_packageHandler->setPackagePath(newPlasmoidLocation);
    m_packageHandler->createPackage(QStringLiteral("author"), QStringLiteral("email"), QStringLiteral("Plasma/Applet"), QStringLiteral("newPlasmoid"));

    QDir dir(newPlasmoidLocation);
    Q_ASSERT(dir.exists());
    Q_ASSERT(dir.exists(QStringLiteral("metadata.desktop")));
    Q_ASSERT(dir.exists(QStringLiteral("contents")));
    Q_ASSERT(dir.cd(QStringLiteral("contents")));
    Q_ASSERT(dir.exists(QStringLiteral("ui")));
    Q_ASSERT(dir.cd(QStringLiteral("ui")));
    Q_ASSERT(dir.exists(QStringLiteral("main.qml")));
}

void PlasmoidHandlerTest::loadPlasmoidTestData()
{
    const QString packagePath = QFINDTESTDATA("testdata/org.kde.tests.packagehandlertest");
    Q_ASSERT(!packagePath.isEmpty());

    m_packageHandler->setPackagePath(packagePath);
}

void PlasmoidHandlerTest::checkDescriptions()
{
    Q_ASSERT(!m_packageHandler->description(QStringLiteral("ui")).isEmpty());
    Q_ASSERT(!m_packageHandler->description(QStringLiteral("main.qml")).isEmpty());
    // invalid
    Q_ASSERT(m_packageHandler->description(QStringLiteral("plasmoid")).isEmpty());
}

void PlasmoidHandlerTest::cleanupTestCase()
{
    QDir testdata(QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0));
    testdata.removeRecursively();
}

QTEST_GUILESS_MAIN(PlasmoidHandlerTest)

