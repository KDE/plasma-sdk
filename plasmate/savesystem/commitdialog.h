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

#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include <KDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>

/**
  * Simple class used to display a dialog window where the user will write comments
  * about the commit he wants to perform.
  * The widget is composed of a QLineEdit, used to insert a bried ( and required )
  * comment, and a QPlainTexEdit field, used to give more detailed information.
  *
  * @author Diego [Po]lentino Casella <polentino911@gmail.com>
  */
class CommitDialog : public KDialog
{
public:
    CommitDialog(QWidget *parent = 0);

    QString briefText() const;
    QString fullText() const;

private:
    QLineEdit      *m_commitBriefText;
    QPlainTextEdit *m_commitFullText;
};

#endif // COMMITDIALOG_H
