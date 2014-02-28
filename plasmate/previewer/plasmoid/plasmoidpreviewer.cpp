/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 * Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "plasmoidpreviewer.h"
#include "../plasmoid/../../../plasmoidviewer/view.h"
#include "../../editors/metadata/metadatahandler.h"

#include <KLocalizedString>
#include <KMessageBox>

PlasmoidPreviewer::PlasmoidPreviewer(const QString &title, QWidget *parent, Qt::WindowFlags flags)
        : Previewer(title, parent, flags),
          m_view(new View(View::createCorona(), false))
{
    m_view->addContainment(QStringLiteral("org.kde.desktopcontainment"));

    setWidget(QWidget::createWindowContainer(m_view.data()));
}

PlasmoidPreviewer::~PlasmoidPreviewer()
{
}

void PlasmoidPreviewer::showPreview(const QString &packagePath)
{
    QString metadataPath = packagePath;
    if (!metadataPath.endsWith(QLatin1Char('/'))) {
        metadataPath += QLatin1Char('/');
    }

    metadataPath += QStringLiteral("metadata.desktop");

    if (!QFile::exists(metadataPath)) {
        KMessageBox::error(this, i18n("metadata.desktop doesn't exist"));
        return;
    }

    MetadataHandler metadata;
    metadata.setFilePath(metadataPath);
    m_view->addApplet(metadata.pluginName());
}

void PlasmoidPreviewer::refreshPreview()
{
    emit refreshView();
}

