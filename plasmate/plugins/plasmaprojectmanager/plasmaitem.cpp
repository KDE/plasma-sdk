/*
    Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "plasmaitem.h"
#include "packagehandler/plasmoidhandler.h"
#include "packagehandler/themehandler.h"
#include "editors/metadata/metadatahandler.h"

#include <interfaces/iproject.h>

#include <QFile>

AbstractPlasmaItem::AbstractPlasmaItem(const Path &path, ProjectBaseItem *parent)
    : m_packageHandler(nullptr),
      m_path(path)
{
    AbstractPlasmaItem *p = dynamic_cast<AbstractPlasmaItem*>(parent);

    if (p && p->packageHandler()) {
        setPackageHandler(p->packageHandler());
    } else {
        const QString metadataFilePath = m_path.toLocalFile() + QStringLiteral("/metadata.desktop");

        if (QFile::exists(metadataFilePath)) {
            MetadataHandler metadata;
            metadata.setFilePath(metadataFilePath);
            const QString pluginApi = metadata.pluginApi();
            if (pluginApi == QStringLiteral("declarativeappletscript")) {
                m_packageHandler = QSharedPointer<PlasmoidHandler>::create();
            } else {
                m_packageHandler = QSharedPointer<ThemeHandler>::create();
            }

            m_packageHandler->setPackagePath(m_path.toLocalFile());
        }
    }
}

AbstractPlasmaItem::~AbstractPlasmaItem()
{
}

QSharedPointer<PackageHandler> AbstractPlasmaItem::packageHandler() const
{
    return m_packageHandler;
}

void AbstractPlasmaItem::setPackageHandler(const QSharedPointer<PackageHandler> &packageHandler)
{
    m_packageHandler = packageHandler;
}

QString AbstractPlasmaItem::description() const
{
    if (!m_packageHandler) {
        return QString();
    }

    QString textItem = m_path.lastPathSegment();

    if (textItem.endsWith(QStringLiteral(".svg")) ||
        textItem.endsWith(QStringLiteral(".svgz"))) {
        textItem = textItem.split(QLatin1Char('.')).first();
    }

    return m_packageHandler->description(textItem);
}

PlasmaFileItem::PlasmaFileItem(IProject* project, const Path& path, ProjectBaseItem* parent)
    : ProjectFileItem(project, path, parent),
      AbstractPlasmaItem(path, parent)
{
    const QString textItem = description();
    if (!textItem.isEmpty()) {
        setText(textItem);
    }
}

PlasmaFileItem::~PlasmaFileItem()
{
}

PlasmaFolderItem::PlasmaFolderItem(IProject* project, const Path& path, ProjectBaseItem* parent)
    : ProjectFolderItem(project, path, parent),
      AbstractPlasmaItem(path, parent)
{
    const QString textItem = description();
    if (!textItem.isEmpty()) {
        setText(textItem);
    }
}

PlasmaFolderItem::~PlasmaFolderItem()
{
}

