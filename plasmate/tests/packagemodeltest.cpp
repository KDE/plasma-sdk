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

#include "packagemodeltest.h"
#include "../packagemodel.h"
#include "modeltest.h"

#include <QTest>

PackageModelTest::PackageModelTest(QObject *parent)
        : QObject(parent)
{
}

PackageModelTest::~PackageModelTest()
{
}

void PackageModelTest::initTestCase()
{
    QStandardPaths::enableTestMode(true);
}

void PackageModelTest::testPlasmoidModel()
{
    PackageHandler *packageHandler = new PlasmoidHandler(this);
    const QString testDataPlasmoid = QFINDTESTDATA(QStringLiteral("testdata/org.kde.tests.packagehandlertest"));
    Q_ASSERT(!testDataPlasmoid.isEmpty());

    packageHandler->setPackagePath(testDataPlasmoid);
    packageHandler->createPackage(QStringLiteral("author"), QStringLiteral("email"), QStringLiteral("Plasma/Applet"), QStringLiteral("newPlasmoid"));

    PackageModel *model = new PackageModel(packageHandler, this);
    packageHandler->loadPackageInfo();
    new ModelTest(model, this);
}

QTEST_MAIN(PackageModelTest)

