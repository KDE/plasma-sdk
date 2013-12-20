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

    QDir dir(m_packageHandler.projectPath() + m_packageHandler.contentsPrefix());
    Q_ASSERT(dir.exists());
    //FIXME make it more generic?
    Q_ASSERT(dir.cd(QStringLiteral("ui")));
    QFile mainFile(dir.path() + QStringLiteral("/main.qml"));
    Q_ASSERT(mainFile.exists());
}

void PackageHandlerTest::checkLoadPackageInfo()
{
    QFile testFile(m_packageHandler.projectPath() + m_packageHandler.contentsPrefix() +
                   QStringLiteral("ui/foo.qml"));
    testFile.open(QIODevice::ReadWrite);
    testFile.close();

    testFile.setFileName(m_packageHandler.projectPath() + m_packageHandler.contentsPrefix() +
                         QStringLiteral("bar.qml"));
    testFile.open(QIODevice::ReadWrite);
    testFile.close();

    QList<PackageHandler::Node> nodes = m_packageHandler.loadPackageInfo();

    Q_ASSERT_X(nodes.size() == 9, "Check node list size", QString(nodes.size()).toLocal8Bit().data());

    QStringList topEntriesList;
    topEntriesList.append(QStringLiteral("images"));
    topEntriesList.append(QStringLiteral("scripts"));
    topEntriesList.append(QStringLiteral("data"));
    topEntriesList.append(QStringLiteral("ui"));
    topEntriesList.append(QStringLiteral("theme"));
    topEntriesList.append(QStringLiteral("config"));
    topEntriesList.append(QStringLiteral("translations"));
    topEntriesList.append(QStringLiteral("bar.qml"));

    QStringList indexedTopLevel;

    bool loopOk = false;

    for (const auto &it : nodes) {
        if (it.name != QStringLiteral("metadata.desktop")) {
            Q_ASSERT_X(topEntriesList.contains(it.name),
                       "checking top entries node", it.name.toLocal8Bit().data());

            indexedTopLevel.append(it.name);

            if (it.name == QStringLiteral("ui")) {
                QStringList uiDirFileList;
                for (const auto &uiFile : it.children) {
                    uiDirFileList.append(uiFile.name);
                }

                loopOk = !uiDirFileList.isEmpty();

                const QString path= m_packageHandler.projectPath() +
                                    m_packageHandler.contentsPrefix() + it.name;
                Q_ASSERT_X(uiDirFileList.contains(QStringLiteral("foo.qml")),
                           "Checking unamed files under named directory",
                           path.toLocal8Bit().data());
            }
        }
    }
    Q_ASSERT(loopOk);
    Q_ASSERT_X(indexedTopLevel.contains("bar.qml"),
               "Checking top level unnamed files",
               "bar.qml doesn't exist");
}

void PackageHandlerTest::cleanupTestCase()
{
}

QTEST_GUILESS_MAIN(PackageHandlerTest)

