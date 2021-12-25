
#include <QObject>
#include <QDialog>   
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>

#include "starsDlg.h"
#include "nighsky.h"
#include "Star.h"


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
starsDlg::starsDlg(QWidget* p) : QDialog(p), m_pParent(p)
{
	setupUI();
    onUpdateList();
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
starsDlg::~starsDlg()
{}



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
void starsDlg::setupUI()
{
    //QWidget* layoutWidget;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* hboxLayout;
    QSpacerItem* spacerItem;

    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("Dialog"));
    this->resize(711, 452);
    this->setWindowTitle(QCoreApplication::translate("Dialog", "Star Details", nullptr));
    
    // build layout widget....
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    // set up table widget...
    QStringList    qslHeaders;
    qslHeaders << /*"index" <<*/ "Cartesian (x,y,z)" << "Spherical (r,theta, phi)" << "type" << "mass" <<  "radius" << "luminosity" ;
    m_tblStars = new QTableWidget(this);
    m_tblStars->setObjectName("tableWidget");
    m_tblStars->setColumnCount(7);
    m_tblStars->setHorizontalHeaderLabels(qslHeaders);   
 
     // setup button box and add items to it
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    hboxLayout->setContentsMargins(0, 0, 0, 0);
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);
    
    // hide button....
    m_btnCancel = new QPushButton(this);
    m_btnCancel->setObjectName(QString::fromUtf8("cancelButton"));
    m_btnCancel->setText("Hide");
    
    hboxLayout->addItem(spacerItem);
    hboxLayout->addWidget(m_btnCancel);
    
    // add items to the layout widget....
    verticalLayout->addWidget(m_tblStars);
    verticalLayout->addLayout(hboxLayout);
    
    QObject::connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(onHide()));         // TODO : change this to just hide the dialog
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// slots
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
void starsDlg::onUpdateList(void)
{
	qDebug() << "in onUpdateList";

    std::vector<CStar*>* pStars = reinterpret_cast<nightSky*>(m_pParent)->getStarData();

    if (0 == pStars->size())
    {
        QMessageBox::warning(this, "View Stars Dialog", "No stars have been generated, use File|New on main application to generate starts", QMessageBox::Ok);
    }
    else
    {
        int ndx = 0;
        int cntStars = pStars->size();

        m_tblStars->setRowCount(cntStars + 1);

        std::vector<CStar*>::iterator   iter = pStars->begin();
        while(pStars->end() != iter)
        {
             //QTableWidgetItem* pItem = new QTableWidgetItem(QString("%1").arg(ndx));
             QTableWidgetItem* pItemC = new QTableWidgetItem(QString("(%1, %2, %3)").arg((*iter)->getX()).arg((*iter)->getY()).arg((*iter)->getZ()));
             QTableWidgetItem* pItemS = new QTableWidgetItem(QString("(%1, %2, %3)").arg((*iter)->getR()).arg((*iter)->getT()).arg((*iter)->getP()));
             QTableWidgetItem* pItemT = new QTableWidgetItem(QString("%1").arg((*iter)->getType()));
             QTableWidgetItem* pItemM = new QTableWidgetItem(QString("%1").arg((*iter)->getMass()));
             QTableWidgetItem* pItemSi = new QTableWidgetItem(QString("%1").arg((*iter)->getRadius()));
             QTableWidgetItem* pItemL = new QTableWidgetItem(QString("%1").arg((*iter)->getLum()));
            

            // m_tblStars->setItem(ndx, 0, pItem);
             m_tblStars->setItem(ndx, 0, pItemC);
             m_tblStars->setItem(ndx, 1, pItemS);
             m_tblStars->setItem(ndx, 2, pItemT);
             m_tblStars->setItem(ndx, 3, pItemM);
             m_tblStars->setItem(ndx, 4, pItemSi);
             m_tblStars->setItem(ndx, 5, pItemL);
             
             ++ndx;
             ++iter;
        }
    }
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
void  starsDlg::onClearList()
{
    m_tblStars->clear();
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
void starsDlg::onHide()
{
    this->hide();
}