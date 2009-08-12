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

#include <QDialogButtonBox>
#include <QPushButton>

#include <klocale.h>


CommitDialog::CommitDialog()
{
    setWindowTitle(i18n("New SavePoint"));
    setModal(true);
    setMinimumWidth(300);
    setMinimumHeight(300);
    setMaximumWidth(300);
    setMaximumHeight(300);

    m_commitBriefLabel = new QLabel(i18n("Brief comment ( required ):"), this);
    m_commitBriefLabel->setGeometry(QRect(10, 0, 280, 40));

    m_commitBriefText = new QLineEdit(i18n("Type here a brief description"), this);
    m_commitBriefText->setGeometry(QRect(10, 30, 280, 30));

    m_commitFullLabel = new QLabel(i18n("Detailed comment:"), this);
    m_commitFullLabel->setGeometry(QRect(10, 70, 280, 40));

    m_commitFullText = new QPlainTextEdit(this);
    m_commitFullText->setGeometry(QRect(10, 100, 280, 150));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setGeometry(QRect(10, 260, 280, 40));
    QPushButton *buttonCancel = buttonBox->addButton(QDialogButtonBox::Cancel);
    QPushButton *buttonApply = buttonBox->addButton(QDialogButtonBox::Apply);
    buttonBox->setCenterButtons(true);

    m_commitBriefText->selectAll();

    connect(m_commitBriefText, SIGNAL(returnPressed()),
            this, SLOT(accept()));
    connect(buttonApply, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()),
            this, SLOT(reject()));

}
