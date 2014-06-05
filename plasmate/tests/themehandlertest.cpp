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

void ThemeHandlerTest::checkDescriptions()
{
   Q_ASSERT(!m_packageHandler->description(QStringLiteral("opaque")).isEmpty());
   Q_ASSERT(!m_packageHandler->description(QStringLiteral("dialogs/background")).isEmpty());
   Q_ASSERT(m_packageHandler->description(QStringLiteral("invalid")).isEmpty());
}

void ThemeHandlerTest::cleanupTestCase()
{
    QDir testdata(QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0));
    testdata.removeRecursively();
}

QTEST_GUILESS_MAIN(ThemeHandlerTest)

