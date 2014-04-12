/*

Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

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

#include "themehandlertest.h"
#include "../packagehandler/themehandler.h"

#include <QDir>
#include <QTest>
#include <QString>
#include <QStandardPaths>

ThemeHandlerTest::ThemeHandlerTest(QObject *parent)
        : QObject(parent),
          m_packageHandler(new ThemeHandler(this))
{
}

ThemeHandlerTest::~ThemeHandlerTest()
{
}

void ThemeHandlerTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);
    connect(m_packageHandler, &PackageHandler::error, [=](const QString &error) {
        QFAIL(error.toLocal8Bit().data());
    });
}

void ThemeHandlerTest::createNewTheme()
{
    QString newThemeLocation = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    const QString dummyAuthor = QStringLiteral("dummyThemeAuthor");
    const QString dummyEmail = QStringLiteral("dummyThemeEmail");
    const QString packageName = QStringLiteral("dummyTheme");

    if (!newThemeLocation.endsWith(QLatin1Char('/'))) {
        newThemeLocation += QLatin1Char('/');
    }

    newThemeLocation += packageName;

    m_packageHandler->setPackagePath(newThemeLocation);
    m_packageHandler->createPackage(dummyAuthor, dummyEmail, "", packageName);
    QDir d(newThemeLocation);

    Q_ASSERT(d.exists());
    Q_ASSERT(d.exists(QStringLiteral("metadata.desktop")));
    Q_ASSERT(d.exists(m_packageHandler->contentsPrefix()));

    d.cd(m_packageHandler->contentsPrefix());

    const QString dirName = QStringLiteral("dialogs");
    d.mkpath(dirName);
    d.cd(dirName);

    QStringList files;
    files << QStringLiteral("/background.svg")
          << QStringLiteral("/shutdowndialog.svg");

    for (const auto &it : files) {
        QFile f(d.path() + it);
        f.open(QIODevice::ReadWrite);
    }
}

void ThemeHandlerTest::checkNodes()
{
    PackageHandler::Node *topNode = m_packageHandler->loadPackageInfo();
    Q_ASSERT(topNode->name().isEmpty());
    Q_ASSERT(topNode->description().isEmpty());
    Q_ASSERT(topNode->mimeTypes().isEmpty());
    Q_ASSERT(!topNode->parent());
    Q_ASSERT(!topNode->childNodes().isEmpty());
    for (auto it : topNode->childNodes()) {
        QCOMPARE(topNode, it->parent());
        Q_ASSERT(!it->name().isEmpty());
        Q_ASSERT(!it->description().isEmpty());
    }
    Q_ASSERT(m_packageHandler->urlForNode(topNode).isValid());


    QStringList childNodeNames;

    auto populateChildNodeNamesList = [&](PackageHandler::Node *node) {
        childNodeNames.clear();
        for (const auto it : node->childNodes()) {
            childNodeNames.append(it->name());
        }
    };

    auto checkUrl = [=](const QString &nodePath, PackageHandler::Node *node, bool hasContentsPath) {
        QString path = m_packageHandler->packagePath();

        if (hasContentsPath) {
            path += m_packageHandler->contentsPrefix();
        }

        path += nodePath;

        const QUrl url = QUrl::fromLocalFile(path);
        const QUrl urlForNode = m_packageHandler->urlForNode(node);
        QCOMPARE(urlForNode, url);
    };

    QCOMPARE(topNode->childNodes().size(), 8);
    for (const auto it : topNode->childNodes()) {
        populateChildNodeNamesList(it);

        if (it->name() == QStringLiteral("dialogs") ||
            it->name() == QStringLiteral("opaque/dialogs") ||
            it->name() == QStringLiteral("locolor/dialogs")) {

            QCOMPARE(childNodeNames.size(), 2);

            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/background")));
            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/shutdowndialog")));

            if (it->name() == QStringLiteral("dialogs")) {
                for (const auto &child : it->childNodes()) {
                    checkUrl(child->name() + QStringLiteral(".svg"), child, true);
                }
            }
        } else if (it->name() == QStringLiteral("widgets") ||
                   it->name() == QStringLiteral("locolor/widgets")) {

            QCOMPARE(childNodeNames.size(), 5);

            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/background")));
            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/clock")));
            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/panel-background")));
            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/plot-background")));
            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/tooltip")));

            checkUrl(it->name(), it, true);
        } else if (it->name() == QStringLiteral("opaque/widgets")) {
            QCOMPARE(childNodeNames.size(), 2);

            Q_ASSERT(childNodeNames.contains(it->name() + QStringLiteral("/panel-background")));
            Q_ASSERT(childNodeNames->contents(it->name() + QStringLiteral("/tooltip")));
        } else if (it->name() == QStringLiteral("colors")) {
            QCOMPARE(childNodeNames.size(), 0);

            checkUrl(it->name(), it, true);
        } else if (it->name() == QStringLiteral("metadata.desktop")) {
            QCOMPARE(childNodeNames.size(), 0);

            checkUrl(it->name(), it, false);
        } else {
            const QString errorMessage = QString(QStringLiteral("Unrecognized name %1")).arg(it->name());
            QFAIL(errorMessage.toLocal8Bit().data());
        }
    }
}

void ThemeHandlerTest::cleanupTestCase()
{
    QDir testdata(QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0));
    testdata.removeRecursively();
}

QTEST_GUILESS_MAIN(ThemeHandlerTest)

