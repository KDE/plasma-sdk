/******************************************************************************
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

#ifndef GITRUNNER_H
#define GITRUNNER_H

#include "dvcsjob.h"

#include <QStringList>
#include <QDir>
#include <QObject>

#include <KUrl>
#include <KProcess>

/**
  * This class executes the majority of Git commands.
  * Its usage is very simple: after setting the communication mode and the working directory,
  * you simply have to call the desired method, check the return value to ensure the process
  * has correctly done its duty, and check/process the saved result.
  *
  * @note <b>Always remember to check the result</b> of a git command, before performing an other one,
  * because every command call ovverrides the previous stored result.
  *
  * Example of usage:
  * @code
  *     GitRunner *git = new GitRunner();
  *     git->setDirectory(KUrl("/home/user/foo/"));
  *     git->setCommunicationMode(KProcess::SeparateChannels);
  *     if(git->isValidDirectory()) {
  *         // We are now sure to be in a valid git <b>root</b> tree
  *
  *         // Now suppose to create some files named foo.h foo.cpp under "foo/":
  *         // we add it to the git index by doing this
  *         QStringList *list = new QStringList();
  *         *list << "foo.h" << "foo.cpp";
  *
  *         // Note: sometimes is more easy to pass a list with only an argument: '.',
  *         // so git will automatically add all new/changed files under the working
  *         // directory and its subdirectories =)
  *
  *         if(git->add(KUrl::List(*list)) == DvcsJob::JobSucceeded) {
  *             // Here we are sure the files are added correctly to git index;
  *             // so now we can commit.
  *
  *             if(git->commit("My first commit") != DvcsJob::JobSucceeded) {
  *                 // Deal with errors
  *             }
  *         } else {
  *             // Deal with a unsuccessful git-add command
  *     }
  *
  *     // Now retrieve the available branches
  *     if(git->branches() != DvcsJob::JobSucceeded) {
  *         // Deal with errors
  *     }
  *     QString branches = git->getResult();
  *
  *     // Now let suppose you find an oldBranch branch, and you want to delete it
  *     if(git->deleteBranch(oldBranch) != DvcsJob::JobSucceeded) {
  *         //Do something
  *     }
  * @endcode
  *
  * @author Diego [Po]lentino Casella <polentino911@gmail.com>
  */
class GitRunner
{
public:
    /**
      * Simple constructor; it initializes the class with some default parameters.
      * @see setDirectory()
      * @see setCommunicationMode()
      */
    GitRunner();
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
      * Sets the communication mode.
      * @param comm Accepts one of the KProcess:OutputChannelMode modes.
      */
    void setCommunicationMode(KProcess::OutputChannelMode comm);

    /**
      * Adds a list of files in the git index, so they are ready to be committed later.
      * @param localLocations A list of KUrl containing the <b>absolute<path> of the files
      * that will be added.
      * @return The status of the performed operation.
      * @note Sometimes is more easy using a KUrl::List with a single element, '.' ; by
      * doing this, git will automatically include recursively all the modified/new
      * files in the working directory.
      */
    DvcsJob::JobStatus add(const KUrl::List &localLocations);

    /**
      * Clones an existing git repository to an other one.
      * @note Now is supported only a local cloning operation; soon there will be
      * a more general cloning support over the internet.
      * @param repoOrigin The absolute path of the directory containing the git repo.
      * @param repoDestination The destination where cloning the source to.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus createWorkingCopy(const KUrl &repoOrigin,
                                         const KUrl &repoDestination);

    /**
      * Removes the listed files from the directory and the git repo.
      * @param files A List of KUrl files to be removed.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus remove(const KUrl::List &files);


    /**
      * Checks the status of the git index tree, useful to look for uncommitted cnahges.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus status();

    /**
      * Commits the files previously added, and stores also a message containing some infos about it.
      * @param message The string containing the message to be included in the commit.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus commit(const QString &message);

    /**
      * Move HEAD from the current stage, to the commit pointed by its sha1hash, and then
      * create a new branch so new changes won't affect the original branch history.
      * @param sha1hash The sha1 hash string representing the commit we want to point to.
      * @param newBranch The string representing the new branch.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus moveToCommit(const QString &sha1hash,
                                    const QString &newBranch);

    /**
      * Delete the commit pointed by its sha1hash.
      * @param sha1hash The sha1 has string representing the commit we want to delete.
      * @return The status of the performed operation.
      * @note This command will also delete <b>all</b> the commits directly derived from
      * this one, so pay attention when using it.
      * @see moveToCommit()
      */
    DvcsJob::JobStatus deleteCommit(const QString &sha1hash);

    /**
      * Logs all git-related event, such as commits, merges and so on.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus log();

    /**
      * Init a new git repository tree in the given directory.
      * @param directory The directory we want being revision-controlled.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus init(const KUrl &directory);

    /**
      * Create a new branch with the given name.
      * @param newBranch The string containing the new branch name.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus newBranch(const QString &newBranch);

    /**
      * Renames the current branch with the new name supplied.
      * @param newBranch The string containing the new name for the current branch.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus renameBranch(const QString &newBranch);

    /**
      * Switch to the supplied branch.
      * @param newBranch The string containing the name of the branch we want to move into.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus switchBranch(const QString &newBranch);

    /**
      * Merges the current branch with the branch supplied, creating a new commit and
      * setting a message.
      * @param branchName The string containing the branch name we want to merge with.
      * @param message The string used to store the merge commit.
      * @return The status of the performed operation.
      * @note Always commit before performing a merge, otherwise you'll lose all your
      * uncommitted work.
      */
    DvcsJob::JobStatus mergeBranch(const QString &branchName,
                                   const QString &message);

    /**
      * Deletes the branch passed.
      * @param The string containing the branch you no longer need and want to remove.
      * @return The status of the performed operation.
      * @note It deletes also the commit directly derived from it, so ensure they're no
      * longer needed.
      */
    DvcsJob::JobStatus deleteBranch(const QString &branch);

    /**
      * Retrieve the current branch in use.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus currentBranch();

    /**
      * Retrieves all the branches available in the current git repo.
      * @return The status of the performed operation.
      * @note The current branch is marked with a '*'.
      */
    DvcsJob::JobStatus branches();

    /**
      * Utility function: sets the author name, so it will be displayed when
      * calling git-log or querying a particular commit.
      * @param The string representing the author name.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus setAuthor(const QString &username);

    /**
      * Utility function: sets the author email, so it will be displayed when
      * calling git-log or querying a particular commit.
      * @param The string representing the author email.
      * @return The status of the performed operation.
      */
    DvcsJob::JobStatus setEmail(const QString &email);

    /**
      * @return The string containing the result of the last action performed.
      */
    QString& getResult();

private:

    /**
      * Private function that initialize the job.
      * @param The job that must be initialized.
      */
    void initJob(DvcsJob &job);

    /**
      * Private function that start the job.
      * @param The job that must be started.
      */
    void startJob(DvcsJob &job);

    QString                     m_result;
    KUrl                        *m_lastRepoRoot;
    DvcsJob::JobStatus          m_jobStatus;
    KProcess::OutputChannelMode m_commMode;
    volatile bool               m_isRunning;
};

#endif
