#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

class   QLineEdit;
class   QPlainTextEdit;

class CommitDialog : public QDialog
{
public:
    CommitDialog();

    QLineEdit       *m_commitBriefText;
    QPlainTextEdit  *m_commitFullText;

private:
    QLabel          *m_commitBriefLabel;
    QLabel          *m_commitFullLabel;
};

#endif // COMMITDIALOG_H
