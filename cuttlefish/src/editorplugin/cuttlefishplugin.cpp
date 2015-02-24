/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
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

#include "cuttlefishplugin.h"

#include <KTextEditor/Application>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <KActionCollection>
#include <KIconLoader>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardItemModel>
#include <QStandardPaths>

K_PLUGIN_FACTORY_WITH_JSON(CuttleFishPluginFactory, "cuttlefishplugin.json", registerPlugin<CuttleFishPlugin>();)

CuttleFishPlugin::CuttleFishPlugin(QObject *parent, const QList<QVariant> &):
    KTextEditor::Plugin(parent)
    , m_action(this)
{
    qDebug() << "CUTTLEFISH PLUGIN LOADED!";
    m_action.setIcon(QIcon::fromTheme("cuttlefish"));
    m_action.setText("Insert Icon with cuttlefish");
}

CuttleFishPlugin::~CuttleFishPlugin()
{
}

QObject *CuttleFishPlugin::createView(KTextEditor::MainWindow *mainWindow)
{
    qDebug() << "CUTTLEFISH CREATE VIEW!";
    connect(KTextEditor::Editor::instance()->application(), &KTextEditor::Application::documentCreated,
            this, &CuttleFishPlugin::documentCreated);
    Q_FOREACH (auto doc, KTextEditor::Editor::instance()->application()->documents()) {
        qDebug() << "Doc!";
        documentCreated(doc);
    }

    //showCuttleFish();

    return new QObject(this);
}

void CuttleFishPlugin::documentCreated(KTextEditor::Document* document)
{
    qDebug() << "CUTTLEFISH documentCreated!!!";
    Q_FOREACH (auto view, document->views()) {
        qDebug() << "View!";
    }

    m_doc = document; // FIXME: we're currently not tracking this pointer
    connect(document, &KTextEditor::Document::viewCreated,
        [this](KTextEditor::Document *document, KTextEditor::View *view) {
            qDebug() << "!!! ViewCreated";
            connect(view, &KTextEditor::View::contextMenuAboutToShow,
                    this, &CuttleFishPlugin::contextMenuAboutToShow);
        }
    );
}

void CuttleFishPlugin::contextMenuAboutToShow(KTextEditor::View* view, QMenu* menu)
{
    qDebug() << "CUTTLEFISH ContextMenu!!!";
    menu->addAction(&m_action);
    //connect(&m_action, &QAction::triggered, this, &CuttleFishPlugin::showCuttleFish);

    connect(&m_action, &QAction::triggered, [this, view] {
        QProcess *cuttlefish = new QProcess(this);
        const QString cfexe = QStandardPaths::findExecutable("fake-cuttlefish");
        qDebug() << "CU exe: " << cfexe;
        cuttlefish->setProgram(cfexe);
        connect(cuttlefish, &QProcess::readyReadStandardOutput,
                [this, cuttlefish, view]() {
                    qDebug() << "StdOut READ";
                    auto qba = cuttlefish->readAllStandardOutput();
                    qDebug() << "STDOUT: " << qba;

                    m_doc->insertText(view->cursorPosition(), QString::fromLocal8Bit(qba));
//                     m_doc->insertLine(0, QString::fromLocal8Bit(qba));
                    cuttlefish->terminate();

                }
        );
        cuttlefish->start();
    }
    );
}

void CuttleFishPlugin::showCuttleFish()
{
    qDebug() << "CUTTLFISH showCuttleFish";
    QProcess *cuttlefish = new QProcess(this);
    const QString cfexe = QStandardPaths::findExecutable("fake-cuttlefish");
    qDebug() << "CU exe: " << cfexe;
    cuttlefish->setProgram(cfexe);
    connect(cuttlefish, &QProcess::readyReadStandardOutput,
            [this, cuttlefish]() {
                qDebug() << "StdOut READ";
                auto qba = cuttlefish->readAllStandardOutput();
                qDebug() << "STDOUT: " << qba;
                //m_doc->insertAfterCursor(qba);

            }
    );
    cuttlefish->start();
}

void CuttleFishPlugin::iconPicked()
{
    qDebug() << "CUTTLFISH iconPicked";
}


// required for CuttleFishPluginFactory vtable
#include "cuttlefishplugin.moc"
