/********************************************************************
 * This file is part of the KDE project.
 *
 * Copyright (C) 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *********************************************************************/

#include "tabboxpreviewer.h"
#include <KStandardDirs>
#include <KConfigGroup>
#include <KConfig>
#include <QVBoxLayout>
#include <QWidget>
#include <QGraphicsWidget>

TabBoxPreviewer::TabBoxPreviewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
    : Previewer(title, parent, flags)
{
    m_view = new WindowSwitcher(this);
    connect(m_view->rootObject(), SIGNAL(clicked()), this, SLOT(signalForward()));
    init();
}


void TabBoxPreviewer::showPreview(const QString &packagePath)
{
    m_view->setLayout(packageMainFile(packagePath));
}


QString TabBoxPreviewer::packageMainFile(const QString& projectPath)
{
    KConfig c(projectPath  + '/' + "metadata.desktop");
    KConfigGroup projectInformation(&c, "Desktop Entry");
    const QString relativeFilenamePath = projectInformation.readEntry("X-Plasma-MainScript");
    m_fullFilenamePath = KStandardDirs::locate("data", projectPath + "/contents" + '/' + relativeFilenamePath);
    return m_fullFilenamePath;
}

void TabBoxPreviewer::refreshPreview()
{
    if (m_view) {
        m_view =0;
        m_view = new WindowSwitcher(this);
        connect(m_view->rootObject(), SIGNAL(clicked()), this, SLOT(signalForward()));
        setLayoutPath(m_fullFilenamePath);
        init();
    }
}

void TabBoxPreviewer::setLayoutPath(const QString& filePath)
{
    if (m_view) {
        m_view->setLayout(filePath);
    }
}

void TabBoxPreviewer::signalForward()
{
    refreshPreview();
}


void TabBoxPreviewer::init()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_view);
    QWidget *w = new QWidget(this);
    w->setLayout(layout);

    setWidget(w);
}

#include "moc_tabboxpreviewer.cpp"