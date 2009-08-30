/******************************************************************************
 * Copyright (C) 2007 by Pino Toscano <pino@kde.org>                          *
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful, but     *
 *    WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *    General Public License for more details.                                *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program; if not, write to the Free Software Foundation  *
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA        *
 *                                                                            *
 ******************************************************************************/

#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include "dvcsjob.h"
#include "gitrunner.h"

#include "timelineprivatestorage.h"
#include "timelineitem.h"

class QWidget;
class QIcon;
class QListWidgetItem;

/**
  * This class implements a TimeLine widget,a graphical application used to easily manage git
  * repositories.
  * TimeLine is made to allow beginner developers to manage git repositories in a simple way,
  * by supplying an easy to use GUI to perform the most common actions on a git repo.
  * Since it has been designed with beginner developers in mind, the concept of commits,
  * branches, merges and the name git are kept hidden to them, and replace with more
  * comfortable terms like SavePoint, Join, Sections and of course TimeLine.
  * Using the TimeLine in your app is pretty simple: since it subclass the QWidget class, it
  * can be used as stand alone widget, or included for example in a QDockWidget, like in the
  * example below:
  *
  * @code
  *     QDockWidget *dockTimeLine = new QDockWidget(i18n("TimeLine"), this);
  *     TimeLine *timeLine = new TimeLine(this, KUrl("path/to/git/repo/"));
  *     addDockWidget(Qt::RightDockWidgetArea, dockTimeLine);
  * @endcode
  *
  * That's almost all! Only one thing remains to build a fully working TimeLine:
  * connecting its newSavePoint() slot with a signal at you choice.
  * Suppose you have a QMenu "menu" variable, you simply have to:
  *
  * @code
  *     QAction *createSavePoint = menu.addAction(i18n("Create new SavePoint"));
  *
  *     connect(createSavePoint, SIGNAL(triggered(bool)),
  *             timeLine, SLOT(newSavePoint()));
  * @endcode
  */
class TimeLine : public QWidget
{
    Q_OBJECT

public:

    /**
      * Specialized ctor. It initializes the TimeLine object with the given parameters.
      * @param parent The parent widget.
      * @param dir The path of the working directory.
      */
    TimeLine(QWidget *parent, const KUrl &dir);
    ~TimeLine();

    /**
      * Sets the item pointed by the index enabled or not.
      * @param index The index to reference the TimeLineItem object.
      * @param enabled The boolean value we want to set.
      */
    void setItemEnabled(int index, bool enabled);

    /**
      * Checks if the indexed TimeLineItem is enabled or not.
      * @param index The index to reference the TimeLineItem object.
      * @return True if the indexed element is enabled, false otherwise.
      */
    bool isItemEnabled(int index) const;

    /**
      * Marks as current (selected) element the one referenced by its index value.
      * @param index The index to reference the TimeLineItem object.
      */
    void setCurrentIndex(int index);

    /**
      * @return The index of the current index selected.
      */
    int currentIndex() const;

    /**
      * Sets the visibility state for the TimeLine widget.
      * @param visible The state we want to set.
      */
    void setTimeLineVisibility(bool visible);

    /**
      * @return True if the TimeLine is visible, false otherwise.
      */
    bool isTimeLineVisible() const;

    /**
      * Sets the workinf directory with the one passed.
      * @param dir The path of the new working directory.
      */
    void setWorkingDir(const KUrl &dir);

    /**
      * Parses the supplied path and initialize the TimeLine object.
      * @param dir The path of the new working directory.
      */
    void loadTimeLine(const KUrl &dir);


public Q_SLOTS:

    /**
      * Creates a custom menu based on the QListWidgetItem that has triggered the event.
      * @param item Reference to the event sender.
      */
    void showContextMenu(QListWidgetItem *item);

    /**
      * Creates a new SavePoint, if there are uncommitted/new files.
      */
    void newSavePoint();

Q_SIGNALS:
    /**
      * Emitted when an element is clicked.
      */
    void currentIndexChanged(int);

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
    int uiAddItem(const QIcon &icon,
                  QStringList &data,
                  const TimeLineItem::ItemIdentifier id,
                  const Qt::ItemFlag flag);

    /**
      * Initializes the GUI.
      * @param parent The parent widget.
      */
    void initUI(QWidget *parent);

    KUrl        m_workingDir;
    GitRunner   *m_gitRunner;
    QString     m_currentBranch;
    QStringList m_branches;
    TimeLinePrivateStorage *d;


};

#endif
