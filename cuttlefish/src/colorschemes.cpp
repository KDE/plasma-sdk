/***************************************************************************
 *                                                                         *
 *   Copyright 2019 David Redondo <kde@david-redondo.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

#include "colorschemes.h"

#include <QAbstractItemModel>

#include <KColorSchemeManager>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

using namespace CuttleFish;

ColorSchemes::ColorSchemes(QObject *parent)
    : QObject(parent)
    , m_colorManager(new KColorSchemeManager(this))
{
}

QAbstractItemModel* CuttleFish::ColorSchemes::colorSchemes()
{
    return m_colorManager->model();
}

void ColorSchemes::activateColorScheme(const QString& name)
{
    m_colorManager->activateScheme(m_colorManager->indexForScheme(name));
}

