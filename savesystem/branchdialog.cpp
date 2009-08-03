/**
  * Simple class that pops-up a dialog to create/rename a  branch
  */

#include "branchdialog.h"

#include <klocale.h>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QLineEdit>
//#include <QLabel>

BranchDialog::BranchDialog()
{

    setWindowTitle(i18n("New Section .."));
    setModal(true);
    setMinimumWidth(300);
    setMinimumHeight(80);
    setMaximumWidth(300);
    setMaximumHeight(80);

    QHBoxLayout *hlayout = new QHBoxLayout();
    m_branchLabel = new QLabel(i18n("New Section name:"), this);
    m_branchEdit  = new QLineEdit(i18n("type here"), this);
    m_branchEdit->selectAll();
    hlayout->addWidget(m_branchLabel);
    hlayout->addWidget(m_branchEdit);
    QVBoxLayout *vlayout = new QVBoxLayout();
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    QPushButton *buttonCancel = buttonBox->addButton(QDialogButtonBox::Cancel);
    QPushButton *buttonApply = buttonBox->addButton(QDialogButtonBox::Apply);

    buttonBox->setCenterButtons(true);
    vlayout->addItem(hlayout);
    vlayout->addWidget(buttonBox);
    setLayout(vlayout);

    connect(m_branchEdit, SIGNAL(returnPressed()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

