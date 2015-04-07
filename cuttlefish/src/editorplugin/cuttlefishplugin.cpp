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
{
}

CuttleFishPlugin::~CuttleFishPlugin()
{
}

QObject *CuttleFishPlugin::createView(KTextEditor::MainWindow *mainWindow)
{
    Q_UNUSED(mainWindow);
    connect(KTextEditor::Editor::instance()->application(), &KTextEditor::Application::documentCreated,
            this, &CuttleFishPlugin::documentCreated);
    Q_FOREACH (auto doc, KTextEditor::Editor::instance()->application()->documents()) {
        documentCreated(doc);
    }
    return new QObject(this);
}

void CuttleFishPlugin::documentCreated(KTextEditor::Document* document)
{
    connect(document, &KTextEditor::Document::viewCreated,
        [this](KTextEditor::Document *document, KTextEditor::View *view) {
            Q_UNUSED(document);
            connect(view, &KTextEditor::View::contextMenuAboutToShow,
                    this, &CuttleFishPlugin::contextMenuAboutToShow);
        }
    );
}

void CuttleFishPlugin::contextMenuAboutToShow(KTextEditor::View* view, QMenu* menu)
{
    Q_UNUSED(view);
    if (m_decorated.contains(menu)) {
        return;
    }
    m_decorated << menu;
    QAction *action = new QAction(menu);
    action->setIcon(QIcon::fromTheme("cuttlefish"));
    action->setText(i18n("Insert Icon with Cuttlefish"));
    menu->addAction(action);

    connect(action, &QAction::triggered,
        [this] {
            const QString cfexe = QStandardPaths::findExecutable("cuttlefish");

            QProcess *cuttlefish = new QProcess(this);
            cuttlefish->setProgram(cfexe);
            cuttlefish->setArguments(QStringList() << "--picker");

            connect(cuttlefish, &QProcess::readyReadStandardOutput,
                [this, cuttlefish]() {
                    auto qba = cuttlefish->readAllStandardOutput();
                    auto view = KTextEditor::Editor::instance()->application()->activeMainWindow()->activeView();
                    if (view) {
                        view->document()->insertText(view->cursorPosition(), QString::fromLocal8Bit(qba));
                    }
                    cuttlefish->terminate();
                }
            );

            connect(cuttlefish, &QProcess::stateChanged,
                [cuttlefish](QProcess::ProcessState newState) {
                    if (newState == QProcess::NotRunning &&
                        KTextEditor::Editor::instance()->application()->activeMainWindow()) {

                        delete cuttlefish;
                    }
                }
            );

            cuttlefish->start();
        }
    );
}

// required for CuttleFishPluginFactory vtable
#include "cuttlefishplugin.moc"
