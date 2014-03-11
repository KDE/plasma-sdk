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

#include "common.h"
#include "../../startpage.h"
#include "startpagetest.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QTest>

StartPageTest::StartPageTest(QWidget *parent)
        : QWidget(parent),
          m_startPage(0)
{
    createTestData();

    m_startPage = new StartPage();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_startPage);
    setLayout(layout);
}

StartPageTest::~StartPageTest()
{
}

void StartPageTest::createTestData()
{
    #pragma message("We disable this test, until we find a way to make it able to work without hardcoded paths.")
    #if 0
    QString newPlasmoidLocation = QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0);

    QDir templatesDir(newPlasmoidLocation);
    templatesDir.mkpath(QStringLiteral("templates"));
    templatesDir.cd(QStringLiteral("templates"));

    if (!newPlasmoidLocation.endsWith(QLatin1Char('/'))) {
        newPlasmoidLocation += QLatin1Char('/') + QStringLiteral("org.kde.packagehandlertest.newPlasmoid");
    }

    const QString mainPlasmoidFile = QFINDTESTDATA("/home/kokeroulis/sources/kde5/plasmate-linuxteam/plasmate/tests/testdata/mainPlasmoid.qml");
    Q_ASSERT(!mainPlasmoidFile.isEmpty());

    QFile f(mainPlasmoidFile);
    f.copy(templatesDir.path() + QLatin1Char('/') + QStringLiteral("mainPlasmoid.qml"));
    #endif
}

PLASMATE_TEST_MAIN(StartPageTest)

