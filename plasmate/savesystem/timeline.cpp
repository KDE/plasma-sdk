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

#include <KAction>
#include <QDebug>
#include <KConfig>
#include <KLocale>
#include <KMenu>
#include <KMessageBox>
#include <KInputDialog>
#include <Plasma/PackageMetadata>


#include <QDateTime>
#include <QDockWidget>
#include <QPoint>
#include <QPointer>
#include <QRect>
#include <QRegExp>
#include <QResizeEvent>
#include <QStringList>

#include "timeline.h"
#include "tablewidget.h"
#include "tabledelegate.h"
#include "timelineitem.h"
#include "dvcsjob.h"
#include "gitrunner.h"
#include "commitdialog.h"

TimeLine::TimeLine(QWidget *parent,
                   const QUrl &dir,
                   Qt::DockWidgetArea location)
        :QDockWidget(i18n("TimeLine"))
{
    m_gitRunner = new GitRunner();
    initUI(parent, location);

    setWorkingDir(dir);
}

TimeLine::~TimeLine()
{
    delete m_gitRunner;
    delete m_table;
}


void TimeLine::loadTimeLine(const QUrl &dir)
{
    m_table->clear();

    setWorkingDir(dir);

    if (!m_gitRunner->isValidDirectory()) {
        newSavePoint();
        return;
    }

    m_branches = listBranches();

    m_currentBranch = currentBranch();

    TimeLineItem *branchItem = new TimeLineItem();
    branchItem->setText(i18n("On branch: %1", m_currentBranch));

    QString branchToolTip = i18n("You are currently working on branch:\n"
                                 "%1\n"
                                 "\n"
                                 "Available branches are:\n"
                                 "%2\n"
                                 "Click here to switch to those branches.",
                                 m_currentBranch,
                                 m_branches.join("\n")
                                 );
    branchItem->setToolTip(branchToolTip);

    branchItem->setIdentifier(TimeLineItem::Branch);
    m_table->addItem(branchItem);

    TimeLineItem *saveItem = new TimeLineItem();
    saveItem->setText(i18n("New save point"));

    saveItem->setIdentifier(TimeLineItem::NotACommit);
    m_table->addItem(saveItem);

    identifyCommits(m_table);

    parentWidget()->show();
}

void TimeLine::identifyCommits(TableWidget *widget)
{

    // Log gets the full git commit list
    if (m_gitRunner->log() != DvcsJob::JobSucceeded) {
        // handle error
        return;
    }

    const QString rawCommits = m_gitRunner->getResult();
    const QStringList commitLog = rawCommits.split('\n',QString::SkipEmptyParts);

    // Regexp to match the sha1hash from the git commits.
    const QString regExp("(commit\\s[0-9a-f]{40})");
    const QRegExp rx(regExp);

    TimeLineItem *commitItem = NULL;
    int logIndex = 0;

    // Iterate every commit log line. the newest commits are on the top of the log
    while ( logIndex < commitLog.size()) {
        QString toolTipText;

        // here we got a sha1hash, hence a new commit beginns
        if (commitLog.at(logIndex).contains(rx)) {

            commitItem = new TimeLineItem();

            // set the git hash
            commitItem->setHash(commitLog.at(logIndex).right(40)); // FIXME: As long as the hash has 40 chars it works.
            ++logIndex;

            if (commitLog.at(logIndex).contains("Merge: ", Qt::CaseSensitive)) {
                commitItem->setIdentifier(TimeLineItem::Merge);
                ++logIndex;
            } else {
                commitItem->setIdentifier(TimeLineItem::Commit);
            }

            // The next line is the author
            toolTipText = commitLog.at(logIndex) + "\n\n";
            ++logIndex;

            toolTipText.replace("Author", i18n("Author"),
                                Qt::CaseSensitive);

            // Then comes the date
            QString date = commitLog.at(logIndex);
            ++logIndex;

            date.remove("Date: ",Qt::CaseSensitive);
            toolTipText.prepend(i18n("Created on: %1", date) + "\n");

            //Our format date is Tue Nov 6 23:17:56 2012 +0200"
            QStringList dateList = date.split(" ", QString::SkipEmptyParts);
            QDateTime tmpDateTime = QDateTime::fromString(dateList.at(2) + " " + dateList.at(1) + " " + dateList.at(4) +
                                    " " +  dateList.at(3), "d MMM yyyy hh:mm:ss");

            QString localeTime = KGlobal::locale()->formatDateTime(tmpDateTime, KLocale::LongDate);
            commitItem->setText(localeTime);
        }

        // The rest is Commit log info
        while (logIndex < commitLog.size()) {
            if (commitLog.at(logIndex).contains(rx)) {
                break;
            }
            toolTipText.append(commitLog.at(logIndex) + "\n");
            ++logIndex;
        }

        commitItem->setToolTip(toolTipText);


        //check if this is the last item
        if (logIndex + 1  < commitLog.size()) {
            // The last Item is maked as such.
            commitItem->setText(i18n("First save point"));
        }

        widget->addItem(commitItem);
    }
}

QStringList TimeLine::listBranches() const
{
    QStringList branchList;
    if (m_gitRunner->branches() != DvcsJob::JobSucceeded) {
        // handle error
        return branchList;
    }
    // Scan available branches,and save them
    const QString branches = m_gitRunner->getResult();

    foreach (QString branch, branches.split('\n', QString::SkipEmptyParts)) {
        branch.remove(0, 2); // Clean the string form *
        branchList.append(branch);
    }
    return branchList;
}

QString TimeLine::currentBranch() const
{
    if (m_gitRunner->currentBranch() != DvcsJob::JobSucceeded) {
        return QString();
    }
    return m_gitRunner->getResult();
}

Qt::DockWidgetArea TimeLine::location() const
{
    return m_table->location();
}

void TimeLine::showContextMenu(QTableWidgetItem *item)
{
    emit savePointClicked();
    TimeLineItem *tlItem = dynamic_cast<TimeLineItem*>(item);

    if (!tlItem) {
        return;
    }

    KMenu menu(this);
    menu.addTitle(tlItem->toolTip());
    menu.addSeparator();

    switch (tlItem->getIdentifier()) {
    case TimeLineItem::Commit:
    case TimeLineItem::Merge:
    {
        QAction *restoreCommit = menu.addAction(i18n("Restore save point"));
        restoreCommit->setData(QVariant(tlItem->getHash()));
        QAction *moveToCommit = menu.addAction(i18n("Move to this save point"));
        moveToCommit->setData(QVariant(tlItem->getHash()));
        connect(restoreCommit, SIGNAL(triggered(bool)),
                this, SLOT(restoreCommit()));
        connect(moveToCommit, SIGNAL(triggered(bool)),
                this, SLOT(moveToCommit()));
        break;
    }
    case TimeLineItem::Branch:
    {
        QAction *createBranch = menu.addAction(i18n("Create new branch"));
        QMenu *switchBranchMenu = menu.addMenu(i18n("Switch to branch"));
        QMenu *mergeBranchMenu = menu.addMenu(i18n("Combine into branch"));
        QAction *renameBranch = menu.addAction(i18n("Rename current branch"));

        menu.addSeparator();
        QMenu *deleteBranchMenu = menu.addMenu(i18n("Delete branch"));
        const int index = m_branches.size();

        // Scan all the branches and, create a menu item for each item, and connect them
        for (int i = 0; i < index; ++i) {
            const QString tmp = m_branches.value(i);
            if (tmp.compare(m_currentBranch) == 0) {
                continue;
            }

            connect(switchBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(switchBranch()));
            connect(mergeBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(mergeBranch()));
            connect(deleteBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(deleteBranch()));
        }

        connect(createBranch, SIGNAL(triggered(bool)),
                this, SLOT(createBranch()));
        connect(renameBranch, SIGNAL(triggered(bool)),
                this, SLOT(renameBranch()));

        switchBranchMenu->setEnabled(!switchBranchMenu->actions().isEmpty());
        mergeBranchMenu->setEnabled(!mergeBranchMenu->actions().isEmpty());
        deleteBranchMenu->setEnabled(!deleteBranchMenu->actions().isEmpty());
        renameBranch->setEnabled(m_currentBranch != "master");
        break;
    }
    case TimeLineItem::NotACommit:
    {
        newSavePoint();
        return;
    }
    case TimeLineItem::OutsideWorkingDir:
        break;
    }

    QRect rc = m_table->visualItemRect(tlItem);
    menu.exec(mapToGlobal(rc.bottomLeft()));
}

void TimeLine::newSavePoint()
{
    QString commitMessage;
    CommitDialog *commitDialog = new CommitDialog();
    commitDialog->setModal(true);
    connect(commitDialog, SIGNAL(finished()), commitDialog, SLOT(deleteLater()));

    bool dialogAlreadyOpen = false;
    if (!m_gitRunner->isValidDirectory()) {
        dialogAlreadyOpen = true;

        commitMessage = i18n("Initial Commit");

        m_gitRunner->init(m_workingDir);
        // Retrieve Name and Email, and set git global parameters
        Plasma::PackageMetadata metadata(m_workingDir.path() + "metadata.desktop");
        m_gitRunner->setAuthor(metadata.author());
        m_gitRunner->setEmail(metadata.email());

        // Set up a basic .gitignore file
        m_gitRunner->addIgnoredFileExtension("*.*~");
        m_gitRunner->addIgnoredFileExtension("*.*bak");
        m_gitRunner->addIgnoredFileExtension("NOTES");
    }

    if (!m_gitRunner->hasNewChangesToCommit()) {
        const QString dialog = i18n("<b>No changes have been made in order to create a savepoint.</b>");
        KMessageBox::information(this, dialog);
        return;
    }

    if (!dialogAlreadyOpen) {
        if (commitDialog->exec() == QDialog::Rejected) {
            return;
        }

        commitMessage = commitDialog->briefText();
    }

    // Ensure the required comment is not empty
    if (commitMessage.isEmpty()) {
        return;
    }

    QString optionalComment = commitDialog->fullText();

    if (!optionalComment.isEmpty() != 0) {
        commitMessage.append("\n\n");
        commitMessage.append(optionalComment);
    }

    m_gitRunner->add(QUrl::List(QString('.')));
    m_gitRunner->commit(commitMessage);
    loadTimeLine(m_workingDir);
    if (isHidden()) {
        show();
    }
}

void TimeLine::restoreCommit()
{
    const QString dialog = (i18n("<b>You are restoring the selected save point.</b>\nWith this operation, all the save points and branches created starting from it, will be deleted.\nContinue anyway?"));
    const int code = KMessageBox::warningContinueCancel(this, dialog);
    if (code!=KMessageBox::Continue) {
        return;
    }

    QAction *sender = qobject_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->deleteCommit(data.toString());
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::moveToCommit()
{
    QString dialog = i18n("<b>You are going to move to the selected save point.</b>\nTo perform this, a new branch will be created and your current work may be lost if you do not have saved it as a Savepoint.\nContinue?");
    const int code = KMessageBox::warningContinueCancel(this,dialog);
    if (code!=KMessageBox::Continue) {
        return;
    }

    bool ok;
    const QString newBranchName = branchDialog(&ok);

    if (!ok) {
        return;
    }

    if (m_branches.contains(newBranchName)) {
        dialog = i18n("Cannot rename the section: a section with this name already exists.");
        KMessageBox::information(this, dialog);
        return;
    }

    // Retrieve data from the sender
    QAction *sender = qobject_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->moveToCommit(data.toString(), newBranchName);
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::switchBranch()
{
    QAction *sender = qobject_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');
    m_gitRunner->switchBranch(branch);
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::mergeBranch()
{
    // Prompt the user that a new save point will be created; if so,
    // popup a Savepoint dialog.
    const QString dialog = i18n("<b>You are going to combine two branches.</b>\nWith this operation, a new save point will be created; then you should have to manually resolve some conflicts on source code. Continue?");
    const int code = KMessageBox::warningContinueCancel(this,dialog);
    if (code!=KMessageBox::Continue) {
        return;
    }

    CommitDialog *commitDialog = new CommitDialog();
    connect(commitDialog, SIGNAL(finished()), commitDialog, SLOT(deleteLater()));
    commitDialog->setModal(true);
    if (commitDialog->exec() == QDialog::Rejected) {
        return;
    }

    QString commit = commitDialog->briefText();
    QString optionalComment = commitDialog->fullText();

    if (!optionalComment.isEmpty()) {
        commit.append("\n\n");
        commit.append(optionalComment);
    }

    QString branchToMerge = m_currentBranch;
    QAction *sender = qobject_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');

    // To merge current branch into the selected one, first whe have to
    // move to the selected branch and then call merge function !
    m_gitRunner->switchBranch(branch);
    m_gitRunner->mergeBranch(branchToMerge, commit);

    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::deleteBranch()
{
    const QString dialog = i18n("<b>You are going to remove the selected branch.</b>\nWith this operation, you will also delete all save points and branches performed inside it.\nContinue anyway?");
    const int code = KMessageBox::warningContinueCancel(this,dialog);
    if (code!=KMessageBox::Continue) {
        return;
    }

    QAction *sender = qobject_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');
    m_gitRunner->deleteBranch(branch);
    loadTimeLine(m_workingDir);
}

void TimeLine::renameBranch()
{
    QAction *sender = qobject_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');


    bool ok;
    const QString newBranchName = branchDialog(&ok);

    if (!ok) {
        return;
    }

    QString dialog = i18n("Cannot rename the section: a section with this name already exists.");
    KMessageBox::information(this, dialog);
    return;

    m_gitRunner->renameBranch(newBranchName);
    loadTimeLine(m_workingDir);
}

void TimeLine::createBranch()
{
    QAction *sender = qobject_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');

    bool ok;
    const QString newBranchName = branchDialog(&ok);

    if (!ok) {
        return;
    }

    const QString dialog = i18n("Cannot create section: a section with this name already exists.");
    KMessageBox::information(this, dialog);
    return;

    m_gitRunner->newBranch(newBranchName);
    loadTimeLine(m_workingDir);
}

bool TimeLine::setWorkingDir(const QUrl &dir)
{
    if (dir.isValid()) {
        m_gitRunner->setDirectory(dir);
        m_workingDir = dir;
        return true;
    }

    // If the dir hasn't a git tree, don't show nothig!
    hide();
    return false;
}

void TimeLine::initUI(QWidget *parent,Qt::DockWidgetArea location)
{
    Q_UNUSED(parent)

    setFeatures(QDockWidget::AllDockWidgetFeatures);
    m_table = new TableWidget(location);
    m_table->setParent(this);
    setWidget(m_table);

    m_delegate = new TableDelegate();
    m_table->setItemDelegate(m_delegate);

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
            m_table,SLOT(updateLayout(Qt::DockWidgetArea)));
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
            this, SLOT(showContextMenu(QTableWidgetItem *)));
}

void TimeLine::resizeEvent (QResizeEvent * event)
{
    /*
      I've reimplemented this event handler because, when resizing the main widget to a size
      smaller than the dockwidget, a bothersome effects appears. According with the orientation,
      it appears a scrollbar INSIDE the m_table widget, which hovers partially the icons when
      the dock is vertical, and hovers completely the text when it is horizontal.
      So I've implemented this simple workaround, in order to expand the dockwidget when a
      scrollbar is shown.
      I know that resizing a child widget inside a parent resizeEvent() is dangerous at 98%,
      by the way this trick is very simple and well structured, so it wont cause issues I hope =)
      */

    QSize newSize = event->size();
    //qDebug() << "New size: " << newSize;
    //qDebug() << "m_table size:" << m_table->size();
    //qDebug() << "m_table total length" << m_table->totalLenght();

    bool vertical = ((m_table->location() == Qt::RightDockWidgetArea) ||
                     (m_table->location() == Qt::LeftDockWidgetArea));

    if (vertical) {
        if (m_table->totalLenght() > newSize.height()) {
            m_table->setFixedWidth(m_table->columnWidth(0) + m_table->scrollbarSize(vertical).width());
            return;
        } else {
            if (m_table->columnWidth(0) < newSize.width()) {
                m_table->setFixedWidth(m_table->columnWidth(0));
                return;
            }
        }
    } else {
        if (m_table->totalLenght() > newSize.width()) {
            m_table->setFixedHeight(m_table->rowHeight(0) + m_table->scrollbarSize(vertical).height());
            return;
        } else {
            if (m_table->rowHeight(0) < newSize.height()) {
                m_table->setFixedHeight(m_table->rowHeight(0));
                return;
            }
        }
    }
}

QString TimeLine::branchDialog(bool *ok)
{
    QValidator *validator = new QRegExpValidator(QRegExp("[a-zA-Z0-9_.]*"));

    return KInputDialog::getText(i18n("New Branch"), i18n("New branch name:"), "type here", ok, this, validator);
}

#include "moc_timeline.cpp"
