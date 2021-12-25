#pragma once

#include <QDialog>
#include <QTextBrowser>

class QPushButton;
class QHelpEngine;

class helpDlg : public QDialog
{
	Q_OBJECT

public:
	helpDlg(QObject* p);
	~helpDlg();

private slots:
	void onHide();

private:
	void setupUI();

	QObject*       m_pParent;

	QHelpEngine*   m_pHelpEngine;
	QPushButton*   m_pOK;
};

class helpBrowser : public QTextBrowser
{
	Q_OBJECT

public:
	helpBrowser(QHelpEngine*, QWidget*);
	~helpBrowser();

	void setSource(const QUrl& url);
	QVariant loadResource(int, const QUrl&);

private:
	QHelpEngine* m_pHelpEngine;
};
