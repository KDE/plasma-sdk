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

/**
  * Simple class that pops-up a dialog to add a comment to the commit
  */

#include "commitdialog.h"

#include <QFormLayout>
#include <QPushButton>

#include <klocale.h>


CommitDialog::CommitDialog(QWidget *parent)
    : KDialog(parent)
{
    setWindowTitle(i18n("New Save Point"));

    QWidget *mainWidget = new QWidget(this);
    QFormLayout *layout = new QFormLayout(mainWidget);

    QLabel *commitBriefLabel = new QLabel(i18n("Brief comment (required):"), mainWidget);
    m_commitBriefText = new QLineEdit(i18n("Type here a brief description"), mainWidget);
    layout->addRow(commitBriefLabel, m_commitBriefText);

    QLabel *commitFullLabel = new QLabel(i18n("Detailed comment:"), mainWidget);
    m_commitFullText = new QPlainTextEdit(mainWidget);
    layout->addRow(commitFullLabel, m_commitFullText);

    setMainWidget(mainWidget);
    setButtons(KDialog::Ok | KDialog::Cancel);
    setButtonText(KDialog::Ok, i18n("Create Save Point"));

    m_commitBriefText->setFocus();
    m_commitBriefText->selectAll();

    connect(m_commitBriefText, SIGNAL(returnPressed()),
            this, SLOT(accept()));
}

QString CommitDialog::briefText() const
{
    return m_commitBriefText->text();
}

QString CommitDialog::fullText() const
{
    return m_commitFullText->toPlainText();
}

#include "commitdialog.moc"

