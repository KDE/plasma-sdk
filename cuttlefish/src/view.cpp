/***************************************************************************
 *                                                                         *
 *   Copyright 2014 Sebastian Kügler <sebas@kde.org>                       *
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
 ***************************************************************************/

#include "view.h"
#include "iconmodel.h"

#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>

#include <Plasma/Package>
#include <Plasma/PluginLoader>

#include <KDeclarative/KDeclarative>
#include <KLocalizedString>

using namespace CuttleFish;

View::View(const QString &category, QCommandLineParser &parser, QWindow *parent)
    : QQuickView(parent),
    m_browserRootItem(nullptr)
{
    setResizeMode(QQuickView::SizeRootObjectToView);
    QQuickWindow::setDefaultAlphaBuffer(true);

    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(engine());
    kdeclarative.setTranslationDomain(QStringLiteral("cuttlefish"));
    kdeclarative.setupBindings();

    auto iconModel = new IconModel(this);
    rootContext()->setContextProperty("iconModel", iconModel);
    rootContext()->setContextProperty("pickerMode", parser.isSet("picker"));
    qmlRegisterType<IconModel>();

    m_package = Plasma::PluginLoader::self()->loadPackage("Plasma/Generic");
    m_package.setPath("org.kde.plasma.cuttlefish");

    if (!m_package.isValid() || !m_package.metadata().isValid()) {
        qWarning() << "Could not load package org.kde.plasma.cuttlefish:" << m_package.path();
        return;
    }

    setIcon(QIcon::fromTheme(m_package.metadata().icon()));
    setTitle(m_package.metadata().name());

    const QString qmlFile = m_package.filePath("mainscript");
    setSource(QUrl::fromLocalFile(m_package.filePath("mainscript")));
    show();

    //qDebug() << "m_dirModel" << m_dirModel.rowCount(dirModel::index(KDirModel::Name);

}

View::~View()
{
}
