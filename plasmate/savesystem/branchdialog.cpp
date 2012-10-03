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

#include "branchdialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <klocale.h>


BranchDialog::BranchDialog()
{

    setWindowTitle(i18n("New Branch"));
    setModal(true);
    setMinimumWidth(300);
    setMinimumHeight(80);
    setMaximumWidth(300);
    setMaximumHeight(80);

    QHBoxLayout *hlayout = new QHBoxLayout();
    m_branchLabel = new QLabel(i18n("New branch name:"), this);
    m_branchEdit  = new QLineEdit(i18n("type here"), this);
    m_branchEdit->selectAll();
    hlayout->addWidget(m_branchLabel);
    hlayout->addWidget(m_branchEdit);
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->addButton(QDialogButtonBox::Cancel);
    buttonBox->addButton(QDialogButtonBox::Apply);

    buttonBox->setCenterButtons(true);
    vlayout->addItem(hlayout);
    vlayout->addWidget(buttonBox);
    setLayout(vlayout);

    // For security reasons, allow standard chars to be inserted
    connect(m_branchEdit, SIGNAL(textEdited(const QString&)),
            this, SLOT(validateBranchName(const QString&)));

    connect(m_branchEdit, SIGNAL(returnPressed()),
            this, SLOT(accept()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton *)),
            this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()),
            this, SLOT(reject()));
}

void BranchDialog::validateBranchName(const QString& name)
{
    QRegExp validatePluginName("[a-zA-Z0-9_.]*");
    if (!validatePluginName.exactMatch(name)) {
        int pos = 0;
        for (int i = 0; i < name.size(); i++) {
            if (validatePluginName.indexIn(name, pos, QRegExp::CaretAtZero) == -1)
                break;
            pos += validatePluginName.matchedLength();
        }
        m_branchEdit->setText(QString(name).remove(pos, 1));
    }
}
