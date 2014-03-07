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


#include "metadatahandler.h"

#include <KConfig>
#include <KSharedConfig>

#include <QDebug>
MetadataHandler::MetadataHandler(QObject *parent)
        : QObject(parent)
{
}

MetadataHandler::~MetadataHandler()
{
}

void MetadataHandler::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
    m_metadataConfig = KConfigGroup(KSharedConfig::openConfig(m_filePath),
                       QStringLiteral("Desktop Entry"));
}

QString MetadataHandler::filePath() const
{
    return m_filePath;
}

void MetadataHandler::writeFile()
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-EnabledByDefault", true);
    m_metadataConfig.writeEntry("Type", QStringLiteral("Service"));
    m_metadataConfig.writeEntry("X-Plasma-MainScript", mainScript());
    m_metadataConfig.writeEntry("X-Plasma-DefaultSize", QSize(200, 100));
    m_metadataConfig.sync();
}

QString MetadataHandler::name() const
{
    return m_metadataConfig.readEntry("Name", QString());
}

void MetadataHandler::setName(const QString &name)
{
    m_metadataConfig.writeEntry("Name", name);
}

QString MetadataHandler::description() const
{
    return m_metadataConfig.readEntry("Comment", QString());
}

void MetadataHandler::setDescription(const QString &description)
{
    m_metadataConfig.writeEntry("Comment", description);
}

QString MetadataHandler::icon() const
{
    return m_metadataConfig.readEntry("Icon", QString());
}

void MetadataHandler::setIcon(const QString &icon)
{
    m_metadataConfig.writeEntry("Icon", icon);
}

QString MetadataHandler::category() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Category", QString());
}

void MetadataHandler::setCategory(const QString &category)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-Category", category);
}

QString MetadataHandler::version() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Version", QString());
}

void MetadataHandler::setVersion(const QString &version)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-Version", version);
}

QString MetadataHandler::website() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Website", QString());
}

void MetadataHandler::setWebsite(const QString &website)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-Website", website);
}

QString MetadataHandler::author() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Author", QString());
}

void MetadataHandler::setAuthor(const QString &author)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-Author", author);
}

QString MetadataHandler::email() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Email", QString());
}

void MetadataHandler::setEmail(const QString &email)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-Email", email);
}

QString MetadataHandler::license() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-License", QString());
}

void MetadataHandler::setLicense(const QString &license)
{
    m_metadataConfig.writeEntry("X-KDE-PluginInfo-License", license);
}

QStringList MetadataHandler::serviceTypes() const
{
    return m_metadataConfig.readEntry("X-KDE-ServiceTypes", QStringList());
}

void MetadataHandler::setServiceTypes(const QStringList &serviceTypes)
{
    m_metadataConfig.writeEntry("X-KDE-ServiceTypes", serviceTypes);
}

QString MetadataHandler::pluginName() const
{
    return m_metadataConfig.readEntry("X-KDE-PluginInfo-Name", QString());
}

void MetadataHandler::setPluginName(const QString &pluginName)
{
    return m_metadataConfig.writeEntry("X-KDE-PluginInfo-Name", pluginName);
}

QString MetadataHandler::pluginApi() const
{
    return m_metadataConfig.readEntry("X-Plasma-API", QString());
}

void MetadataHandler::setPluginApi(const QString &pluginApi)
{
    m_metadataConfig.writeEntry("X-Plasma-API", pluginApi);
}

QString MetadataHandler::mainScript() const
{
    return m_metadataConfig.readEntry("X-Plasma-MainScript", QString());
}

void MetadataHandler::setMainScript(const QString &mainScriptName)
{
    return m_metadataConfig.writeEntry("X-Plasma-MainScript", mainScriptName);
}

