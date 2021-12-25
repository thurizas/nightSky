#include "helpDlg.h"

#include <QApplication>
#include <QObject>
#include <QDialog>   
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QTabWidget>
#include <QHelpEngine>
#include <QDebug>
#include <QFileInfo>
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
helpDlg::helpDlg(QObject* p) : m_pParent(p), m_pHelpEngine(nullptr)
{
    //m_pHelpEngine = new QHelpEngine(QApplication::applicationDirPath() + "../../nightSky//doc/nightSky.qhc");
    m_pHelpEngine = new QHelpEngine("D:/GNUHome/Projects/GamingAids/nightSky/nightSky/docs/nightSky.qhc");        // TODO : fix this for release
    if (!m_pHelpEngine->setupData())
    {
        QString err = m_pHelpEngine->error();
        QMessageBox::warning(this, "Help engine failure", err, QMessageBox::Ok);
    }
	setupUI();
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
helpDlg::~helpDlg()
{
    if (nullptr != m_pHelpEngine)
        delete m_pHelpEngine;
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
void helpDlg::setupUI()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("helpDlg"));
    this->resize(473, 250);
    this->setWindowTitle("Night Sky - Help");

    // setup dialog layout...
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    
    // build splitter...
    QSplitter* splitter = new QSplitter(this);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);
    
    // left hand control, tabWidget....
    QTabWidget* tabWidget = new QTabWidget(splitter);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    
    QWidget* tab1 = reinterpret_cast<QWidget*>(m_pHelpEngine->contentWidget()); 
    tab1->setObjectName(QString::fromUtf8("tab"));
    
    QWidget* tab2 = reinterpret_cast<QTabWidget*>(m_pHelpEngine->indexWidget()); 
    tab2->setObjectName(QString::fromUtf8("tab2"));

    // add widgets to tab....
    tabWidget->addTab(tab1, QString());
    tabWidget->setTabText(tabWidget->indexOf(tab1),  "Contents");
    tabWidget->addTab(tab2, QString());
    tabWidget->setTabText(tabWidget->indexOf(tab2),  "Index");

    // set up right hand control...
    helpBrowser* textViewer = new helpBrowser(m_pHelpEngine, splitter);
    textViewer->setObjectName(QString::fromUtf8("textBrowser"));

    // add controls to splitter...
    splitter->addWidget(tabWidget);
    splitter->addWidget(textViewer);

    // build button box.
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_pOK = new QPushButton(this);
    m_pOK->setObjectName(QString::fromUtf8("pushButton"));
    m_pOK->setText("Done");

    // add items to the box...
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(m_pOK);

    // add items to the layout
    verticalLayout->addWidget(splitter);
    verticalLayout->addLayout(horizontalLayout);

    connect(reinterpret_cast<QWidget*>(m_pHelpEngine->contentWidget()), SIGNAL(linkActivated(QUrl)), textViewer, SLOT(setSource(QUrl)));
    connect(reinterpret_cast<QWidget*>(m_pHelpEngine->indexWidget()), SIGNAL(linkActivated(QUrl, QString)), textViewer, SLOT(setSource(QUrl)));
    connect(m_pOK, SIGNAL(clicked()), this, SLOT(onHide()));
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
void helpDlg::onHide()
{
    QDialog::accept();
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
helpBrowser::helpBrowser(QHelpEngine* he , QWidget* p) : QTextBrowser(p), m_pHelpEngine(he)
{ }


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
helpBrowser::~helpBrowser()
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
QVariant helpBrowser::loadResource(int type, const QUrl& name)
{
    if (name.scheme() == "qthelp")
        return QVariant(m_pHelpEngine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
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
void helpBrowser::setSource(const QUrl& url)
{
    // this ugly code is to get help system work with code in the debugger... need to fix on release builds.....
    QString fpath = url.toString();
    int nLoc = fpath.lastIndexOf('/');
    if (-1 != nLoc)
    {        
        QString fname = fpath.right(fpath.length() - (nLoc+1));
        QFileInfo finfo = QFileInfo(QString("D:/GNUHome/Projects/GamingAids/nightSky/nightSky/docs/%1").arg(fname));

        if (!finfo.exists())
            QMessageBox::warning(this, "help system", QString("%1 does not exist").arg(finfo.absoluteFilePath(), QMessageBox::Ok));

        if (!finfo.isReadable())
            QMessageBox::warning(this, "help system", QString("%1 is not readable").arg(finfo.absoluteFilePath(), QMessageBox::Ok));

        //QUrl url = QUrl(finfo.absoluteFilePath());
        QTextBrowser::setSource(QUrl::fromLocalFile(finfo.absoluteFilePath()));
    }
    else
    {
        QTextBrowser::setSource(url);
    }
        
    

}