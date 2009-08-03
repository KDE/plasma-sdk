#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>

class CommitDialog : public QDialog
{
public:
    CommitDialog();

    QLineEdit *m_commitBriefText;
    QPlainTextEdit *m_commitFullText;

private:
    QLabel *m_commitBriefLabel;
    QLabel *m_commitFullLabel;
};

#endif // COMMITDIALOG_H
