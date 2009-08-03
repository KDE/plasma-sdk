#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H


#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLabel>

class BranchDialog : public QDialog
{
public:
    BranchDialog();
    QString &text();

    QLineEdit *m_branchEdit;

private:
    QLabel *m_branchLabel;
};

#endif // BRANCHDIALOG_H
