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


#include <KParts/MainWindow>
#include <KLocalizedString>

#include "dockwidgetshandler.h"
#include "packagehandler.h"
#include "noteseditor/noteseditor.h"
#include "previewer/plasmoid/plasmoidpreviewer.h"
#include "editors/editpage.h"

DockWidgetsHandler::DockWidgetsHandler(PackageHandler *packageHandler, KParts::MainWindow *parent)
    : QObject(parent),
      m_packageHandler(packageHandler),
      m_mainWindow(parent)
{
    // TODO
    m_projectType = QStringLiteral("Plasma/Applet");
}

DockWidgetsHandler::~DockWidgetsHandler()
{
}

void DockWidgetsHandler::toggleNotes()
{
    if (!m_notesWidget) {
        m_notesWidget.reset(new NotesEditor(QString()));
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
    // TODO
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
      << m_fileListWidget.take();

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

    configDocks.sync();
}

