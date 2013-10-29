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
#include <KDebug>
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
                   const KUrl &dir,
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


void TimeLine::loadTimeLine(const KUrl &dir)
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
    QList<QHash<QString, QString> > dataList = m_gitRunner->log();
    QListIterator <QHash<QString, QString> > it(dataList);
    while (it.hasNext()) {
        QHash<QString, QString> data = it.next();

        TimeLineItem *commitItem = new TimeLineItem();

        QString subject = data["subject"];
        if (subject.contains("Merge", Qt::CaseSensitive)) {
            commitItem->setIdentifier(TimeLineItem::Merge);
        } else {
            commitItem->setIdentifier(TimeLineItem::Commit);
        }

        commitItem->setHash(data["sha1hash"]);

        QString toolTipText;
        toolTipText.append(i18n("Author: %1 \n", data["author"]));
        toolTipText.append(i18n("Date: %1 \n\n", data["date"]));
        toolTipText.append(i18n("Commid ID: %1 \n", data["sha1hash"]));
        commitItem->setToolTip(toolTipText);

        if (data == dataList.last()) {
            commitItem->setText(i18n("First Save Point"));
        } else {
            commitItem->setText(subject);
        }

        widget->addItem(commitItem);
    }
}

QStringList TimeLine::listBranches() const
{
    return m_gitRunner->branches();
}

QString TimeLine::currentBranch() const
{
    return m_gitRunner->currentBranch();
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

        // Retrieve Name and Email, and set git global parameters
        Plasma::PackageMetadata metadata(m_workingDir.pathOrUrl() + "metadata.desktop");
        m_gitRunner->setAuthor(metadata.author());
        m_gitRunner->setEmail(metadata.email());

        // Set up a basic .gitignore file
        m_gitRunner->addIgnoredFileExtension("*.*~");
        m_gitRunner->addIgnoredFileExtension("*.*bak");
        m_gitRunner->addIgnoredFileExtension("NOTES");

        // create the new repo
        m_gitRunner->init(m_workingDir.pathOrUrl());
        connect(m_gitRunner, SIGNAL(initFinished()), this, SLOT(reloadTimeLine()));

        // return and wait until the signal gets emitted.We don't want to make
        // any actions before that
        return;
    }

    if (!m_gitRunner->hasNewChangesToCommit()) {
        const QString dialog = i18n("<b>No changes have been made in order to create a savepoint.</b>");
        KMessageBox::information(this, dialog);
        return;
    }

    if (!dialogAlreadyOpen) {
        if (commitDialog->exec() == KDialog::Rejected) {
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

    m_gitRunner->add(KUrl::List(QString('.')));
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
    if (code != KMessageBox::Continue) {
        return;
    }

    QAction *sender = qobject_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->deleteCommit(data.toString());
    //loadTimeLine(m_workingDir);

    connect(m_gitRunner, SIGNAL(deleteCommitFinished()), this, SLOT(reloadTimeLine()));
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
    //loadTimeLine(m_workingDir);

    connect(m_gitRunner, SIGNAL(moveToCommitFinished()), this, SLOT(reloadTimeLine()));
    emit sourceDirectoryChanged();
}

void TimeLine::switchBranch()
{
    if (m_gitRunner->hasNewChangesToCommit()) {
        QString text = i18n("You have uncommited changes, you must commit them in order to change a branch");
        KMessageBox::information(this, text);
        return;
    }
    QString branch = senderToString();
    m_gitRunner->switchBranch(branch);
    //loadTimeLine(m_workingDir);

    connect(m_gitRunner, SIGNAL(switchBranchFinished()), this, SLOT(reloadTimeLine()));
    emit sourceDirectoryChanged();
}

void TimeLine::mergeBranch()
{
    // Prompt the user that a new save point will be created; if so,
    // popup a Savepoint dialog.
    const QString dialog = i18n("<b>You are going to combine two branches.</b>\nWith this operation, a new save point will be created; then you should have to manually resolve some conflicts on source code. Continue?");
    const int code = KMessageBox::warningContinueCancel(this,dialog);
    if (code != KMessageBox::Continue) {
        return;
    }

    CommitDialog *commitDialog = new CommitDialog();
    connect(commitDialog, SIGNAL(finished()), commitDialog, SLOT(deleteLater()));
    commitDialog->setModal(true);
    if (commitDialog->exec() == KDialog::Rejected) {
        return;
    }

    QString commit = commitDialog->briefText();
    QString optionalComment = commitDialog->fullText();

    if (!optionalComment.isEmpty()) {
        commit.append("\n\n");
        commit.append(optionalComment);
    }

    const QString branchToMerge = m_currentBranch;
    const QString branch = senderToString();

    // To merge current branch into the selected one, first whe have to
    // move to the selected branch and then call merge function !
    m_gitRunner->switchBranch(branch);
    m_gitRunner->mergeBranch(branchToMerge, commit);

    //loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::deleteBranch()
{
    const QString dialog = i18n("<b>You are going to remove the selected branch.</b>\nWith this operation, you will also delete all save points and branches performed inside it.\nContinue anyway?");
    const int code = KMessageBox::warningContinueCancel(this,dialog);
    if (code!=KMessageBox::Continue) {
        return;
    }

    const QString branch = senderToString();
    m_gitRunner->deleteBranch(branch);
    connect(m_gitRunner, SIGNAL(deleteBranchFinished()), this, SLOT(reloadTimeLine()));
    //loadTimeLine(m_workingDir);
}

void TimeLine::renameBranch()
{
    const QString branch = senderToString();

    bool ok;
    const QString newBranchName = branchDialog(&ok);

    if (!ok) {
        return;
    }

    QString dialog = i18n("Cannot rename the section: a section with this name already exists.");
    KMessageBox::information(this, dialog);
    return;

    m_gitRunner->renameBranch(newBranchName);

    connect(m_gitRunner, SIGNAL(renameBranchFinished()), this, SLOT(reloadTimeLine()));
//    loadTimeLine(m_workingDir);
}

void TimeLine::createBranch()
{
    QString branch = senderToString();

    bool ok;
    const QString newBranchName = branchDialog(&ok);
    if (!ok) {
        return;
    }

    if (listBranches().contains(newBranchName)) {
        const QString dialog = i18n("Cannot create section: a section with this name already exists.");
        KMessageBox::information(this, dialog);
        return;
    }

    m_gitRunner->newBranch(newBranchName);
    connect(m_gitRunner, SIGNAL(newBranchFinished()), this, SLOT(reloadTimeLine()));
    //loadTimeLine(m_workingDir);
}

bool TimeLine::setWorkingDir(const KUrl &dir)
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

QString TimeLine::senderToString() const
{
    QAction *sender = qobject_cast<QAction*>(this->sender());

    if (!sender) {
        return QString();
    }

    QString branch = sender->text();
    branch.remove('&');
    branch.replace(" ", "");

    return branch;
}

void TimeLine::reloadTimeLine()
{
    loadTimeLine(m_workingDir);
}

#include "moc_timeline.cpp"
