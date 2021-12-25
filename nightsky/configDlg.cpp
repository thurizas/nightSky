#include <QString>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

#include "configDlg.h"
#include "nighsky.h"


/**********************************************************************************************************************************************************************************
 * function  : closeEvent
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
configDlg::configDlg(QWidget* p): QDialog(p), m_pParent(p)
{
    setupUI();
    updateValues();
}



/**********************************************************************************************************************************************************************************
 * function  : closeEvent
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
configDlg::~configDlg()
{

}



/**********************************************************************************************************************************************************************************
 * function  : closeEvent
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void configDlg::setupUI()
{
    QWidget*     layoutWidget;
    QHBoxLayout* hboxLayout;
    QSpacerItem* spacerItem;

    QFrame* line;
    QFrame* line2;

    QLabel* label;
    QLabel* label1;
    QLabel* label6;
    QLabel* label7;
    QLabel* label8;
    QLabel* label9;
    QLabel* label10;

    // set up for for use with the dialog...
    QFont font;
    font.setFamily(QString::fromUtf8("Courier New"));
    font.setPointSize(12);

    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("configDialog"));
    this->setWindowTitle("Configuration Parameters");
    this->resize(434, 392);

    layoutWidget = new QWidget(this);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(70, 340, 351, 33));
    
    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(190, 15, 431, 20));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    
    // top divider....
    label1 = new QLabel(this);
    label1->setObjectName(QString::fromUtf8("label1"));
    label1->setFont(font);
    label1->setGeometry(QRect(9, 12, 191, 21));
    label1->setText("Display Properties");
    
    // controls for dealing with displaying grid ....
    m_cbxGrid = new QCheckBox(this);
    m_cbxGrid->setText("Display Grid");
    m_cbxGrid->setObjectName(QString::fromUtf8("m_cbxGrid"));
    m_cbxGrid->setGeometry(QRect(20, 40, 151, 21));
    m_cbxGrid->setFont(font);
    
    // ... set up labels for use with lat/long entry fields ...
    for (int ndx = 0; ndx < 4; ndx++)
    {
        m_labels[ndx] = new QLabel(this);
        m_labels[ndx]->setObjectName(QString("label%1").arg(ndx + 2));
        m_labels[ndx]->setEnabled(false);
        m_labels[ndx]->setFont(font);
    }
    m_labels[0]->setGeometry(QRect(40, 70, 171, 16));
    m_labels[0]->setText("Latitude spacing");
    m_labels[1]->setGeometry(QRect(40, 100, 171, 16));
    m_labels[1]->setText("Longitude spacing");
    m_labels[2]->setGeometry(QRect(270, 70, 71, 16));
    m_labels[2]->setText("Degrees");
    m_labels[3]->setGeometry(QRect(270, 100, 71, 16));
    m_labels[2]->setText("Degrees");

    // ... set up edit controls for data entry ...
    m_txtLatSpace = new QLineEdit(this);
    m_txtLatSpace->setObjectName(QString::fromUtf8("m_edtLatSpace"));
    m_txtLatSpace->setEnabled(false);
    m_txtLatSpace->setFont(font);
    m_txtLatSpace->setGeometry(QRect(220, 70, 41, 20));
        
    m_txtLonSpace = new QLineEdit(this);
    m_txtLonSpace->setObjectName(QString::fromUtf8("m_ednLonSpace"));
    m_txtLonSpace->setEnabled(false);
    m_txtLonSpace->setFont(font);
    m_txtLonSpace->setGeometry(QRect(220, 100, 41, 20));
               
    // set up controls for getting projection to use....
    label6 = new QLabel(this);
    label6->setObjectName(QString::fromUtf8("label6"));
    label6->setFont(font);
    label6->setText("Projection Type:");
    label6->setGeometry(QRect(20, 130, 181, 16));

    m_cboProjection = new QComboBox(this);
    m_cboProjection->setObjectName(QString::fromUtf8("m_cboProjection"));
    m_cboProjection->setGeometry(QRect(20, 150, 271, 22));
    m_cboProjection->setFont(font);
    m_cboProjection->insertItem(0, "choose a projection");
    m_cboProjection->insertItem(1, "Mollweide");
    m_cboProjection->insertItem(2, "Orthrographic");

    // set up controls for radius of projection...
    label10 = new QLabel(this);
    label10->setObjectName(QString::fromUtf8("label_2"));
    label10->setText("Radius of Projection:");
    label10->setGeometry(QRect(20, 190, 221, 16));
    label10->setFont(font);

    m_txtProjRadius = new QLineEdit(this);
    m_txtProjRadius->setObjectName(QString::fromUtf8("m_nProjRadius"));
    m_txtProjRadius->setGeometry(QRect(240, 190, 113, 20));
               
    // second divider...now working with simulation properties.....
    line2 = new QFrame(this);
    line2->setObjectName(QString::fromUtf8("line_2"));
    line2->setGeometry(QRect(230, 220, 431, 20));
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(9, 217, 221, 21));
    label->setFont(font);
    label->setText("Simulation Properties");

    // set up stellar density control...
    label8 = new QLabel(this);
    label8->setObjectName(QString::fromUtf8("label8"));
    label8->setGeometry(QRect(20, 250, 201, 16));
    label8->setText("Stellar Density:");
    label8->setFont(font);

    m_txtDensity = new QLineEdit(this);
    m_txtDensity->setObjectName(QString::fromUtf8("m_txtDensity"));
    m_txtDensity->setGeometry(QRect(290, 250, 113, 20));
    m_txtDensity->setFont(font);

    // set up simulation radius control...
    label9 = new QLabel(this);
    label9->setObjectName(QString::fromUtf8("label9"));
    label9->setGeometry(QRect(20, 280, 271, 16));
    label9->setText("Radius of Space (parsecs):");
    label9->setFont(font);
       
    m_txtRadius = new QLineEdit(this);
    m_txtRadius->setObjectName(QString::fromUtf8("m_txtRadius"));
    m_txtRadius->setGeometry(QRect(290, 280, 113, 20));
    m_txtRadius->setFont(font);    

    // set up number of stars generated control...
    label7 = new QLabel(this);
    label7->setObjectName(QString::fromUtf8("label7"));
    label7->setGeometry(QRect(20, 310, 181, 16));
    label7->setText("Number of Stars:");
    label7->setFont(font);

    m_txtStarCnt = new QLineEdit(this);
    m_txtStarCnt->setObjectName(QString::fromUtf8("m_txtStarCnt"));
    m_txtStarCnt->setGeometry(QRect(290, 310, 113, 20));
    m_txtStarCnt->setFont(font);
    m_txtStarCnt->setReadOnly(true);

    // set up button box....
    hboxLayout = new QHBoxLayout(layoutWidget);
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_btnAccept = new QPushButton(layoutWidget);
    m_btnAccept->setObjectName(QString::fromUtf8("m_btnAccept"));
    m_btnAccept->setText("Accept");

    m_btnCancel = new QPushButton(layoutWidget);
    m_btnCancel->setObjectName(QString::fromUtf8("m_btnCancel"));
    m_btnCancel->setText("Cancel");
    
    hboxLayout->addItem(spacerItem);
    hboxLayout->addWidget(m_btnAccept);
    hboxLayout->addWidget(m_btnCancel);

    // set up signals/slots....
    QObject::connect(m_btnAccept, SIGNAL(clicked()), this, SLOT(onAccept()));
    QObject::connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    QObject::connect(m_cbxGrid, SIGNAL(stateChanged(int)), this, SLOT(onGridChecked()));
    QObject::connect(m_txtDensity, SIGNAL(textChanged(const QString&)), this, SLOT(calcNbrStar()));
    QObject::connect(m_txtRadius, SIGNAL(textChanged(const QString&)), this, SLOT(calcNbrStar()));
}



/**********************************************************************************************************************************************************************************
 * function  :  
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void configDlg::updateValues()
{
    bool bCheck = reinterpret_cast<nightSky*>(m_pParent)->getDisplayGrid();
    Qt::CheckState checked = (bCheck ? Qt::Checked : Qt::Unchecked);    // convert true/false into Qt::CheckState value.
    m_cbxGrid->setCheckState(checked);

    m_txtLatSpace->setText(QString("%1").arg(reinterpret_cast<nightSky*>(m_pParent)->getLatSpacing()));
    m_txtLonSpace->setText(QString("%1").arg(reinterpret_cast<nightSky*>(m_pParent)->getLonSpacing()));
    
    m_cboProjection->setCurrentIndex(reinterpret_cast<nightSky*>(m_pParent)->getProjType());
    
    m_txtProjRadius->setText(QString("%1").arg(reinterpret_cast<nightSky*>(m_pParent)->getProjRadius()));
    m_txtDensity->setText(QString("%1").arg(reinterpret_cast<nightSky*>(m_pParent)->getStellarDensity()));
    m_txtRadius->setText(QString("%1").arg(reinterpret_cast<nightSky*>(m_pParent)->getSimRadius()));

 
    m_txtStarCnt->setText(QString("%1").arg(m_cntStars));
}



/**********************************************************************************************************************************************************************************
 * function  :  
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void configDlg::onGridChecked()
{
    bool checked = m_cbxGrid->isChecked();

    for (int ndx = 0; ndx < 4; ndx++)
    {
        m_labels[ndx]->setEnabled(checked);
    }
    m_txtLatSpace->setEnabled(checked);
    m_txtLonSpace->setEnabled(checked);
}



/**********************************************************************************************************************************************************************************
 * function  :  
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void configDlg::calcNbrStar()
{
    float radius = reinterpret_cast<nightSky*>(m_pParent)->getSimRadius();
    float density = reinterpret_cast<nightSky*>(m_pParent)->getStellarDensity();

    m_cntStars = 4 * PI * radius * radius * radius * density / 3;
}



/**********************************************************************************************************************************************************************************
 * function  :  
 *
 * abstract  : gets the values from the controls and store in member variables.
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/

void configDlg::onAccept()
{
    m_bShowGrid = m_cbxGrid->isChecked();
    m_nLatStep = m_txtLatSpace->text().toInt();
    m_nLonStep = m_txtLonSpace->text().toInt();
    m_nProj = m_cboProjection->currentIndex();
    m_nProjRadius = m_txtProjRadius->text().toInt();
    m_fRadius = m_txtRadius->text().toFloat();
    m_fDensity = m_txtDensity->text().toFloat();

    QDialog::accept();
}