#pragma once

#include <QDialog>


class QPushButton;
class QTableWidget;

class starsDlg : public QDialog
{
	Q_OBJECT

public:
	starsDlg(QWidget* p);
	~starsDlg();

public slots:
	void onUpdateList(void);
	void onClearList(void);

private:
	QWidget*      m_pParent;
	QPushButton*  m_btnCancel;
	QTableWidget* m_tblStars;

	void setupUI();


private slots:
	void onHide();

};
