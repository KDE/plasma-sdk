/* Copyright 2011 Kevin Ottens <ervin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.
*/

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <kdeclarative.h>
#include <KDirLister>
#include <KDirModel>
#include <KLocale>
#include <KStandardDirs>

#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeView>
#include <QtGui/QSortFilterProxyModel>

#include "plasmafiltermodel.h"

static const char description[] = I18N_NOOP("Explore the Plasma Widgets shared on the network");
static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData aboutData("plasma-remote-widgets-browser", 0, ki18n("Plasma Remote Widgets Browser"),
                         version, ki18n(description), KAboutData::License_GPL,
                         ki18n("(c) 2011 The KDE Team"));
    aboutData.addAuthor(ki18n("Kevin Ottens"),
                        ki18n( "Author" ),
                        "ervin@kde.org");
    aboutData.setProgramIconName("plasma");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KApplication app;

    QDeclarativeView *view = new QDeclarativeView;

    KDirModel *model = new KDirModel(view);
    KDirLister *lister = new KDirLister(view);
    QSortFilterProxyModel *sorter = new QSortFilterProxyModel(view);
    PlasmaFilterModel *filter = new PlasmaFilterModel(view);

    lister->openUrl(KUrl("network:/"), KDirLister::Keep);
    model->setDirLister(lister);
    sorter->setSourceModel(model);
    filter->setSourceModel(sorter);

    sorter->setDynamicSortFilter(true);
    sorter->sort(0);

    KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(view->engine());
    kdeclarative.initialize();
    //binds things like kconfig and icons
    kdeclarative.setupBindings();

    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    foreach (const QString &importPath, KGlobal::dirs()->findDirs("module", "imports")) {
        view->engine()->addImportPath(importPath);
    }

    view->rootContext()->setContextProperty("plasmaModel", filter);
    view->setSource(QUrl("qrc:/main.qml"));
    view->show();

    return app.exec();
}
