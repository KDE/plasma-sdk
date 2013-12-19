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

#include "packagehandlertest.h"

#include <QDir>
#include <QTest>
#include <QString>

PackageHandlerTest::PackageHandlerTest(QObject *parent)
        : QObject(parent)
{
}

PackageHandlerTest::~PackageHandlerTest()
{
}

void PackageHandlerTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);

    connect(&m_packageHandler, &PackageHandler::error, [=](const QString &error) {
        QFAIL(error.toLocal8Bit().data());
    });

    m_packageHandler.setPackageType(QStringLiteral("Plasma/Applet"));
}

void PackageHandlerTest::checkDirectories()
{
    // FIXME
    m_packageHandler.setPackagePath(QStringLiteral("org.kde.tests.packagehandlertest"));

    QDir dir(m_packageHandler.projectPath());
    Q_ASSERT(dir.exists());
    //FIXME make it more generic?
    Q_ASSERT(dir.cd(QStringLiteral("ui")));
    QFile mainFile(dir.path() + QStringLiteral("/main.qml"));
    Q_ASSERT(mainFile.exists());
}

void PackageHandlerTest::checkLoadPackageInfo()
{
    QList<PackageHandler::Node> nodes = m_packageHandler.loadPackageInfo();
    QStringList directories;
    directories.append(QStringLiteral("images"));
    directories.append(QStringLiteral("scripts"));
    directories.append(QStringLiteral("data"));
    directories.append(QStringLiteral("ui"));
    directories.append(QStringLiteral("theme"));
    directories.append(QStringLiteral("config"));
    directories.append(QStringLiteral("translations"));
    for (const auto &it : nodes) {
        Q_ASSERT_X(directories.contains(it.name),
                   "checking Directories node", it.name.toLocal8Bit().data());
    }
}

void PackageHandlerTest::cleanupTestCase()
{
}

QTEST_GUILESS_MAIN(PackageHandlerTest)

