
#include <QDialog>

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

#include "aboutDlg.h"


const QString desc = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                     "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                     "p, li { white-space: pre-wrap; }\n"
                     "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
                     "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600;\">Night Sky</span></p>\n"
                     "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600;\"><br /></p>\n"
                     "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">A tool to procedurally generate a star field.  The tool will random genrerate a number of stars, as well as their type,"
                     " size, mass and luminosity (main sequence only).  Their positions are also generated. in both cartesian coordinates and spherical polar coordinates.</span></p>\n"
                     "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><br /></p>\n"
                     "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">The star field can be exported in either a Mollweide or and orthographic projection.  The image is exported in dxf (drawing exchange format) and can be imported into a number of other tools.</span></p></body></html>";
const QString copyr = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                      "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                      "p, li { white-space: pre-wrap; }\n"
                      "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
                      "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Version 1.0.0</span></p>\n"
                      "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><br /></p>\n"
                      "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                      "<span style=\" font-size:10pt;\">Copyright (C) 2021 Anzus Software</span></p>\n"
                      "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                      "<span style=\" font-size:10pt;\">All rights reserved</span></p></body></html>";

/**********************************************************************************************************************************************************************************
 * function  : ctor
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
aboutDlg::aboutDlg(QWidget* p) : QDialog(p), m_pParent(p)
{
	setupUI();
}



/**********************************************************************************************************************************************************************************
 * function  : dtor
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
aboutDlg::~aboutDlg()
{


}



/**********************************************************************************************************************************************************************************
 * function  : setupUI
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void aboutDlg::setupUI()
{
    QWidget* layoutWidget;
    QHBoxLayout* horizontalLayout;
    QSpacerItem* spacerItem;
    QLabel*      image;


    // setup the size police we are going to use
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());

    // setup the dialog
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("Dialog"));
    this->resize(608, 342);
    this->setWindowTitle( "About NightSky");
    this->setSizePolicy(sizePolicy);

    // setup central widget of dialog
    layoutWidget = new QWidget(this);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(320, 300, 281, 31));
    
    // setup button box..
    horizontalLayout = new QHBoxLayout(layoutWidget);
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    // set up image ....
    image = new QLabel(this);
    image->setObjectName(QString::fromUtf8("image"));
    image->setGeometry(QRect(0, 30, 321, 211)); 
    image->setText(QString());                     // remove string from label.
    image->setPixmap(QPixmap(QString::fromUtf8("../resources/320px-Planisphereceleste.jpg")));    
    
    // set up description text box....
    m_txtDescription = new QTextEdit(this);
    m_txtDescription->setObjectName(QString::fromUtf8("m_txtDescription"));
    m_txtDescription->setGeometry(QRect(330, 10, 271, 281));
    m_txtDescription->setFrameShape(QFrame::NoFrame);
    m_txtDescription->setFrameShadow(QFrame::Plain);
    m_txtDescription->setReadOnly(true);
    m_txtDescription->setHtml(desc);

    // set up copyright box....
    m_txtCopyright = new QTextEdit(this);
    m_txtCopyright->setObjectName(QString::fromUtf8("m_txtCopyright"));
    m_txtCopyright->setGeometry(QRect(10, 250, 301, 81));
    m_txtCopyright->setFrameShape(QFrame::NoFrame);
    m_txtCopyright->setFrameShadow(QFrame::Plain);
    m_txtCopyright->setReadOnly(true);
    m_txtCopyright->setHtml(copyr);

    //set up button box...
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_btnAboutQt = new QPushButton(layoutWidget);
    m_btnAboutQt->setObjectName(QString::fromUtf8("cancelButton"));
    m_btnAboutQt->setText("About QT");
    m_btnOK = new QPushButton(layoutWidget);
    m_btnOK->setObjectName(QString::fromUtf8("okButton"));
    m_btnOK->setText("OK");        
    
    // add items to the button box...
    horizontalLayout->addItem(spacerItem);
    horizontalLayout->addWidget(m_btnAboutQt);
    horizontalLayout->addWidget(m_btnOK);

    QObject::connect(m_btnOK, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(m_btnAboutQt, SIGNAL(clicked()), this, SLOT(aboutQT()));
}



/**********************************************************************************************************************************************************************************
 * function  : aboutQt
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void aboutDlg::aboutQT()
{
    qApp->aboutQt();
}