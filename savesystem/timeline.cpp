#include <KAction>
#include <KConfig>
#include <KIcon>
#include <KMenu>
#include <Plasma/PackageMetadata>

#include <QDockWidget>
#include <QMessageBox>
#include <QPoint>
#include <QPointer>
#include <QRect>
#include <QRegExp>
#include <QResizeEvent>
#include <QStringList>


//#include <QDebug>

#include "timeline.h"

#include "tablewidget.h"
#include "tabledelegate.h"
#include "timelineitem.h"
#include "dvcsjob.h"
#include "gitrunner.h"
#include "branchdialog.h"
#include "commitdialog.h"

TimeLine::TimeLine(QWidget *parent,
                   const KUrl &dir,
                   Qt::DockWidgetArea location)
                   :QDockWidget(i18n("TimeLine"))
{
    m_gitRunner = new GitRunner();
    initUI(parent, location);

    if (dir.isValid()) {
        setWorkingDir(dir);
        //loadTimeLine(dir);
    }
}

void TimeLine::loadTimeLine(const KUrl &dir)
{
    m_table->clear();
    QStringList list = QStringList();

    m_gitRunner->setDirectory(dir);

    if (!m_gitRunner->isValidDirectory()) {
        // If the dir hasn't a git tree, don't show nothig!
        hide();
        return;
    }

    setWorkingDir(dir);

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
    m_branches = branches.split('\n', QString::SkipEmptyParts);
    int index = m_branches.size();
    // Clean the strings
    for (int i = 0; i < index; i++) {
        QString tmp = m_branches.takeFirst();
        tmp.remove(0, 2);
        m_branches.append(tmp);
    }

    if (m_gitRunner->log() != DvcsJob::JobSucceeded) {
        // handle error
        return;
    }

    QString info;
    info.append(i18n("On Section: "));
    info.append(m_currentBranch);
    list.append(info);

    info.clear();
    info.append(i18n("You are currently working on Section:\n"));
    info.append(m_currentBranch);
    info.append('\n');
    info.append(i18n("\nAvailable Sections are:\n"));
    info.append(branches);
    info.append(i18n("\nClick here to switch to those Sections."));

    list.append(info);
    list.append(QString());

    /*this->uiAddItem(KIcon("system-switch-user"),
                    list,
                    TimeLineItem::Branch,
                    Qt::ItemIsEnabled);*/
    TimeLineItem *item = new TimeLineItem(KIcon("system-switch-user"),
                                          list,
                                          TimeLineItem::Branch,
                                          Qt::ItemIsEnabled);
    m_table->addItem(item);

    list.clear();

    const QString rawCommits = m_gitRunner->getResult();

    // Split the string according with the regexp
    const QString regExp("(commit\\s[0-9a-f]{40})");
    const QRegExp rx(regExp);

    const QStringList commitList = rawCommits.split('\n',QString::SkipEmptyParts);

    index = 0;
    int commitIndex = 0;

    // Iterate every commit and create an element in the sidebar.
    while(index < commitList.size()) {
        bool isMerge = false;
        QStringList list;
        QString commitInfo;
        QString sha1hash;
        QString text;
        QString author;
        QString date;

        if(commitList.at(index).contains(rx)) {
            const QString commit("commit ");
            // We can do this because now we are sure that the indexed string contains the sha1 hash =)
            sha1hash = commitList.at(index);
            list = sha1hash.split(commit,
                              QString::SkipEmptyParts,
                              Qt::CaseSensitive);

            sha1hash = list.at(0);
            list.clear();
            ++index;

            if(commitList.at(index).contains("Merge: ", Qt::CaseSensitive)) {
                    isMerge = true;
                    ++index;
            }
            // Now check the "Author" field
            author = commitList.at(index);
            author = author.replace("Author",
                                i18n("Author"),
                                Qt::CaseSensitive);
            author.append("\n\n");
            ++index;
            // Check the "Date" field
            date = commitList.at(index);
            date = date.remove("Date",
                                  Qt::CaseSensitive);
            date.append('\n');
            ++index;
        }

        // Ok, start grouping the needed infos
        commitInfo.append(i18n("Comment:\n\n"));
        while(index < commitList.size()) {
            if(commitList.at(index).contains(rx)) {
                break;
            }
            commitInfo.append(commitList.at(index));
            commitInfo.append("\n");
            ++index;
        }

        // Put all together
        date.prepend(i18n("SavePoint created on: "));

        commitInfo.prepend(author);
        commitInfo.prepend(date);

        text.setNum(commitIndex);
        text.prepend("SavePoint #");
        list.append(text);
        list.append(commitInfo);
        list.append(sha1hash);

        TimeLineItem *item1 = new TimeLineItem(isMerge ? KIcon("svn_merge") : KIcon("dialog-ok"),
                                               list,
                                               isMerge ? TimeLineItem::Merge : TimeLineItem::Commit,
                                               Qt::ItemIsEnabled);
        m_table->addItem(item1);

        isMerge = false;
        ++commitIndex;
    }

    parentWidget()->show();
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)),
            this, SLOT(showContextMenu(QTableWidgetItem *)));
}

Qt::DockWidgetArea TimeLine::location()
{
    return m_table->location();
}

void TimeLine::showContextMenu(QTableWidgetItem *item)
{
    TimeLineItem *tlItem = dynamic_cast<TimeLineItem*>(item);
    //kDebug() << table->currentItem() << tlItem;

    if (!tlItem) {
        return;
    }

    QRect rc = m_table->visualItemRect(tlItem);
    KMenu menu(this);
    menu.addTitle(tlItem->text());
    menu.addSeparator();

    if (tlItem->getIdentifier() == TimeLineItem::Commit ||
        tlItem->getIdentifier() == TimeLineItem::Merge) {

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
        QMenu *switchBranchMenu = menu.addMenu(i18n("Switch to Section"));
        QMenu *mergeBranchMenu = menu.addMenu(i18n("Combine into Section"));
        QAction *renameBranch = menu.addAction(i18n("Rename current Section"));
        menu.addSeparator();
        QMenu *deleteBranchMenu = menu.addMenu(i18n("Delete Section"));
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
    }

    menu.exec(mapToGlobal(rc.bottomLeft()));
}

void TimeLine::newSavePoint()
{
    QPointer<CommitDialog> commitDialog = new CommitDialog();
    bool dialogAlreadyOpen = false;
    if(!m_gitRunner->isValidDirectory()) {

        if(!m_gitRunner->hasNewChangesToCommit())
            return;
        dialogAlreadyOpen = true;

        if(commitDialog->exec() == QDialog::Rejected)
            return;

        m_gitRunner->init(m_workingDir);
        // Retrieve Name and Email, and set git global parameters
        Plasma::PackageMetadata metadata(m_workingDir.pathOrUrl() + "metadata.desktop");
        m_gitRunner->setAuthor(metadata.author());
        m_gitRunner->setEmail(metadata.email());

        // Set up a basic .gitignore file
        m_gitRunner->addIgnoredFileExtension("*.*~");
        m_gitRunner->addIgnoredFileExtension("*.*bak");
    }

    if(!m_gitRunner->hasNewChangesToCommit())
        return;

    if(!dialogAlreadyOpen) {
        if(commitDialog->exec() == QDialog::Rejected)
            return;
    }
    QString commit = QString(commitDialog->m_commitBriefText->text());
    // Ensure the required comment is not empty
    if(commit.isEmpty())
        return;

    QString optionalComment = QString(commitDialog->m_commitFullText->toPlainText());
    delete commitDialog;

    if (!optionalComment.isEmpty() != 0) {
        commit.append("\n\n");
        commit.append(optionalComment);
    }

    m_gitRunner->add(KUrl::List(QString('.')));
    m_gitRunner->commit(commit);
    m_table->disconnect();
    loadTimeLine(m_workingDir);
    if(isHidden())
        show();
}

void TimeLine::restoreCommit()
{
    QPointer<QMessageBox> warningMB = new QMessageBox(0);
    warningMB->setIcon(QMessageBox::Information);
    warningMB->setWindowTitle(i18n("Information"));
    warningMB->setText(i18n("You are restoring the selected SavePoint."));
    warningMB->setInformativeText(i18n("With this operation, all the SavePoints and Sections created starting from it, will be deleted.\nContinue anyway?"));
    warningMB->setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB->setDefaultButton(QMessageBox::Discard);
    if (warningMB->exec() == QMessageBox::Discard)
        return;

    delete warningMB;

    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->deleteCommit(data.toString());
    m_table->disconnect();
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::moveToCommit()
{
    QPointer<QMessageBox> warningMB = new QMessageBox(0);
    warningMB->setIcon(QMessageBox::Information);
    warningMB->setWindowTitle(i18n("Information"));
    warningMB->setText(i18n("You are going to move to the selected SavePoint."));
    warningMB->setInformativeText(i18n("To perform this, a new Section will be created and your current work may be lost if you don't have saved it as a Savepoint.\nContinue?"));
    warningMB->setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB->setDefaultButton(QMessageBox::Discard);
    if (warningMB->exec() == QMessageBox::Discard)
        return;

    delete warningMB;

    QPointer<BranchDialog> newBranch = new BranchDialog();
    if (newBranch->exec() == QDialog::Rejected)
        return;

    QString newBranchName = QString(newBranch->m_branchEdit->text());
    delete newBranch;

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Cannot rename the section: a section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }

    // Retrieve data from the sender
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QVariant data = sender->data();
    m_gitRunner->moveToCommit(data.toString(), newBranchName);
    m_table->disconnect();
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::switchBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');
    m_gitRunner->switchBranch(branch);
    m_table->disconnect();
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::mergeBranch()
{
    // Prompt the user that a new SavePoint will be created; if so,
    // popup a Savepoint dialog.
    QPointer<QMessageBox> warningMB = new QMessageBox(0);
    warningMB->setIcon(QMessageBox::Information);
    warningMB->setWindowTitle(i18n("Information"));
    warningMB->setText(i18n("You are going to combine two Sections."));
    warningMB->setInformativeText(i18n("With this operation, a new SavePoint will be created; then you should have to manually resolve some conflicts on source code. Continue?"));
    warningMB->setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB->setDefaultButton(QMessageBox::Discard);
    if (warningMB->exec() == QMessageBox::Discard)
        return;

    delete warningMB;

    QPointer<CommitDialog> commitDialog = new CommitDialog();
    if (commitDialog->exec() == QDialog::Rejected)
        return;

    QString commit = QString(commitDialog->m_commitBriefText->text());
    QString optionalComment = QString(commitDialog->m_commitFullText->toPlainText());
    delete commitDialog;

    if (!optionalComment.isEmpty()) {
        commit.append("\n\n");
        commit.append(optionalComment);
    }

    QString branchToMerge = m_currentBranch;
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');

    // To merge current branch into the selected one, first whe have to
    // move to the selected branch and then call merge function !
    m_gitRunner->switchBranch(branch);
    m_gitRunner->mergeBranch(branchToMerge, commit);

    m_table->disconnect();
    loadTimeLine(m_workingDir);

    emit sourceDirectoryChanged();
}

void TimeLine::deleteBranch()
{
    QPointer<QMessageBox> warningMB = new QMessageBox(0);
    warningMB->setIcon(QMessageBox::Warning);
    warningMB->setWindowTitle(i18n("Warning"));
    warningMB->setText(i18n("<b>You are going to remove the selected Section.</b>"));
    warningMB->setInformativeText(i18n("With this operation, you'll also delete all SavePoints/Sections performed inside it.\nContinue anyway?"));
    warningMB->setStandardButtons(QMessageBox::Ok | QMessageBox::Discard);
    warningMB->setDefaultButton(QMessageBox::Discard);
    if (warningMB->exec() == QMessageBox::Discard)
        return;

    delete warningMB;

    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');
    m_gitRunner->deleteBranch(branch);
    m_table->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::renameBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');

    QPointer<BranchDialog> renameBranch = new BranchDialog();

    if (renameBranch->exec() == QDialog::Rejected) {
        return;
    }

    QString newBranchName = QString(renameBranch->m_branchEdit->text());
    delete renameBranch;

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Cannot rename the section: a section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }

    m_gitRunner->renameBranch(newBranchName);
    m_table->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::createBranch()
{
    QAction *sender = dynamic_cast<QAction*>(this->sender());
    QString branch = sender->text();
    branch.remove('&');

    QPointer<BranchDialog> createBranch = new BranchDialog();

    if (createBranch->exec() == QDialog::Rejected) {
        return;
    }

    QString newBranchName = QString(createBranch->m_branchEdit->text());
    delete createBranch;

    if (m_branches.contains(newBranchName)) {
        QMessageBox *mb = new QMessageBox(QMessageBox::Warning,
                                          i18n("Warning"),
                                          i18n("Cannot create section: a section with this name already exists."),
                                          QMessageBox::NoButton,
                                          this, Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        mb->exec();
        return;
    }

    m_gitRunner->newBranch(newBranchName);
    m_table->disconnect();
    loadTimeLine(m_workingDir);
}

void TimeLine::setWorkingDir(const KUrl &dir)
{
    m_workingDir = dir;
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
    //qDebug() << "m_table total lenght" << m_table->totalLenght();

    bool vertical = ((m_table->location() == Qt::RightDockWidgetArea) ||
                    (m_table->location() == Qt::LeftDockWidgetArea));

    if(vertical) {
        if(m_table->totalLenght() > newSize.height()) {
            m_table->setFixedWidth(m_table->columnWidth(0) + m_table->scrollbarSize(vertical).width());
            return;
        } else {
            if(m_table->columnWidth(0) < newSize.width()) {
                m_table->setFixedWidth(m_table->columnWidth(0));
                return;
            }
        }
    } else {
        if(m_table->totalLenght() > newSize.width()) {
            m_table->setFixedHeight(m_table->rowHeight(0) + m_table->scrollbarSize(vertical).height());
            return;
        } else {
            if(m_table->rowHeight(0) < newSize.height()) {
                m_table->setFixedHeight(m_table->rowHeight(0));
                return;
            }
        }
    }
}

#include "moc_timeline.cpp"
