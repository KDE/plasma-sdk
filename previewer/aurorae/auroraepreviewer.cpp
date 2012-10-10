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

#include "auroraepreviewer.h"
#include <QVariantList>
#include <QGraphicsObject>
#include <KDebug>
#include <QObject>

AuroraePreviewer::AuroraePreviewer(const QString & title, QWidget * parent, Qt::WindowFlags flags)
: Previewer(title, parent, flags)
{
    m_view = new KWin::KWinDecorationModule();
    connect(m_view->decorationForm()->rootObject(), SIGNAL(clicked()), this, SLOT(refreshPreview()));
    setWidget(m_view);
}

void AuroraePreviewer::init()
{
    if(m_view) {
        m_view = 0;
        m_view = new KWin::KWinDecorationModule();
        //we need to create the SIGNAL again because when we initialize our object
        //the signal was lost
        connect(m_view->decorationForm()->rootObject(), SIGNAL(clicked()), this, SLOT(refreshPreview()));
        m_view->loadDecoration(m_packagePath);
        setWidget(m_view);
    }
}


void AuroraePreviewer::showPreview(const QString &packagePath)
{
    kDebug() << "222222222222222222222222222222222";
    m_packagePath = packagePath;
    m_view->loadDecoration(m_packagePath);
    //m_view->setLayout(packageMainFile(packagePath));
}

void AuroraePreviewer::refreshPreview()
{
    kDebug() << "testtttttttttttttttttttttttttttttttttttT";
    init();

}
#include "moc_auroraepreviewer.cpp"