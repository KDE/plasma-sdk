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

#include "metadatahandlertest.h"

#include <QFile>
#include <QTest>

#include <KConfigGroup>
#include <KSharedConfig>

MetadataHandlerTest::MetadataHandlerTest(QObject *parent)
        : QObject(parent)
{
}

MetadataHandlerTest::~MetadataHandlerTest()
{
}

void MetadataHandlerTest::initTestCase()
{
    const QString path = QFINDTESTDATA("testdata/metadata/battery.desktop");

    Q_ASSERT(!path.isEmpty());

    m_metadataHandler.setFilePath(path);
}

void MetadataHandlerTest::read()
{
    QCOMPARE(m_metadataHandler.name(), QStringLiteral("Battery & Brightness"));
    QCOMPARE(m_metadataHandler.description(), QStringLiteral("See the power status of your battery"));
    QCOMPARE(m_metadataHandler.icon(), QStringLiteral("battery"));
    QCOMPARE(m_metadataHandler.author(), QStringLiteral("Sebastian Kügler, Kai Uwe Broulik"));
    QCOMPARE(m_metadataHandler.category(), QStringLiteral("System Information"));
    QCOMPARE(m_metadataHandler.email(), QStringLiteral("sebas@kde.org, kde@privat.broulik.de"));
    QCOMPARE(m_metadataHandler.license(), QStringLiteral("GPL"));
    QCOMPARE(m_metadataHandler.website(), QStringLiteral("http://vizZzion.org"));
    QCOMPARE(m_metadataHandler.pluginName(), QStringLiteral("org.kde.plasma.battery"));

    QStringList servicesList;
    servicesList.append("Plasma/Applet");
    servicesList.append("Plasma/PopupApplet");
    QCOMPARE(m_metadataHandler.serviceTypes(), servicesList);
}

void MetadataHandlerTest::write()
{
    QString path = QFINDTESTDATA("testdata/org.kde.tests.packagehandlertest");
    path += QStringLiteral("/metadata.desktop");
    Q_ASSERT(!path.isEmpty());

    QFile metadataFile(path);
    if (metadataFile.exists()) {
        Q_ASSERT(metadataFile.remove());
    }

    m_metadataHandler.setFilePath(path);

    const QString name(QStringLiteral("Package handler test"));
    const QString description(QStringLiteral("A Dummy package"));
    const QString icon(QStringLiteral("plasmagik"));
    const QString author(QStringLiteral("plasma"));
    const QString category(QStringLiteral("System Information"));
    const QString email(QStringLiteral("plasma@kde.org"));
    const QString license(QStringLiteral("GPL"));
    const QString website(QStringLiteral("http://kde.org"));
    const QString pluginName(QStringLiteral("org.kde.plasmate.dummyTest"));


    m_metadataHandler.setName(name);
    m_metadataHandler.setDescription(description);
    m_metadataHandler.setIcon(icon);
    m_metadataHandler.setAuthor(author);
    m_metadataHandler.setCategory(category);
    m_metadataHandler.setEmail(email);
    m_metadataHandler.setLicense(license);
    m_metadataHandler.setWebsite(website);
    m_metadataHandler.setPluginName(pluginName);

    QStringList serviceList;
    serviceList.append("Plasma/Applet");
    m_metadataHandler.setServiceTypes(serviceList);

    m_metadataHandler.writeFile();

    QCOMPARE(m_metadataHandler.name(), name);
    QCOMPARE(m_metadataHandler.description(), description);
    QCOMPARE(m_metadataHandler.icon(), icon);
    QCOMPARE(m_metadataHandler.author(), author);
    QCOMPARE(m_metadataHandler.category(), category);
    QCOMPARE(m_metadataHandler.email(), email);
    QCOMPARE(m_metadataHandler.license(), license);
    QCOMPARE(m_metadataHandler.website(), website);
    QCOMPARE(m_metadataHandler.serviceTypes(), serviceList);
    QCOMPARE(m_metadataHandler.pluginName(), pluginName);

    KConfigGroup cg = KConfigGroup(KSharedConfig::openConfig(path),
                                   QStringLiteral("Desktop Entry"));

    QCOMPARE(cg.readEntry("X-KDE-PluginInfo-EnabledByDefault", QString()), QStringLiteral("true"));
    QCOMPARE(cg.readEntry("Type", QString()), QStringLiteral("Service"));
    QCOMPARE(cg.readEntry("X-Plasma-MainScript"), QStringLiteral("ui/main.qml"));
}

QTEST_GUILESS_MAIN(MetadataHandlerTest)

