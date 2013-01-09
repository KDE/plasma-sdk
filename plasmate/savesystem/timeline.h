/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
Copyright 2009-2010 Lim Yuen Hoe <yuenhoe@hotmail.com>

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

#ifndef TIMELINE_H
#define TIMELINE_H

#include <KUrl>
#include <QDockWidget>

class GitRunner;

class QStringList;
class QTableWidgetItem;
class QWidget;
class TableWidget;
class TableDelegate;
class TimeLineItem;

class TimeLine : public QDockWidget
{
    Q_OBJECT

    /**
      * Specialized ctor. It initializes the TimeLine object with the given parameters.
      * @param parent The parent widget.
      * @param dir The path of the working directory.
      * @param The DockWidget location
      */
public:
    TimeLine(QWidget *parent,
             const KUrl &dir,
             Qt::DockWidgetArea location = Qt::BottomDockWidgetArea);
    virtual ~TimeLine();

    /**
      * Sets the workinf directory with the one passed.
      * @param dir The path of the new working directory.
      * @return returns true if the directory is a valid one.
      */
    bool setWorkingDir(const KUrl &dir);

    /**
      * Parses the supplied path and initialize the TimeLine object.
      * @param dir The path of the new working directory.
      */
    void loadTimeLine(const KUrl &dir);

    /**
      * Returns the location of the dockwidget.
      */
    Qt::DockWidgetArea location() const;


public Q_SLOTS:

    /**
      * Creates a custom menu based on the QListWidgetItem that has triggered the event.
      * @param item Reference to the event sender.
      */
    void showContextMenu(QTableWidgetItem *item);

    /**
      * Creates a new SavePoint, if there are uncommitted/new files.
      */
    void newSavePoint();

Q_SIGNALS:
    /**
      * Emitted when an element is clicked.
      */
    void currentIndexChanged(int,int);

    /**
      * Emitted when the timeline performs a switch to an other branch, or a move to
      * a different commit than HEAD; so the editor can update its current view.
      */
    void sourceDirectoryChanged();

    /**
     * Emitted when a timeline item is clicked - to notify mainwindow to
     * make preparations for a possible save point move/restore.
     */
    void savePointClicked();

private Q_SLOTS:

    /**
      * For internal use only; its triggered when the user selects to restore a commit.
      */
    void restoreCommit();

    /**
      * For internal use only; its triggered when the user selects to move to particular a commit.
      */
    void moveToCommit();

    /**
      * For internal use only; its triggered when the user selects to switch to a specific branch.
      */
    void switchBranch();

    /**
      * For internal use only; its triggered when the user selects to create a new branch.
      */
    void createBranch();

    /**
      * For internal use only; its triggered when the user selects to rename a branch.
      */
    void renameBranch();

    /**
      * For internal use only; its triggered when the user selects to merge a branch.
      */
    void mergeBranch();

    /**
      * For internal use only; its triggered when the user selects to delete a branch.
      */
    void deleteBranch();


private:
    /**
      * Stores a new item with the specified attributes.
      * @param icon The icon that will be displayed in the TimeLine
      * @param dataList A QStringList with three elements:
      *     - dataList(0) contains the name that will be displayed under the icon;
      *     - dataList(1) contains the string that will be displayed in the popup;
      *     - dataList(2) contains the sha1 hash identifier.
      * @param id The object's identifier.
      * @param flag The object's flag.
      */
    /*void uiAddItem(const QIcon &icon,
                   QStringList &data,
                   const TimeLineItem::ItemIdentifier id,
                   const Qt::ItemFlag flag);*/

    /**
      * Initializes the GUI.
      * @param parent The parent widget.
      * @param The location of the dockwidget
      */
    void initUI(QWidget *parent,
                Qt::DockWidgetArea location = Qt::TopDockWidgetArea);

    QString currentBranch() const;
    QStringList listBranches() const;

    /**
      * Method called whenever the user resizes the dockwidget or its parent
      * @param A reference to the QResizeEvent event.
      */
    void resizeEvent(QResizeEvent * event);

    QString branchDialog(bool *ok);

    KUrl        m_workingDir;
    GitRunner   *m_gitRunner;
    QString     m_currentBranch;
    QStringList m_branches;
    TableWidget *m_table;
    TableDelegate *m_delegate;
};

#endif // TIMELINE_H
