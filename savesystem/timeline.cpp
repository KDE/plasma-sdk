/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/




#include <qabstractitemdelegate.h>
#include <qaction.h>
#include <qapplication.h>
#include <qevent.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpainter.h>
#include <qscrollbar.h>
#include <qsplitter.h>
#include <qstackedwidget.h>

#include <kiconloader.h>


#include <KUrl>
#include <KIcon>
#include <QListWidget>
#include <QRegExp>
#include <QMessageBox>
#include <KMenu>
#include <QMenu>
#include <QRect>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <Plasma/PackageMetadata>

#include "branchdialog.h"
#include "commitdialog.h"
#include "timelineitem.h"
#include "timelinedelegate.h"
#include "timelinelistwidget.h"
#include "timelineprivatestorage.h"
#include "timeline.h"



//using namespace KDevelop;

TimeLine::TimeLine(QWidget* parent, const KUrl& dir)
        : QWidget(parent), d(new TimeLinePrivateStorage)
{
    m_gitRunner = new GitRunner();
    initUI(parent);

    if (dir.isValid()) {
        setWorkingDir(dir);
        loadTimeLine(dir);
    }
}

TimeLine::~TimeLine()
{
    delete d;
}

int TimeLine::uiAddItem(const QIcon& icon,
                        QStringList& data,
                        const TimeLineItem::ItemIdentifier id,
                        const Qt::ItemFlag flag)
{
    TimeLineItem *newitem = new TimeLineItem(icon, data, id, flag);
    d->list->addItem(newitem);
    d->pages.append(newitem);
    // updating the minimum height of the icon view, so all are visible with no scrolling
    d->adjustListSize(false, true);
    return d->pages.count() - 1;
}

void TimeLine::loadTimeLine(const KUrl &dir)
{
    d->list->clear();
    QStringList list = QStringList();

    m_gitRunner->setDirectory(dir);
    bool res = m_gitRunner->isValidDirectory(dir);

    if (!res) {
        // If the dir hasn't a git tree we have to create only one item,
        // used to notify the user that a timeline should be created
        // ( that is, call GitRunner::init() ).
        list.append(i18n("Not a SavePoint"));
        list.append(i18n("No TimeLine found.\nClick to init the TimeLine and store your first SavePoint !"));
        list.append(QString());                  // We don't have a sha1sum now, so set an empty string

        this->uiAddItem(KIcon("document-save"),
                        list,
                        TimeLineItem::OutsideWorkingDir,
                        Qt::ItemIsEnabled);

        connect(d->list, SIGNAL(itemClicked(QListWidgetItem *)),
                this, SLOT(customContextMenuPainter(QListWidgetItem *)));
        return;
    }

    // The first element of the timeline is used to prompt the user to
    // add a SavePoint
    list.append(i18n("Not a SavePoint"));
    list.append(i18n("Click here to store a new SavePoint!"));
    list.append(QString());                  // We don't have a sha1sum now, so set an empty string
    this->uiAddItem(KIcon("document-save"),
                    list,
                    TimeLineItem::NotACommit,
                    Qt::ItemIsEnabled);


    if (m_gitRunner->currentBranch() != DvcsJob::JobSucceeded) {
        // handle error
        return;
    }

    m_currentBranch = m_gitRunner->getResult();

    if (m_gitRunner->branches() != DvcsJob::JobSucceeded) {
        // handle error
        return;
    }

    // Scan available branches,and save them
    QString branches = m_gitRunner->getResult();
    m_branches = branches.split("\n", QString::SkipEmptyParts);
    int index = m_branches.size();
    // Clean the strings
    for (int i = 0; i < index; i++) {
        QString tmp = m_branches.takeFirst();
        tmp.remove(0, 2);
        m_branches.append(tmp);
    }

    if (m_gitRunner->log(dir) != DvcsJob::JobSucceeded) {
        // handle error
        return;
    }

    QString rawCommits = m_gitRunner->getResult();

    // Split the string according with the regexp
    QRegExp rx("(commit )");
    QStringList commits = rawCommits.split(rx, QString::SkipEmptyParts);
    index = commits.size();

    // Iterate every commit and create an element in the sidebar.
    for (int i = 0; i < index; i++) {
        bool isMerge = false;

        // Save commit(index) and split it
        QStringList tmp = commits.takeFirst().split("\n");
        QString sha1sum = tmp.takeFirst();
        QString author = tmp.takeFirst();
        //QString tmpString = tmp.takeFirst();
        if (author.contains("Merge", Qt::CaseSensitive)) {
            isMerge = true;
            author = tmp.takeFirst().remove(0, 8);
        }
        //QString sha1sum = tmp.takeFirst();

        QString date = tmp.takeFirst().remove(0, 6);

        QString commitInfo = (i18n("SavePoint created on: ") + date + "\n");
        commitInfo.append(author + "\n");
        commitInfo.append(i18n("Comment:\n"));

        int tmpSize = tmp.size();
        for (int j = 0; j < tmpSize - 1; j++) {
            commitInfo.append(tmp.takeFirst().append("\n"));
            //commitInfo.append( "\n" );
            if (isMerge && (j == tmpSize - 4))
                break;
        }

        QString text = QString("SavePoint #");
        QString savePointNumber = QString();
        savePointNumber.setNum(i);
        text.append(savePointNumber);

        QStringList list = QStringList(text);
        list.append(commitInfo);
        list.append(sha1sum);
        this->uiAddItem(isMerge ? KIcon("svn_merge") : KIcon("dialog-ok"),
                        list,
                        isMerge ? TimeLineItem::Merge : TimeLineItem::Commit,
                        Qt::ItemIsEnabled);

        isMerge = false;

    }

    // As last element, show the current branch
    list.clear();
    QString info;
    info.append(i18n("On Section: "));
    info.append(m_currentBranch);
    list.append(info);

    info.clear();
    info.append(i18n("You are currently working on Section:\n"));
    info.append(m_currentBranch);
    info.append(i18n("\n"));
    info.append(i18n("\nAvailable Sections are:\n"));
    info.append(branches);
    info.append(i18n("\nClick here to switch to those Sections."));

    list.append(info);
    list.append(QString());

    this->uiAddItem(KIcon("system-switch-user"),
                    list,
                    TimeLineItem::Branch,
                    Qt::ItemIsEnabled);

    // Now the TimeLineItem can emit signal customContextMenuRequested()
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(d->list, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(customContextMenuPainter(QListWidgetItem *)));

}

void TimeLine::customContextMenuPainter(QListWidgetItem *item)
{
    TimeLineItem *tlItem = dynamic_cast<TimeLineItem*>(item);

    if (tlItem->getIdentifier() == TimeLineItem::OutsideWorkingDir) {
        // These lines are only for testing purpose !
        m_gitRunner->init(m_workingDir);

        // Retrieve Name and Email, and set git global parameters
        Plasma::PackageMetadata metadata(m_workingDir.pathOrUrl() + "metadata.desktop");

        m_gitRunner->setAuthor(metadata.author());
        m_gitRunner->setEmail(metadata.email());

        m_gitRunner->add(KUrl::List(QString(".")));

        CommitDialog *commitDialog = new CommitDialog();
        if(commitDialog->exec() == QDialog::Rejected)
            return;

        QString commit = QString(commitDialog->m_commitBriefText->text());
        QString optionalComment = QString(commitDialog->m_commitFullText->toPlainText());
        if (optionalComment.compare("") != 0) {
            commit.append("\n\n");
            commit.append(optionalComment);
        }

        m_gitRunner->commit(commit);
        d->list->disconnect();
        loadTimeLine(m_workingDir);
        return;

    } else if (tlItem->getIdentifier() == TimeLineItem::NotACommit) {
        // This line is only for testing purpose !
        m_gitRunner->add(KUrl::List(QString(".")));

        CommitDialog *commitDialog = new CommitDialog();
        if(commitDialog->exec() == QDialog::Rejected)
            return;

        QString commit = QString(commitDialog->m_commitBriefText->text());
        QString optionalComment = QString(commitDialog->m_commitFullText->toPlainText());
        if (optionalComment.compare("") != 0) {
            commit.append("\n\n");
            commit.append(optionalComment);
        }

        m_gitRunner->commit(commit);
        d->list->disconnect();
        loadTimeLine(m_workingDir);

        return;
    }

    QRect rc = d->list->visualItemRect(item);
    KMenu menu(this);
    menu.addTitle(tlItem->text());
    menu.addSeparator();

    if (tlItem->getIdentifier() == TimeLineItem::Commit || tlItem->getIdentifier() == TimeLineItem::Merge) {

        QAction *restoreCommit = menu.addAction(i18n("Restore SavePoint"));
        restoreCommit->setData(QVariant(tlItem->getHash()));
        QAction *moveToCommit = menu.addAction(i18n("Move to this SavePoint"));
        moveToCommit->setData(QVariant(tlItem->getHash()));

        connect(restoreCommit, SIGNAL(triggered(bool)),
                this, SLOT(restoreCommit()));
        connect(moveToCommit, SIGNAL(triggered(bool)),
                this, SLOT(moveToCommit()));

    } else if (tlItem->getIdentifier() == TimeLineItem::Branch) {

        QAction *createBranch = menu.addAction(i18n("Create new Section"));
        QMenu *switchBranchMenu = menu.addMenu(i18n("Switch to Section:"));
        QMenu *mergeBranchMenu = menu.addMenu(i18n("Combine into Section:"));
        QAction *renameBranch = menu.addAction(i18n("Rename current Section"));
        menu.addSeparator();
        QMenu *deleteBranchMenu = menu.addMenu(i18n("Delete Section:"));
        int index = m_branches.size();

        // Scan all the branches and, create a menu item for each item, and connect them
        for (int i = 0; i < index; i++) {
            QString tmp = m_branches.takeFirst();
            if (tmp.compare(m_currentBranch) == 0)
                continue;

            connect(switchBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(switchBranch()));
            connect(mergeBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(mergeBranch()));
            connect(deleteBranchMenu->addAction(tmp), SIGNAL(triggered(bool)),
                    this, SLOT(deleteBranch()));

            m_branches.append(tmp);
        }

        connect(createBranch, SIGNAL(triggered(bool)),
                this, SLOT(createBranch()));
        connect(renameBranch, SIGNAL(triggered(bool)),
                this, SLOT(renameBranch()));

    }


    menu.exec(mapToGlobal(rc.bottomLeft()));
}

void TimeLine::setItemEnabled(int index, bool enabled)
{
    if (index < 0 || index >= d->pages.count()) {
        return;
    }

    Qt::ItemFlags f = d->pages.at(index)->flags();
    if (enabled) {
        f |= Qt::ItemIsEnabled;
        f |= Qt::ItemIsSelectable;
    } else {
        f &= ~Qt::ItemIsEnabled;
        f &= ~Qt::ItemIsSelectable;
    }
    d->pages.at(index)->setFlags(f);

    if (!enabled && index == currentIndex()) {
        // find an enabled item, and select that one
        for (int i = 0; i < d->pages.count(); ++i)
            if (d->pages.at(i)->flags() & Qt::ItemIsEnabled) {
                setCurrentIndex(i);
                break;
            }
    }
}

void TimeLine::restoreCommit()
{
    QMessageBox warningMB;
    warningMB.setIcon(QMessageBox::Information);
    warningMB.setWindowTitle(i18n("Information"));
    warningMB.setText(i18n("You are restoring the selected SavePoint."));
    warningMB.setInformativeText(i18n("With this operation, all the SavePoints and Sections created starting from it, will be deleted.\nContinue anyway?"));
    warningMB.setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB.setDefaultButton(QMessageBox::Discard);
    if (warningMB.exec() == QMessageBox::Discard)
        return;

    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->deleteCommit(data.toString());
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::moveToCommit()
{
    QMessageBox warningMB;
    warningMB.setIcon(QMessageBox::Information);
    warningMB.setWindowTitle(i18n("Information"));
    warningMB.setText(i18n("You are going to move to the selected SavePoint."));
    warningMB.setInformativeText(i18n("To perform this, a new Section will be created and your current work may be lost if you don't have saved it as a Savepoint.\nContinue?"));
    warningMB.setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB.setDefaultButton(QMessageBox::Discard);
    if (warningMB.exec() == QMessageBox::Discard)
        return;

    BranchDialog *newBranch = new BranchDialog();

    if (newBranch->exec() == QDialog::Rejected)
        return;

    QString newBranchName = QString(newBranch->m_branchEdit->text());

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Can't rename the Section.\nReason: a Section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }


    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->moveToCommit(data.toString(), newBranchName);
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::switchBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove("&");
    m_gitRunner->switchBranch(branch);
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::mergeBranch()
{
    // Prompt the user that a new SavePoint will be created; if so,
    // popup a Savepoint dialog.
    QMessageBox warningMB;
    warningMB.setIcon(QMessageBox::Information);
    warningMB.setWindowTitle(i18n("Information"));
    warningMB.setText(i18n("You are going to combine two Sections."));
    warningMB.setInformativeText(i18n("With this operation, a new SavePoint will be created; then you should have to manually resolve some conflicts on source code. Continue?"));
    warningMB.setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB.setDefaultButton(QMessageBox::Discard);
    if (warningMB.exec() == QMessageBox::Discard)
        return;

    CommitDialog *commitDialog = new CommitDialog();
    if (commitDialog->exec() == QDialog::Rejected)
        return;

    QString commit = QString(commitDialog->m_commitBriefText->text());
    QString optionalComment = QString(commitDialog->m_commitFullText->toPlainText());
    if (optionalComment.compare("") != 0) {
        commit.append("\n\n");
        commit.append(optionalComment);
    }

    QString branchToMerge = m_currentBranch;
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove("&");

    // To merge current branch into the selected one, first whe have to
    // move to the selected branch and then call merge function !
    m_gitRunner->switchBranch(branch);
    m_gitRunner->mergeBranch(branchToMerge, commit);

    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::deleteBranch()
{
    QMessageBox warningMB;
    warningMB.setIcon(QMessageBox::Warning);
    warningMB.setWindowTitle(i18n("Warning"));
    warningMB.setText(i18n("<b>You are going to remove the selected Section.</b>"));
    warningMB.setInformativeText(i18n("With this operation, you'll also delete all SavePoints/Sections performed inside it.\nContinue anyway?"));
    warningMB.setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB.setDefaultButton(QMessageBox::Discard);
    if (warningMB.exec() == QMessageBox::Discard)
        return;

    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove("&");
    m_gitRunner->deleteBranch(branch);
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::renameBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove("&");

    BranchDialog *renameBranch = new BranchDialog();

    if (renameBranch->exec() == QDialog::Rejected) {
        return;
    }

    QString newBranchName = QString(renameBranch->m_branchEdit->text());

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Can't rename the Section.\nReason: a Section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }

    m_gitRunner->renameBranch(newBranchName);
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::createBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove("&");

    BranchDialog *createBranch = new BranchDialog();

    if (createBranch->exec() == QDialog::Rejected) {
        return;
    }

    QString newBranchName = QString(createBranch->m_branchEdit->text());

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Can't create the Section.\nReason: a Section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }

    m_gitRunner->newBranch(newBranchName);
    d->list->disconnect();
    loadTimeLine(m_workingDir);
}

bool TimeLine::isItemEnabled(int index) const
{
    if (index < 0 || index >= d->pages.count()) {
        return false;
    }

    Qt::ItemFlags f = d->pages.at(index)->flags();
    return (f & Qt::ItemIsEnabled) == Qt::ItemIsEnabled;
}

void TimeLine::setCurrentIndex(int index)
{
    if (index < 0 || index >= d->pages.count() || !isItemEnabled(index)) {
        return;
    }

    QModelIndex modelindex = d->list->model()->index(index, 0);
    d->list->setCurrentIndex(modelindex);
    d->list->selectionModel()->select(modelindex, QItemSelectionModel::ClearAndSelect);
}

int TimeLine::currentIndex() const
{
    return d->list->currentRow();
}

void TimeLine::setTimeLineVisibility(bool visible)
{
    if (visible != d->list->isHidden())
        return;

    static bool sideWasVisible = !d->sideContainer->isHidden();

    d->list->setHidden(!visible);
    if (visible) {
        d->sideContainer->setHidden(!sideWasVisible);

        sideWasVisible = true;
    } else {
        sideWasVisible = !d->sideContainer->isHidden();
        d->sideContainer->setHidden(true);
    }
}

bool TimeLine::isTimeLineVisible() const
{
    return !d->sideContainer->isHidden();
}

void TimeLine::setWorkingDir(const KUrl &dir)
{
    m_workingDir = dir;
}

void TimeLine::initUI(QWidget *parent)
{
    QVBoxLayout *mainlay = new QVBoxLayout(this);
    mainlay->setMargin(0);
    mainlay->setSpacing(0);

    d->list = new TimeLineListWidget(this);
    mainlay->addWidget(d->list);
    d->list->setMouseTracking(true);
    d->list->viewport()->setAttribute(Qt::WA_Hover);
    d->sideDelegate = new TimeLineDelegate(d->list);
    d->list->setItemDelegate(d->sideDelegate);
    d->list->setUniformItemSizes(true);
    d->list->setSelectionMode(QAbstractItemView::SingleSelection);
    int iconsize = 32;// Okular::Settings::timelineIconSize();
    d->list->setIconSize(QSize(iconsize, iconsize));
    d->list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->list->setContextMenuPolicy(Qt::CustomContextMenu);
    d->list->viewport()->setAutoFillBackground(false);

    d->splitter = new QSplitter(this);
    mainlay->addWidget(d->splitter);
    d->splitter->setOpaqueResize(true);
    d->splitter->setChildrenCollapsible(true);

    d->sideContainer = new QWidget(d->splitter);
    d->sideContainer->setMinimumWidth(90);
    d->sideContainer->setMinimumHeight(90);
    d->sideContainer->setMaximumWidth(600);
    d->vlay = new QHBoxLayout(d->sideContainer);
    d->vlay->setMargin(0);

    d->sideTitle = new QLabel(d->sideContainer);
    d->vlay->addWidget(d->sideTitle);
    QFont tf = d->sideTitle->font();
    tf.setBold(true);
    d->sideTitle->setFont(tf);
    d->sideTitle->setMargin(3);
    d->sideTitle->setIndent(3);

    d->stack = new QStackedWidget(d->sideContainer);
    d->vlay->addWidget(d->stack);
    d->sideContainer->hide();

    //connect( d->list, SIGNAL( currentRowChanged( int ) ),
    //       this, SIGNAL( currentIndexChanged( int ) ) );
}

#include "timeline.moc"
