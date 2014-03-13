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
}

void PackageHandlerTest::initPlasmoid()
{
}

void PackageHandlerTest::createNewPlasmoid()
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

    m_packageHandler.setPackagePath(newPlasmoidLocation);
    m_packageHandler.createPackage(QStringLiteral("author"), QStringLiteral("email"), QStringLiteral("Plasma/Applet"), QStringLiteral("newPlasmoid"), QStringLiteral("main.qml"), QStringLiteral("declarative"), QStringLiteral(".qml"));
    m_packageHandler.loadPackage();

    QDir dir(newPlasmoidLocation);
    Q_ASSERT(dir.exists());
    Q_ASSERT(dir.exists(QStringLiteral("metadata.desktop")));
    Q_ASSERT(dir.exists(QStringLiteral("contents")));
    Q_ASSERT(dir.cd(QStringLiteral("contents")));
    Q_ASSERT(dir.exists(QStringLiteral("ui")));
    Q_ASSERT(dir.cd(QStringLiteral("ui")));
    Q_ASSERT(dir.exists(QStringLiteral("main.qml")));
}

void PackageHandlerTest::loadPlasmoidTestData()
{
    const QString packagePath = QFINDTESTDATA("testdata/org.kde.tests.packagehandlertest");
    Q_ASSERT(!packagePath.isEmpty());

    m_packageHandler.setPackagePath(packagePath);
}

void PackageHandlerTest::checkPlasmoidMimeTypes()
{
    QStringList mimeTypes;
    PackageHandler::Node* node = m_packageHandler.loadPackageInfo();
    for (auto it : node->childNodes()) {
        mimeTypes.append(it->mimeTypes());
        if (!it->childNodes().isEmpty()) {
            for(auto childIt : it->childNodes()) {
                mimeTypes.append(childIt->mimeTypes());
            }
        }
    }

    Q_ASSERT_X(mimeTypes.contains(QStringLiteral("[plasmate]/kconfigxteditor")),
               "Checking mimetypes","[plasmate]/kconfigxteditor/ doesn't exist");
    Q_ASSERT_X(mimeTypes.contains(QStringLiteral("text/x-qml")), "Checking mimetypes",
              "text/x-qml doesn't exist");
    Q_ASSERT_X(mimeTypes.contains(QStringLiteral("[plasmate]/new")), "Checking mimetypes",
              "[plasmate]/new doesn't exist");
    Q_ASSERT_X(mimeTypes.contains(QStringLiteral("[plasmate]/imageViewer")), "Checking mimetypes",
              "[plasmate]/imageViewer doesn't exist");
    Q_ASSERT_X(mimeTypes.contains(QStringLiteral("[plasmate]/imageDialog")), "Checking mimetypes",
              "[plasmate]/imageDialog doesn't exist");
}

void PackageHandlerTest::checkNodes()
{
    PackageHandler::Node *topNode = m_packageHandler.loadPackageInfo();
    Q_ASSERT(topNode->name().isEmpty());
    Q_ASSERT(topNode->description().isEmpty());
    Q_ASSERT(topNode->mimeTypes().isEmpty());
    Q_ASSERT(!topNode->parent());
    Q_ASSERT(!topNode->childNodes().isEmpty());
    for (auto it : topNode->childNodes()) {
        QCOMPARE(topNode, it->parent());
        Q_ASSERT(!it->name().isEmpty());
        Q_ASSERT(!it->description().isEmpty());
        if (!it->isFile() && it->needsNewFileNode()) {
            const QString firstNodeDescription = it->childNodes().at(0)->description();
            QCOMPARE(firstNodeDescription, QStringLiteral("New.."));
        } else {
            Q_ASSERT(m_packageHandler.urlForNode(it).isValid());
            QCOMPARE(it->needsNewFileNode(), false);
        }
    }
    Q_ASSERT(m_packageHandler.urlForNode(topNode).isValid());
}

void PackageHandlerTest::checkPlasmoidNodes()
{
    PackageHandler::Node *topNode = m_packageHandler.loadPackageInfo();

    QStringList childNodeNames;

    auto populateChildNodeNamesList = [&](PackageHandler::Node *node) {
        childNodeNames.clear();
        for (const auto it : node->childNodes()) {
            childNodeNames.append(it->description());
        }
    };

    QCOMPARE(topNode->childNodes().size(), 9);
    for (const auto it : topNode->childNodes()) {
        if (it->description() == QStringLiteral("User Interface")) {
            populateChildNodeNamesList(it);
            QCOMPARE(childNodeNames.size(), 3);
            Q_ASSERT(childNodeNames.contains(QStringLiteral("New..")));
            Q_ASSERT(childNodeNames.contains(QStringLiteral("Main Script File")));
            Q_ASSERT(childNodeNames.contains(QStringLiteral("foo.qml")));
        } else if (it->description() == QStringLiteral("Configuration Definitions")) {
            populateChildNodeNamesList(it);
            QCOMPARE(childNodeNames.size(), 2);
            Q_ASSERT(childNodeNames.contains(QStringLiteral("Configuration UI pages model")));
            Q_ASSERT(childNodeNames.contains("Configuration XML file"));
        } else if (it->description() == QStringLiteral("Data Files") ||
                   it->description() == QStringLiteral("Executable Scripts") ||
                   it->description() == QStringLiteral("Translations") ||
                   it->description() == QStringLiteral("Themed Images")) {
            populateChildNodeNamesList(it);
            QCOMPARE(childNodeNames.size(), 1);
            Q_ASSERT(childNodeNames.contains(QStringLiteral("New..")));
        } else if(it->description() == QStringLiteral("Images")) {
            populateChildNodeNamesList(it);
            QCOMPARE(childNodeNames.size(), 2);
            Q_ASSERT(childNodeNames.contains(QStringLiteral("New..")));
            Q_ASSERT(childNodeNames.contains(QStringLiteral("plasmagik.png")));
        } else if (it->description() == QStringLiteral("metadata.desktop")) {
            QCOMPARE(childNodeNames.size(), 0);
        } else if (it->description() == QStringLiteral("bar.qml")) {
            populateChildNodeNamesList(it);
            QCOMPARE(childNodeNames.size(), 0);
        } else {
            const QString errorMessage = QString(QStringLiteral("Unrecognized name %1")).arg(it->name());
            QFAIL(errorMessage.toLocal8Bit().data());
        }
    }
}


QTEST_GUILESS_MAIN(PackageHandlerTest)

