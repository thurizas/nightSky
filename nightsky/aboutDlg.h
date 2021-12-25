#pragma once

#include <QDialog>

class QPushButton;
class QTextEdit;

class aboutDlg : public QDialog
{
	Q_OBJECT

public:
	aboutDlg(QWidget*);
	~aboutDlg();

private:
	void setupUI();

	QWidget*     m_pParent;
	QPushButton* m_btnAboutQt;
	QPushButton* m_btnOK;
	QTextEdit*   m_txtDescription;
	QTextEdit*   m_txtCopyright;

private slots:
	void aboutQT();
};
