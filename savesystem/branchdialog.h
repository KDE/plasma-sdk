#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H


#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QLabel>

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
