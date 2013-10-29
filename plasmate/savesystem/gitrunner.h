/******************************************************************************
 * Copyright (C) 2009 by Diego '[Po]lentino' Casella <polentino911@gmail.com> *
 * Copyright (C) 2013 by Giorgos Tsiapaliokas <terietor@gmail.com>            *
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

#ifndef GITRUNNER_H
#define GITRUNNER_H

#include <QStringList>
#include <QObject>

#include <KUrl>

class KJob;

/**
  * This class executes the majority of Git commands.
  * This class contains two kind of methods.
  * The first ones are synchronous, example of usage
  * @code
  *     GitRunner *gitrunner = new GitRunner();
  *     gitrunner->currentBranch();
  * @endcode
  *
  * the second ones are async, example of usage
  * @code
  *     GitRunner *gitrunner = new GitRunner();
  *     connect(gitrunner, SIGNAL(deleteBranchFinished()), this, SLOT(foo()));
  *     gitrunner->deleteBranch("foo");
  * @endcode
  */
class GitRunner : public QObject
{

Q_OBJECT

public:
    /**
      * Simple constructor; it initializes the class with some default parameters.
      * @see setDirectory()
      * @see setCommunicationMode()
      */
    GitRunner(QObject *parent = 0);
    ~GitRunner();

    /**
      * Checks if the current working directory is inside a valid git repository,
      * <b>and</b> that directory is also the root of that repo.
      * @return True if the directory is the git root tree, false otherwise. Note that
      * if we set as working directory a subdirectory managedby git, it will returns false.
      */
    bool isValidDirectory();

    /**
      * Checks if the current git index has new changes/files that could be committed.
      * @return True if there are changes to be committed, false otherwise.
      */
    bool hasNewChangesToCommit();

    /**
      * @return True if a git command is still running, false otherwise.
      * @note Since the main thread is blocked until the command is executed, this
      * method is useful when there are more threads running in the same app.
      */
    bool isRunning();

    /**
      * Sets the working directory for our class.
      * @param dir A KUrl with the <b>absolute</b> path of the working directory.
      */
    void setDirectory(const KUrl &dir);

    /**
      * Adds a list of files in the git index, so they are ready to be committed later.
      * @param localLocations A list of KUrl containing the <b>absolute<path> of the files
      * that will be added.
      * @note Sometimes is more easy using a KUrl::List with a single element, '.' ; by
      * doing this, git will automatically include recursively all the modified/new
      * files in the working directory.
      */
    void add(const KUrl::List &localLocations);

    /**
      * Clones an existing git repository to an other one.
      * @note Now is supported only a local cloning operation; soon there will be
      * a more general cloning support over the internet.
      * This opearation is async.
      * @see createWorkingCopy()
      * @param repoOrigin The absolute path of the directory containing the git repo.
      * @param repoDestination The destination where cloning the source to.
      */
    void createWorkingCopy(const KUrl &repoOrigin,
                                         const KUrl &repoDestination);

    /**
      * Removes the listed files from the directory and the git repo.
      * This opearation is async.
      * @see removeFinished()
      * @param files A List of KUrl files to be removed.
      */
    void remove(const KUrl::List &files);

    /**
      * Checks the status of the git index tree, useful to look for uncommitted cnahges.
      * @return The status of the performed operation.
      */
    QString status();

    /**
      * Commits the files previously added, and stores also a message containing some infos about it.
      * This opearation is async.
      * @see commitFinished()
      * @param message The string containing the message to be included in the commit.
      */
    void commit(const QString &message);

    /**
      * Move HEAD from the current stage, to the commit pointed by its sha1hash, and then
      * create a new branch so new changes won't affect the original branch history.
      * This opearation is async.
      * @see moveToCommitFinished()
      * @param sha1hash The sha1 hash string representing the commit we want to point to.
      * @param newBranch The string representing the new branch.
      */
    void moveToCommit(const QString &sha1hash,
                                    const QString &newBranch);

    /**
      * Delete the commit pointed by its sha1hash.
      * This opearation is async.
      * @see deleteCommitFinished()
      * @param sha1hash The sha1 has string representing the commit we want to delete.
      * @return /he status of the performed operation.
      * @note This command will also delete <b>all</b> the commits directly derived from
      * this one, so pay attention when using it.
      * @see moveToCommit()
      */
    void deleteCommit(const QString &sha1hash);

    /**
      * Logs all git-related event, such as commits, merges and so on.
      * @return The status of the performed operation.
      */
    QList<QHash<QString, QString> > log();

    /**
      * Init a new git repository tree in the given directory.
      * @see setDirectory
      */
    void init();

    /**
      * It adds a new line inside the .gitignore file (if any, will be created too)
      * This opearation is async.
      * @see initFinished()
      * @param file extension to be ignored from git
      */
    void addIgnoredFileExtension(const QString ignoredFileExtension);

    /**
      * Create a new branch with the given name.
      * This opearation is async.
      * @see newBranchFinished()
      * @param newBranch The string containing the new branch name.
      */
    void newBranch(const QString &newBranch);

    /**
      * Renames the current branch with the new name supplied.
      * This opearation is async.
      * @see renameBranchFinished()
      * @param newBranch The string containing the new name for the current branch.
      */
    void renameBranch(const QString &newBranch);

    /**
      * Switch to the supplied branch.
      * This opearation is async.
      * @see switchBranchFinished()
      * @param newBranch The string containing the name of the branch we want to move into.
      */
    void switchBranch(const QString &newBranch);

    /**
      * Merges the current branch with the branch supplied, creating a new commit and
      * setting a message.
      * @param branchName The string containing the branch name we want to merge with.
      * @param message The string used to store the merge commit.
      * @note Always commit before performing a merge, otherwise you'll lose all your
      * uncommitted work.
      */
    void mergeBranch(const QString &branchName,
                                   const QString &message);

    /**
      * Deletes the branch passed.
      * @param The string containing the branch you no longer need and want to remove.
      * @note It deletes also the commit directly derived from it, so ensure they're no
      * longer needed.
      */
    void deleteBranch(const QString &branch);

    /**
      * Retrieve the current branch in use.
      * @return The status of the performed operation.
      */
    QString currentBranch();

    /**
      * Retrieves all the branches available in the current git repo.
      * @return The status of the performed operation.
      * @note The current branch is marked with a '*'.
      */
    QStringList branches();

    /**
      * Utility function: sets the author name, so it will be displayed when
      * calling git-log or querying a particular commit.
      * @param The string representing the author name.
      */
    void setAuthor(const QString &username);

    /**
      * Utility function: sets the author email, so it will be displayed when
      * calling git-log or querying a particular commit.
      * @param The string representing the author email.
      */
    void setEmail(const QString &email);

Q_SIGNALS:
    void renameBranchFinished();
    void deleteBranchFinished();
    void deleteCommitFinished();
    void moveToCommitFinished();
    void newBranchFinished();
    void removeFinished();
    void switchBranchFinished();
    void initFinished();
    void createWorkingCopyFinished();
    void commitFinished();

private Q_SLOTS:
    void handleRenameBranch(KJob *job);
    void handleDeleteBranch(KJob *job);
    void handleMoveToCommit(KJob *job);
    void handleNewBranch(KJob *job);
    void handleRemove(KJob *job);
    void handleSwitchBranch(KJob *job);
    void handleInit(KJob *job);
    void handleCreateWorkingCopy(KJob *job);
    void handleDeleteCommit(KJob *job);
    void handleCommit(KJob *job);

private:
    /**
      * Private function that initialize the job.
      * @param The command that must be executed.
      */
    KJob *initJob(const QStringList& command) const;
    QString execSynchronously(const QStringList& command);

    void handleError(KJob *job);
    QStringList m_branchesWithAsterisk;

    QString removeNewLines(const QString &string);
    KUrl                        *m_lastRepoRoot;
    volatile bool               m_isRunning;
};

#endif
