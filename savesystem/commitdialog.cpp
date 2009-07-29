/**
  * Simple class that pops-up a dialog to add a comment to the commit
  */

#include <klocale.h>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>

#include "commitdialog.h"

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

    connect(m_commitBriefText, SIGNAL(returnPressed()), this, SLOT(accept()));
    connect(buttonApply, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));

}
