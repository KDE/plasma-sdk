/*
Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KParts/MainWindow>
#include <KLibLoader>
#include <KAction>
#include <kservice.h>

class QKeyEvent;
class QModelIndex;

namespace Ui
{
}

class QDockWidget;
class QStringList;


class EditPage;
class PackageModel;
class StartPage;
class Sidebar;
class TimeLine;
class MetaDataEditor;

// our own previewer
class Previewer;
class DocBrowser;
class Publisher;

namespace KTextEditor
{
    class Document;
    class View;
} // namespace KTextEditor

class MainWindow : public KParts::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QStringList recentProjects();

public Q_SLOTS:
    void quit();

    void loadProject(const QString &name, const QString &type);

    void loadRequiredEditor(const KService::List offers, KUrl target);
    void loadMetaDataEditor(KUrl target);
    void saveEditorData();
    void saveAndRefresh();
    void refreshNotes();
    // refreshes editor, killing all local changes
    void editorDestructiveRefresh();
    void selectSavePoint();
    void selectPublish();

    void toggleTimeline();
    void togglePreview();
    void toggleNotes();
    void toggleFileList();
    
    void setTimelineVisible(const bool visible);
    void setPreviewVisible(const bool visible);
    void setNotesVisible(const bool visible);
    void setFileListVisible(const bool visible);

    void openDocumentation();

signals:
    void newSavePointClicked();
    void refreshRequested();

private:
  
    KAction *m_savePointAction;
    KAction *m_timelineAction;
    KAction *m_publishAction;
    KAction *m_previewAction;
    KAction *m_notesAction;
    KAction *m_fileListAction;
    
    void setupActions();

    // QMainWindow takes control of and DELETES the previous centralWidget
    // whenever a new one is set - this is bad because we want to preserve
    // the state of the previous centralWidget for when it becomes active again.
    // This class is a workaround - we set an instance as the permanent
    // centralWidget, and use it to do graceful, non-destructive widget-switching.
    class CentralContainer : public QWidget
    {
    public:
        enum SwitchMode {
            Preserve,
            DeleteAfter
        };
        CentralContainer(QWidget* parent);
        void switchTo(QWidget* newWidget, SwitchMode mode = Preserve);
    private:
        SwitchMode m_curMode;
        QLayout *m_layout;
        QWidget *m_curWidget;
    };

    enum WorkflowTabs { StartPageTab = 0,
                        EditTab,
                        SavePoint,
                        PublishTab,
                        DocsTab,
                        PreviewTab
                      };

    void createMenus();
    void setupTextEditor(KTextEditor::Document *editorPart, KTextEditor::View *view);
    void loadNotesEditor(QDockWidget *container);
    Previewer* createPreviewerFor(const QString& projectType);
    
    void addAction(QString text, const char * icon,const  char *slot, const char *name);

    StartPage *m_startPage;
    Sidebar *m_sidebar;
    TimeLine    *m_timeLine;
    Previewer *m_previewerWidget;
    MetaDataEditor *m_metaEditor;
    Publisher *m_publisher;
    DocBrowser *m_browser;


    QDockWidget *m_editWidget;
    EditPage *m_editPage;
    PackageModel *m_model;
    int m_oldTab;
    bool m_docksCreated;
    CentralContainer *m_central;
    QString m_currentProject;
    QString m_currentProjectType;

    KParts::ReadOnlyPart *m_part;
    KParts::ReadOnlyPart *m_notesPart;
    QDockWidget *m_notesWidget;
};

#endif // MAINWINDOW_H
