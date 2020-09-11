/*
    SPDX-FileCopyrightText: 2014-2015 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cuttlefishplugin.h"

#include <KTextEditor/Application>
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>
#include <QProcess>
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
            this, &CuttleFishPlugin::viewCreated);
}

void CuttleFishPlugin::viewCreated(KTextEditor::Document *document, KTextEditor::View *view)
{
     Q_UNUSED(document);
     connect(view, &KTextEditor::View::contextMenuAboutToShow,
             this, &CuttleFishPlugin::contextMenuAboutToShow);
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
                [ cuttlefish]() {
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
