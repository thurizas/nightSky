#pragma once


#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>



class configDlg : public QDialog
{
	Q_OBJECT

public:
	configDlg(QWidget* p);
	~configDlg();

    bool showGrid() { return m_bShowGrid; }
    int  latStep() { return m_nLatStep;  }
    int  lonStep() { return m_nLonStep; }
    int  projType() { return m_nProj; }
    int  projRadius() { return m_nProjRadius;  }
    float radius() { return m_fRadius;  }
    float density() { return m_fDensity; }

private slots:
    void onGridChecked();
    void calcNbrStar();
    void onAccept();


private:
	QWidget*     m_pParent;
    QPushButton* m_btnAccept;
    QPushButton* m_btnCancel;
    QCheckBox*   m_cbxGrid;
    QComboBox*   m_cboProjection;
    QLineEdit*   m_txtStarCnt;
    QLineEdit*   m_txtDensity;
    QLineEdit*   m_txtRadius;
    QLineEdit*   m_txtLatSpace;
    QLineEdit*   m_txtLonSpace;
    QLineEdit*   m_txtProjRadius;
    QLabel*      m_labels[4];

    bool         m_bShowGrid;
    int          m_nLatStep;
    int          m_nLonStep;
    int          m_nProj;
    int          m_nProjRadius;
    float        m_fRadius;
    float        m_fDensity;
    float        m_cntStars;

	void setupUI();                // builds the UI
    void updateValues();           // gets the current values in use...
};
