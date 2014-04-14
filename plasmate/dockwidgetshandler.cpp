/*

Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KParts/ReadWritePart>
#include <KXMLGUIFactory>
#include <KTextEditor/View>

#include <KLocalizedString>
#include <KSharedConfig>

#include "mainwindow.h"
#include "dockwidgetshandler.h"
#include "packagehandler/packagehandler.h"

#include "docbrowser/docbrowser.h"
#include "noteseditor/noteseditor.h"
#include "previewer/plasmoid/plasmoidpreviewer.h"
#include "editors/editpage.h"

DockWidgetsHandler::DockWidgetsHandler(MainWindow *parent)
    : QObject(parent),
      m_packageHandler(nullptr),
      m_mainWindow(parent)
{
    // TODO
    m_projectType = QStringLiteral("Plasma/Applet");
}

DockWidgetsHandler::~DockWidgetsHandler()
{
}

void DockWidgetsHandler::setPackageHandler(PackageHandler *packageHandler)
{
    m_packageHandler = packageHandler;
}

void DockWidgetsHandler::toggleNotes()
{
    if (!m_notesWidget) {
        m_notesWidget.reset(new NotesEditor(m_packageHandler->packagePath()));
        m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_notesWidget.data());
        return;
    }

    m_mainWindow->removeDockWidget(m_notesWidget.data());
    m_notesWidget.reset();
}

void DockWidgetsHandler::togglePreviewer()
{
    if (!m_previewerWidget) {
        Previewer *previewer = 0;

        if (m_projectType.contains("Plasma/Applet")) {
            previewer = new PlasmoidPreviewer();
        } else {
            // TODO
            ;
        }
        m_previewerWidget.reset(previewer);
        m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_previewerWidget.data());
        return;
    }

    m_mainWindow->removeDockWidget(m_previewerWidget.data());
    m_previewerWidget.reset();
}

void DockWidgetsHandler::toggleDocumentation()
{
    if (!m_docBrowserWidget) {
        m_docBrowserWidget.reset(new DocBrowser());
        m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_docBrowserWidget.data());
        return;
    }

    m_mainWindow->removeDockWidget(m_docBrowserWidget.data());
    m_docBrowserWidget.reset();
}

void DockWidgetsHandler::toggleTimeLine()
{
    // TODO
}

void DockWidgetsHandler::toggleFileList()
{
    if (!m_fileListWidget) {
        m_fileListWidget.reset(new FileList(m_packageHandler));
        m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_fileListWidget.data());

        connect(m_fileListWidget->editPage(), &EditPage::loadRequiredEditor, [=](KParts::ReadOnlyPart *editor){
            QWidget *tmpWidget = m_mainWindow->centralWidget();
            if (tmpWidget) {
                // there was another part in the mainwindow
                // find out what it is.
                //
                // It could be either a KTextEditor::View or
                // a KParts::ReadWritePart if they are their document
                // needs to be saved otherwise if its a KParts::ReadOnlyPart
                // we don't have something to do. ReadOnlyPart doesn't need
                // to save the document since its read-only

                KXMLGUIClient *client = nullptr;
                if (qobject_cast<KTextEditor::View*>(tmpWidget)) {
                    qobject_cast<KTextEditor::View*>(tmpWidget)->document()->documentSave();
                    client = dynamic_cast<KXMLGUIClient*>(tmpWidget);
                } else if (qobject_cast<KParts::ReadWritePart*>(tmpWidget)) {
                    qobject_cast<KParts::ReadWritePart*>(tmpWidget)->save();
                    client = dynamic_cast<KXMLGUIClient*>(tmpWidget);
                } else {
                    client = dynamic_cast<KXMLGUIClient*>(tmpWidget);
                }

                if (client) {
                    m_mainWindow->factory()->removeClient(client);

                    // we don't have to delete the centralWidget,
                    // MainWindow still has its ownership so it will
                    // delete it in the appropriate time
                }
            }

            KTextEditor::Document *document = qobject_cast<KTextEditor::Document*>(editor);
            if (document) {
                KTextEditor::View *view = document->views().at(0);
                m_mainWindow->factory()->addClient(view);
                m_mainWindow->setCentralWidget(view);
                return;
            }

            KParts::ReadOnlyPart *part = qobject_cast<KParts::ReadOnlyPart*>(editor);
            if (part) {
                m_mainWindow->factory()->addClient(part);
                m_mainWindow->setCentralWidget(part->widget());
            }
        });

        return;
    }

    m_mainWindow->removeDockWidget(m_fileListWidget.data());
    m_fileListWidget.reset();
}

void DockWidgetsHandler::removeAllDockWidgets()
{
    QList<QDockWidget*> l;
    l << m_notesWidget.take()
      << m_previewerWidget.take()
      << m_fileListWidget.take()
      << m_docBrowserWidget.take();

    for (auto it : l) {
        m_mainWindow->removeDockWidget(it);
        delete it;
    }
}

void DockWidgetsHandler::saveDockWidgetsState()
{
    // TODO
    const QString projectrc = m_packageHandler->packagePath() + QStringLiteral(".plasmateprojectrc");
    KConfigGroup configDocks(KSharedConfig::openConfig(projectrc), "DocksPosition");
    configDocks.writeEntry("FileList", !m_fileListWidget.isNull());
    configDocks.writeEntry("Notes", !m_notesWidget.isNull());
    configDocks.writeEntry("Previewer", !m_previewerWidget.isNull());
    configDocks.writeEntry("DocBrowser", !m_docBrowserWidget.isNull());

    configDocks.sync();
}

