/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2009 Martin Gräßlin <kde@martin-graesslin.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#include "decorationmodel.h"
#include "preview.h"
// kwin
#include <kdecorationfactory.h>
// Qt
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QTextDocument>
// KDE
#include <KConfigGroup>
#include <KDesktopFile>
#include <KGlobalSettings>
#include <KIcon>
#include <KLocale>
#include <KServiceTypeTrader>
#include <KStandardDirs>
#include <KPluginInfo>
#include "kwindecoration.h"
#include <KDebug>

/* WARNING -------------------------------------------------------------------------
* it is *ABSOLUTELY* mandatory to manage loadPlugin() and destroyPreviousPlugin()
* using disablePreview()
*
* loadPlugin() moves the present factory pointer to "old_fact" which is then deleted
* by the succeeding destroyPreviousPlugin()
*
* So if you loaded a new plugin and that changed the current factory, call disablePreview()
* BEFORE the following destroyPreviousPlugin() destroys the factory for the current m_preview->deco->factory
* (which is invoked on deco deconstruction)
* WARNING ------------------------------------------------------------------------ */

namespace KWin
{

DecorationModel::DecorationModel(KSharedConfigPtr config, QObject* parent)
    : QAbstractListModel(parent)
    , m_plugins(new KDecorationPreviewPlugins(config))
    , m_preview(new KDecorationPreview())
    , m_customButtons(false)
    , m_leftButtons(QString())
    , m_rightButtons(QString())
    , m_renderWidget(new QWidget(0))
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "display";
    roleNames[DecorationModel::PixmapRole] = "preview";
    roleNames[TypeRole] = "type";
    roleNames[AuroraeNameRole] = "auroraeThemeName";
    roleNames[QmlMainScriptRole] = "mainScript";
    roleNames[BorderSizeRole] = "borderSize";
    roleNames[ButtonSizeRole] = "buttonSize";
    setRoleNames(roleNames);
    m_config = KSharedConfig::openConfig("auroraerc");
}

DecorationModel::~DecorationModel()
{
    delete m_preview;
    delete m_plugins;
    delete m_renderWidget;
}



// Find all theme desktop files in all 'data' dirs owned by kwin.
// And insert these into a DecorationInfo structure
void DecorationModel::findDecorations(const QString &filePath)
{
    beginResetModel();
    QString tmpFile = filePath;

    //This method can be called only either inside from the plasmate or from the windowdecoration previewer
    //Inside from the plasmate the filePath contains our projectPath
    //But inside from the windowdecoration previewer the filepath contains our metadata.desktop file.
    //So we much check if the filaPath is a metadata.desktop file or a directory.
    if (!KDesktopFile::isDesktopFile(filePath)) {
        tmpFile.append("metadata.desktop");
    }
    KDesktopFile desktopFile(tmpFile);

    DecorationModelData data;
    data.name = desktopFile.readName();
    data.libraryName = "kwin3_aurorae";
    data.type = DecorationModelData::QmlDecoration;
    data.auroraeName = desktopFile.desktopGroup().readEntry("X-KDE-PluginInfo-Name", QString());
    QString scriptName = desktopFile.desktopGroup().readEntry("X-Plasma-MainScript", QString());
    data.qmlPath = KStandardDirs::locateLocal("appdata", data.auroraeName + "/contents/" + scriptName);

    //When we run the windowdecoration previewer, then our application is no more the plasmate
    //but the windowdecoration. So our locateLocal is targeting to the false directory
    data.qmlPath.replace("windowdecoration", "plasmate");
        //TODO koker here we should put the directory of our plasmate package.
        kDebug() << "pathhhhhhhh" << data.auroraeName + "/contents/" + scriptName;
        kDebug() <<"qmlPATHHHHHHHHHHHH" << data.qmlPath;
        //e.x. ~/.kde/share/apps/plasmate/koker/contents/ui/main.qml
        //QString test = "/home/kokeroulis/.kde/share/apps/plasmate/koker/contents/ui/main.qml";
        //data.qmlPath = test;

    /*TODO Is that code needed?    
        KConfigGroup config(m_config, data.auroraeName);
        data.borderSize = (KDecorationDefines::BorderSize)config.readEntry< int >("BorderSize", KDecorationDefines::BorderNormal);
        data.buttonSize = (KDecorationDefines::BorderSize)config.readEntry< int >("ButtonSize", KDecorationDefines::BorderNormal);
        data.closeDblClick = config.readEntry< bool >("CloseOnDoubleClickMenuButton", true);
        data.comment = service->comment();
        KPluginInfo info(service);
        data.author = info.author();
        data.email= info.email();
        data.version = info.version();
        data.license = info.license();
        data.website = info.website();
    */
    kDebug() << "testttttttttttttttttt3";
        //We need to clear our stack in order to show only the current previewer
        //and not the previous version
        m_decorations.clear();
        m_decorations.append(data);
    //}
    qSort(m_decorations.begin(), m_decorations.end(), DecorationModelData::less);
    endResetModel();
}

void DecorationModel::metaData(DecorationModelData& data, const KDesktopFile& df)
{
    data.comment = df.readComment();
    data.author = df.desktopGroup().readEntry("X-KDE-PluginInfo-Author", QString());
    data.email = df.desktopGroup().readEntry("X-KDE-PluginInfo-Email", QString());
    data.version = df.desktopGroup().readEntry("X-KDE-PluginInfo-Version", QString());
    data.license = df.desktopGroup().readEntry("X-KDE-PluginInfo-License", QString());
    data.website = df.desktopGroup().readEntry("X-KDE-PluginInfo-Website", QString());
    //TODO is that method needed?
}

int DecorationModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_decorations.count();
}

QVariant DecorationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
    case NameRole:
        return m_decorations[ index.row()].name;
    case LibraryNameRole:
        return m_decorations[ index.row()].libraryName;
    case PixmapRole:
        return m_decorations[ index.row()].preview;
    case TypeRole:
        return m_decorations[ index.row()].type;
    case AuroraeNameRole:
        return m_decorations[ index.row()].auroraeName;
    case PackageDescriptionRole:
        return m_decorations[ index.row()].comment;
    case PackageAuthorRole:
        return m_decorations[ index.row()].author;
    case PackageEmailRole:
        return m_decorations[ index.row()].email;
    case PackageWebsiteRole:
        return m_decorations[ index.row()].website;
    case PackageVersionRole:
        return m_decorations[ index.row()].version;
    case PackageLicenseRole:
        return m_decorations[ index.row()].license;
    case BorderSizeRole:
        return static_cast< int >(m_decorations[ index.row()].borderSize);
    case BorderSizesRole: {
        QList< QVariant > sizes;
        const bool mustDisablePreview = m_plugins->factory() && m_plugins->factory() == m_preview->factory();
        if (m_plugins->loadPlugin(m_decorations[index.row()].libraryName) && m_plugins->factory()) {
            foreach (KDecorationDefines::BorderSize size, m_plugins->factory()->borderSizes())   // krazy:exclude=foreach
                sizes << int(size);
            if (mustDisablePreview) // it's nuked with destroyPreviousPlugin()
                m_preview->disablePreview(); // so we need to get rid of m_preview->deco first
            m_plugins->destroyPreviousPlugin();
        }
        return sizes;
    }
    case ButtonSizeRole:
        if (m_decorations[ index.row()].type == DecorationModelData::AuroraeDecoration ||
            m_decorations[ index.row()].type == DecorationModelData::QmlDecoration)
            return static_cast< int >(m_decorations[ index.row()].buttonSize);
        else
            return QVariant();
    case QmlMainScriptRole:
        return m_decorations[ index.row()].qmlPath;
    case CloseOnDblClickRole:
        return m_decorations[index.row()].closeDblClick;
    default:
        return QVariant();
    }
}

bool DecorationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || (role != BorderSizeRole && role != ButtonSizeRole && role != CloseOnDblClickRole))
        return QAbstractItemModel::setData(index, value, role);

    const DecorationModelData::DecorationType type = m_decorations[ index.row()].type;

    if (role == BorderSizeRole) {
        m_decorations[ index.row()].borderSize = (KDecorationDefines::BorderSize)value.toInt();
        if (type == DecorationModelData::AuroraeDecoration || type == DecorationModelData::QmlDecoration) {
            KConfigGroup config(m_config, m_decorations[ index.row()].auroraeName);
            config.writeEntry("BorderSize", value.toInt());
            config.sync();
        }
        emit dataChanged(index, index);

        return true;
    }
    if (role == ButtonSizeRole && (type == DecorationModelData::AuroraeDecoration || type == DecorationModelData::QmlDecoration)) {
        m_decorations[ index.row()].buttonSize = (KDecorationDefines::BorderSize)value.toInt();
        KConfigGroup config(m_config, m_decorations[ index.row()].auroraeName);
        config.writeEntry("ButtonSize", value.toInt());
        config.sync();
        emit dataChanged(index, index);

        return true;
    }
    if (role == CloseOnDblClickRole && (type == DecorationModelData::AuroraeDecoration || type == DecorationModelData::QmlDecoration)) {
        if (m_decorations[ index.row()].closeDblClick == value.toBool()) {
            return false;
        }
        m_decorations[ index.row()].closeDblClick = value.toBool();
        KConfigGroup config(m_config, m_decorations[ index.row()].auroraeName);
        config.writeEntry("CloseOnDoubleClickMenuButton", value.toBool());
        config.sync();
        emit dataChanged(index, index);
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

} // namespace KWin
