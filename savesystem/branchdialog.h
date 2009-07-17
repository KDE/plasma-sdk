#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

class	QDialog;
class	QLabel;
class	QLineEdit;

class BranchDialog : public QDialog
{
	public:
		BranchDialog();
		QString &text();

		QLineEdit	*m_branchEdit;

	private:
		QLabel		*m_branchLabel;
};

#endif // BRANCHDIALOG_H
