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

#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLabel>

/**
  * Simple class which represents a dialog used to prompt the user to insert a new
  * branch name. It shows a small dialog with a text input field and two button to
  * confirm or discard the window.
  * The text typed by the user is validated by using a regular expression, so only
  * standard characters are allowed.
  *
  * @author Diego [Po]lentino Casella <polentino911@gmail.com>
  */
class BranchDialog : public QDialog
{
Q_OBJECT

public:
    BranchDialog();
    QString &text();

    QLineEdit *m_branchEdit;

private Q_SLOTS:
    void validateBranchName(const QString& name);

private:
    QLabel *m_branchLabel;
};

#endif // BRANCHDIALOG_H
