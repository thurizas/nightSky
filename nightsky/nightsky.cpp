
#include <winsock.h>                  // needed for htons

#include <QTableWidget>
#include <QDebug>
#include <QResizeEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

#include <iostream>
#include <ostream>
#include <fstream>

#include <cstdlib>
#include <ctime>

#include "nighsky.h"
#include "Projection.h"
#include "Mollweide.h"
#include "Orthographic.h"
#include "constants.h"
#include "Grid.h"
#include "Star.h"
#include "CadLib.h"
#include "configDlg.h"
#include "starsDlg.h"
#include "aboutDlg.h"
#include "helpDlg.h"

const char* types[] = {"O", "B", "A", "F", "G", "K", "M"};

/**********************************************************************************************************************************************************************************
 * function  : ctor
 *
 * abstract  : 
 *
 * parameters: 
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 * modified  : GKHuber Nov 2021 - support for adding a second projection, move code to draw grid into the individual projection classes, added help system, added preferences
 *                                clean up GUI
 ************************************************************************************************************************************************************************************/
nightSky::nightSky(QWidget *parent) : QMainWindow(parent), m_projection(nullptr), m_pGrid(nullptr), m_pScene(nullptr), m_pStars(nullptr), m_pHelp(nullptr)
{
    m_p3Dview = nullptr;

    readSettings();               // read settings from configuration files...
    setupUI();   
    setupActions();
    setupMenu();
    init();

    //SdebugStarLocs();

    srand (static_cast <unsigned> (time(0)));

    m_dist = new CGEVDist(12.9183, 2.37793, -1.0);        // setup our extreme value distribution for lumonisity.

   
    m_bDirty = false;
    m_b3DShown = false;
    m_fileName = "";
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
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
nightSky::~nightSky()
{
    if(m_vecStars.size() > 0)
    {
        std::vector<CStar*>::iterator    iter = m_vecStars.begin();
        while(m_vecStars.end() != iter)
        {
            delete (*iter);
            (*iter) = NULL;
            ++iter;
        }

        m_vecStars.erase(m_vecStars.begin(), m_vecStars.end());
    }

    delete m_pGrid;
    delete m_projection;
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
 * written   : GKHuber Apr 2015
 * modified  : GKHuber Nov 2021 - removed display pane making map window full screen, add additional menu items to have display info a modeless dialog.
 ************************************************************************************************************************************************************************************/
void nightSky::setupUI()
{
    if(this->objectName().isEmpty())
        this->setObjectName(QStringLiteral("MainWindow"));

    this->resize(1130, 755);

    QWidget* centralwidget = new QWidget(this);
    centralwidget->setObjectName(QStringLiteral("centralWidget"));
    centralwidget->setGeometry(QRect(0, 0, 1029, 704));
    
    // need to generate a scene
    m_pScene = new QGraphicsScene(this);

    m_canvas = new starView(m_pScene, centralwidget);
    m_canvas->setObjectName(QStringLiteral("m_canvas"));
    m_canvas->setMouseTracking(true);
    m_canvas->setBackgroundBrush(QBrush(Qt::blue));
    m_canvas->setGeometry(QRect(5, 5, 1029, 699));

    this->setCentralWidget(centralwidget);
        
    m_menubar = new QMenuBar(this);
    m_menubar->setObjectName(QStringLiteral("menubar"));
    m_menubar->setGeometry(QRect(0, 0, 1029, 21));
    
    m_statusbar = new QStatusBar(this);
    m_statusbar->setObjectName(QStringLiteral("statusbar"));  
    m_statusbar->setGeometry(QRect(0, 0, 1029, 30));
    
    this->setMenuBar(m_menubar);
    this->setStatusBar(m_statusbar);

    qDebug() << QString("dimension of main window is: %1 by %2").arg(this->width()).arg(this->height());
    qDebug() << QString("dimensions of the central widget is %1 by %2").arg(centralwidget->width()).arg(centralwidget->height());
    qDebug() << QString("dimension of the menu bar is %1 by %2").arg(m_menubar->width()).arg(m_menubar->height());
    qDebug() << QString("dimension of the status bar is %1 by %2").arg(m_statusbar->width()).arg(m_statusbar->height());
    qDebug() << QString("dimension of view is: %1 by %2").arg(m_canvas->width()).arg(m_canvas->height());
}



/**********************************************************************************************************************************************************************************
 * function  : setupMenu
 *
 * abstract  : This function sets up the menu for the application.  It must be called after setupActions() so that the actions have been created.
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 * modified  : GKHuber Nov 2021 - added action for m_pViewStars, m_pViewPrefs, and m_pViewRedraw
 ************************************************************************************************************************************************************************************/
void nightSky::setupMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_pFileOpen);
    fileMenu->addAction(m_pFileNew);
    fileMenu->addAction(m_pFileClose);
    fileMenu->addSeparator();
    fileMenu->addAction(m_pFileSave);
    fileMenu->addAction(m_pFileSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_pFileExport);
    fileMenu->addSeparator();
    fileMenu->addAction(m_pFileExit);

    QMenu*  viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(m_pViewZoomIn);
    viewMenu->addAction(m_pViewZoomOut);
    viewMenu->addAction(m_pViewRedraw);
    viewMenu->addSeparator();
    viewMenu->addAction(m_pViewStars);
    viewMenu->addAction(m_pView3D);
    viewMenu->addSeparator();
    viewMenu->addAction(m_pViewPrefs);

    QMenu*  debugMenu = menuBar()->addMenu("Debug");
    debugMenu->addAction(m_pDebugLumDist);
    
    QMenu*  helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_pHelpAbout);
    helpMenu->addSeparator();
    helpMenu->addAction(m_pHelpHelp);
}



/**********************************************************************************************************************************************************************************
 * function  : setupActions
 *
 * abstract  : THis function sets up the actions for when a menu item has been triggered.  In addtion it create the status text as well short-cut keys, if any.  The appropriate 
 *             connections for signals and slots are established.
 *
 * parameters: none
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 * modified  : GKHuber Nov 2021 - added actions to support new features.
 ************************************************************************************************************************************************************************************/
void nightSky::setupActions()
{
    m_pFileOpen = new QAction("&Open", this);
    m_pFileOpen->setShortcuts(QKeySequence::Open);
    m_pFileOpen->setStatusTip("Create a new file");
    connect(m_pFileOpen, SIGNAL(triggered()), this, SLOT(onOpen()));

    m_pFileNew = new QAction("&New", this);
    m_pFileNew->setShortcuts(QKeySequence::New);
    m_pFileNew->setStatusTip("Create a new file");
    connect(m_pFileNew, SIGNAL(triggered()), this, SLOT(onNew()));

    m_pFileClose = new QAction("&Close", this);
    m_pFileClose->setShortcuts(QKeySequence::Close);
    m_pFileClose->setStatusTip("Create a new file");
    connect(m_pFileClose, SIGNAL(triggered()), this, SLOT(onClose()));

    m_pFileSave = new QAction("Save", this);
    m_pFileSave->setShortcuts(QKeySequence::Save);
    m_pFileSave->setStatusTip("Save the current generation");
    connect(m_pFileSave, SIGNAL(triggered()), this, SLOT(onSave()));

    m_pFileSaveAs = new QAction("SaveAs", this);
    m_pFileSaveAs->setShortcuts(QKeySequence::SaveAs);
    m_pFileSaveAs->setStatusTip("Save the current generation, with a new name");
    connect(m_pFileSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAs()));

    m_pFileExport = new QAction("Export", this);
    //m_pFileExport->setShortcuts(QKeySequence::New);
    m_pFileExport->setStatusTip("Export the file to fcw format");
    connect(m_pFileExport, SIGNAL(triggered()), this, SLOT(onExport()));

    m_pView3D = new QAction("View 3D display", this);
    m_pView3D->setStatusTip("View 3D display (skybox view)");
    connect(m_pView3D, SIGNAL(triggered()), this, SLOT(onView3D()));
    
    m_pViewPrefs = new QAction("Preferences", this);
    m_pViewPrefs->setStatusTip("Modify generattion parameters");
    connect(m_pViewPrefs, SIGNAL(triggered()), this, SLOT(onPrefs()));

    m_pViewZoomOut = new QAction("Zoom Out", this);
    m_pViewZoomOut->setStatusTip("Zoom out on star-field");
    connect(m_pViewZoomOut, SIGNAL(triggered()), this, SLOT(onZoomOut()));

    m_pViewZoomIn = new QAction("Zoom In", this);
    m_pViewZoomIn->setStatusTip("Zoom in on star-field");
    connect(m_pViewZoomIn, SIGNAL(triggered()), this, SLOT(onZoomIn()));

    m_pViewStars = new QAction("View Stars", this);
    m_pViewStars->setStatusTip("View details on generated stars");
    connect(m_pViewStars, SIGNAL(triggered()), this, SLOT(onViewStars()));

    m_pViewRedraw = new QAction("Redraw map", this);
    m_pViewRedraw->setStatusTip("Redraws the current image");
    connect(m_pViewRedraw, SIGNAL(triggered()), this, SLOT(onViewRedraw()));

    m_pDebugLumDist = new QAction("View Lum Dist", this);
    m_pDebugLumDist->setStatusTip("View stellar luminosity distribution");
    connect(m_pDebugLumDist, SIGNAL(triggered()), this, SLOT(onDispLum()));

    m_pFileExit = new QAction("e&Xit", this);
    m_pFileExit->setShortcuts(QKeySequence::Quit);
    m_pFileExit->setStatusTip("Exits the application");
    connect(m_pFileExit, SIGNAL(triggered()), this, SLOT(onExit()));

    m_pHelpHelp = new QAction("&Help", this);
    m_pHelpHelp->setShortcuts(QKeySequence::HelpContents);
    m_pHelpHelp->setStatusTip("Displays application help");
    connect(m_pHelpHelp, SIGNAL(triggered()), this, SLOT(onHelp()));

    m_pHelpAbout = new QAction("About", this);
    //m_pHelpAbout->setShortcuts(QKeySequence::About);
    m_pHelpAbout->setStatusTip("Displays about and copyright information");
    connect(m_pHelpAbout, SIGNAL(triggered()), this, SLOT(onAbout()));
}



/**********************************************************************************************************************************************************************************
 * function  : init
 *
 * abstract  : This is called once during construction and any time the parameters change...
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::init()
{
    // delete existing object, if they exist..
    if (nullptr != m_projection) delete m_projection;
    if (nullptr != m_pGrid) delete m_pGrid;
    if (nullptr != m_pStars) emit clearList();

    m_pScene->clear();

    // rebuild with the current settings....
    switch (m_nProjType)
    {
        case PROJ_MOLLWEIDE:
            m_projection = new CMollweide(m_nProjRadius);
            break;
        case PROJ_ORTHRO:
            m_projection = new COrthographic(m_nProjRadius);
            break;
        default:
             // TODO : handle error of unknow projection type....
            break;
    }

    m_pGrid = new CGrid(180 / m_nLatSpacing, 360 / m_nLonSpacing);
  
    m_pGrid->convert(m_projection);                       // convert the grid, in lat lon, to projection coordinates
    m_projection->drawGrid(m_pGrid, m_pScene);
}



/**********************************************************************************************************************************************************************************
 * function  : readSettings
 *
 * abstract  : This reads the saved settings from the registry or init file based on OS.
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::readSettings()
{
    QSettings   settings("Anzus", "Night Sky");

    m_displayGrid = settings.value("display/showGrid", true).toBool();
    m_nLatSpacing = settings.value("display/latitudeSpacing", 15).toInt();
    m_nLonSpacing = settings.value("display/longitudeSpacing", 30).toInt();
    m_nProjType = settings.value("display/ProjectionType", 1).toInt();
    m_nProjRadius = settings.value("display/ProjectionRadius", 400).toInt();
    m_fDensity = settings.value("simulation/StellarDensity", 0.130262f).toFloat();
    m_fRadius = settings.value("simulation/SimulationRadius", 20.0).toFloat();
}




/**********************************************************************************************************************************************************************************
 * function  : readSettings
 *
 * abstract  : This writes the current setting to the registryor init file based on OS.
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::writeSettings()
{
    QSettings settings("Anzus", "Night Sky");

    settings.setValue("display/showGrid", m_displayGrid);
    settings.setValue("display/latitudeSpacing", m_nLatSpacing);
    settings.setValue("display/longitudeSpacing", m_nLonSpacing);
    settings.setValue("display/ProjectionType", m_nProjType);
    settings.setValue("display/ProjectionRadius", m_nProjRadius);
    settings.setValue("simulation/StellarDensity", m_fDensity);
    settings.setValue("simulation/SimulationRadius", m_fRadius);
}




/**********************************************************************************************************************************************************************************
 * function  : resizeEvent
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 * modified  : GKHuber Nov 2021 - removed references to the lower pane as it is not being used anymore
 ************************************************************************************************************************************************************************************/
void nightSky::resizeEvent(QResizeEvent* event)
{
    int  width = event->size().width();
    int  heigth = event->size().height();

    // Let the main window do its thing.
    QMainWindow::resizeEvent(event);
 
    // resize the 2D display...
    m_canvas->setGeometry(QRect(5, 5, width-10, heigth-10));
}



 /**********************************************************************************************************************************************************************************
  * function  : initInterface
  *
  * abstract  : this function clears all the GUI elements and associated data structures in prepraration for a new generation
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  * modified  : GKHuber Nov 2021 - added actions to support new features.
  ************************************************************************************************************************************************************************************/
void nightSky::initInterface()
{
    m_pScene->clear();                              // clear the 2D display
    m_projection->drawGrid(m_pGrid, m_pScene);      // redraw the grid
    emit clearList();
    
    // clear the generated vector of stars
    if(m_vecStars.size() > 0)
    {
        std::vector<CStar*>::iterator    iter = m_vecStars.begin();
        while(m_vecStars.end() != iter)
        {
            delete (*iter);
            (*iter) = NULL;
            ++iter;
        }

        m_vecStars.erase(m_vecStars.begin(), m_vecStars.end());
    }

    m_bDirty = false;
}



/**********************************************************************************************************************************************************************************
 * function  : checkSave
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
int nightSky::checkSave()
{
    QMessageBox msgBox;
    
    msgBox.setText("The document has been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    
    return msgBox.exec();
}



/**********************************************************************************************************************************************************************************
 * function  : onOpen
 *
 * abstract  : This function opens a pre-existing data file.  The file consists of a header (struct hdr) and an array of star records (struct star).  The data form is binary.
 *
 * parameters: void
 *
 * returns   : void
 *
 * written   : GKHuber Dec 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onOpen()
{
    QString filters = "data files (*.sky *.dat);;text files (*.csv *.txt);;all files (*.*)";

    if(m_bDirty)
    {
        // save if requested
        int action = checkSave();
        if(QMessageBox::Save == action)
            onSave();
        
        if(QMessageBox::Cancel == action)
            return;

        // clean up from the last generation run
        initInterface();
    }

    // get the file name
    m_fileName = QFileDialog::getOpenFileName(this, "select file to open", "./", filters);

    std::ifstream rfile;
    rfile.exceptions(std::ios::failbit | std::ios::badbit);           // set out stream to throw exceptiosn on errors.
    try
    {
        rfile.open(m_fileName.toStdString(), std::ios::in | std::ios::binary);
        
        struct hdr   header = { 0 };

        rfile.read((char*)&header, sizeof(struct hdr));

        if (0 == strncmp("nsky", header.hdr_id, ID_LEN))         // check out magic  number ...
        {
            uint8_t version = (0x1 << 4) | ((0x3 << 4) >> 4);    // supported version is currently 1.3
            if (header.hdr_id[4] == version)
            {
                int cntStars = header.hdr_cntStars;
                int offset = header.hdr_offsetRcd;

                if (offset != rfile.tellg())                      // stream pointer not at first record
                {
                    rfile.seekg(0);                               // move to beginning of stream
                    rfile.seekg(offset, std::ios::beg);           // more to records stat
                }

                for (int ndx = 0; ndx < cntStars; ndx++)
                {
                    struct star    temp;
                    rfile.read((char*)&temp, sizeof(struct star));
                    CStar* star = new CStar(temp.star_radius, temp.star_theta, temp.star_phi, temp.star_type, temp.star_mass, true);
                    m_vecStars.push_back(star);

                }

                // populate all views.
                populateList();
                drawStarField();
            }
            else
            {
                // TODO : deal with wrong version of file 
            }

            rfile.close();
        }
        else
        {
            // TODO : we do not have a data file

        }
    }
    catch(std::ifstream::failure exc)
    {
        std::cout << "error opening/reading file" << std::endl;
    }
    catch (std::bad_alloc exc)
    {
        std::cout << "memory allocation error" << std::endl;
    }
}



/**********************************************************************************************************************************************************************************
 * function  : onNew
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onNew()
{
    if(m_bDirty)
    {
        // save if requested
        int action = checkSave();
        if(QMessageBox::Save == action)
            onSave();
        
        if(QMessageBox::Cancel == action)
            return;

        initInterface();        // clean up from the last generation run
    }
 
    genStarLocs();              // generate a new vector of starts
    cleanStarLocs();            // insure that stars are not to close
    
    populateList();             // populate all views.
    drawStarField();

    m_bDirty = true;
}



/**********************************************************************************************************************************************************************************
 * function  : onClose
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onClose()
{
    if(m_bDirty)
    {
        // save if requested
        int action = checkSave();
        if(QMessageBox::Save == action)
            onSave();
        
        if(QMessageBox::Cancel == action)
            return;

        initInterface();        // clean up from the last generation run
    }
}


/**********************************************************************************************************************************************************************************
 * function  : onSave
 *
 * abstract  : save the current star data in a file on the hard-drive.  This file is in binary format, and it consists of a header (see struct hdr) and then an array of star
 *             records (see struct star).
 *
 * parameters: void
 *
 * returns   : void
 *
 * written   : GKHuber Dec 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onSave()
{
    int    ndx = 1;

    if(m_vecStars.size() > 0)                           // if we don't have star data alert user and dump out of function...
    {
        if(m_fileName == "")                            // if we don't have a file path get one ....
        {
            QString filters = "data files (*.sky *.dat);;all files (*.*)";
            
            m_fileName = QFileDialog::getSaveFileName(this, "nightSky -- Save file name", "./", filters);
        }

        qDebug() << QString("file path is: %1").arg(m_fileName);

        std::ofstream   ofile;
        ofile.exceptions(std::ios::failbit | std::ios::badbit);
        try
        {
            int ndx = 1;
            ofile.open(m_fileName.toStdString(), std::ios::out | std::ios::binary);

            // write out the header
            struct hdr  header;
            memcpy(header.hdr_id, "nsky", 4);
            header.hdr_id[4] = (VER_MAJOR << 4) | ((VER_MINOR << 4) >> 4);
            header.hdr_id[5] = (testVal == (htons(testVal)) ? ENDIAN_B : ENDIAN_L);
            header.hdr_display = this->getProjType();
            header.hdr_cbRecord = sizeof(struct star);
            header.hdr_cntStars = (uint32_t)(m_vecStars.size());
            header.hdr_offsetRcd = (uint32_t)sizeof(struct hdr);

            ofile.write((char*)&header, sizeof(struct hdr));

            std::vector<CStar*>::iterator            iter = m_vecStars.begin();

            while (m_vecStars.end() != iter)                       // write one record per star
            {
                (*iter)->serialize(ndx, &ofile);
                ++iter;
                ++ndx;
            }

            ofile.close();
            m_bDirty = false;
        }
        catch (std::ofstream::failure exc)
        {
            qDebug() << QString("Uname to open %1").arg(m_fileName);
        }
    }
    else
    {
        QMessageBox::warning(this, "warning: nightSky", "no star data to save\nGenerate a star field first", QMessageBox::Ok);
    }
}




 /**********************************************************************************************************************************************************************************
  * function  : onSaveAs
  *
  * abstract  : all onSaveAs needs to do is insure that we are using a new file name. by resetting m_fileName to an empty string, we are insure that onSave asks for a name.
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  ************************************************************************************************************************************************************************************/
void nightSky::onSaveAs()
{
    if(m_fileName != "")
    {
        m_fileName = "";
    }

    onSave();
}



 /**********************************************************************************************************************************************************************************
  * function  : :onExport
  *
  * abstract  : export the data in DXF (drawing exchange format) file scale factor 10 is used for brightest stars (observed luminosity, 1 is used for lowest stars)
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  * modified  : GKHuber Nov 2021 - changed star shape from rhombus' to circles, also modified scaling with luminosity
  ************************************************************************************************************************************************************************************/
void nightSky::onExport()
{
    BOOL             result = TRUE;
    CDxfFileWrite    dxffile;                                        // TODO : pass in location of library, not hard code into constructor....

    QString filters = "data exchange files (*.dxf);;CC2 files (*.fcw);;all files (*.*)";
    QString exportName = QFileDialog::getSaveFileName(this, "nightSky -- Export file name", "./", filters);

    dxffile.Create(const_cast<char*>(exportName.toStdString().c_str()));

    // (1) create the header section
    dxffile.BeginSection(SEC_HEADER);
    dxffile.EndSection();

    // (2) create the tables section....
    dxffile.BeginSection(SEC_TABLES);                                 // open tables section....
    DXFSTYLE tstyle;
    DXFLTYPE ltype;

    dxffile.BeginTableType(TAB_LTYPE);                                      // LTYPE table type (line type)
    ZeroMemory(&ltype, sizeof(ltype));                                          // Solid line.
    strncpy(ltype.Name, "Continuous", strlen("Continuous"));                    // TODO : figure out how to change color
    strncpy(ltype.DescriptiveText, "Solid Line", strlen("Solid Line"));
    dxffile.AddLinetype(&ltype);
    dxffile.EndTableType();                                                 // close LTYPE table type 

    result &= dxffile.BeginTableType(TAB_LAYER);                            // LAYER table type  
    result &= dxffile.AddLayer("Grid", 5, "Continuous");
    result &= dxffile.AddLayer("Stars_1", 5, "Continuous");                     // luminosity < 0.001   {0, 0.001}
    result &= dxffile.AddLayer("Stars_2", 5, "Continuous");                     // luminosity < 0.01    {0.001, 0.010}
    result &= dxffile.AddLayer("Stars_3", 5, "Continuous");                     // luminosity < 0.10    {0.010, 0.100 }
    result &= dxffile.AddLayer("Stars_4", 5, "Continuous");                     // luminosity < 1.00    { 0.100, 1.00 }
    result &= dxffile.AddLayer("Stars_5", 5, "Continuous");                     // luminosity < 10.0    { 1.00, 10.0 }
    result &= dxffile.AddLayer("Stars_6", 5, "Continuous");                     // luminosity < 100.0   {10.0,100.0}
    result &= dxffile.AddLayer("Stars_7", 5, "Continuous");                     // luminosity < 1000.0  {100.0,1000.0}
    result &= dxffile.AddLayer("Stars_8", 5, "Continuous");                     // luminosity > 1000.0  {1000.0,100000}
    result &= dxffile.AddLayer("Dust", 5, "Continuous");
    result &= dxffile.AddLayer("Constallations", 5, "Continuous");
    result &= dxffile.EndTableType();                                       // close LAYER table type  

    dxffile.BeginTableType(TAB_STYLE);                                      // STYLE table type 
    ZeroMemory(&tstyle, sizeof(tstyle));
    //tstyle.Name = "Style1";
    strncpy(tstyle.Name, "Style1", 512);
    //tstyle.PrimaryFontFilename = "TIMES.TTF";
    strncpy(tstyle.PrimaryFontFilename, "TIMES.TTF", 512);
    tstyle.Height = 0.3;
    tstyle.WidthFactor = 1;
    dxffile.AddTextStyle(&tstyle);
    dxffile.EndTableType();                                                 // close STYLE table type 

    dxffile.BeginTableType(TAB_DIMSTYLE);                                   // DIMSTYLE table type 

    dxffile.EndTableType();                                                 // close DIMSTYLE table type  
    dxffile.EndSection();                                               // close TABLES section  

    // (3) create the entities section....
    dxffile.BeginSection(SEC_ENTITIES);                                 // Entities Section  

    dxffile.SetCurrentLayer("Grid");                                    // adding the grid to the map
    dxffile.SetCurrentColor(251);                                       // set color to light gray.
     m_projection->drawGrid(m_pGrid, &dxffile);

    dxffile.SetCurrentColor(255);                                       // set color to black
    dxffile.SetCurrentTextStyle("Style1");
    char      msg[MSG_LEN];
    struct _lumInterval
    {
        double   min;
        double   max;
    } lumInt[] = { {0, 0.001}, {0.001, 0.010}, {0.010, 0.100 }, { 0.100, 1.00 }, { 1.00, 10.0 }, {10.0,100.0}, {100.0,1000.0}, {1000.0,100000} };

    REALPOINT center = {0,0};
    double      radius = 10.0;


    for (int32_t ndx = 0; ndx < LUM_BINS; ndx++)
    {
        memset((void*)msg, '\0', MSG_LEN);
        sprintf(msg, "Luminosity: %3.2f - %3.2f", lumInt[ndx].min, lumInt[ndx].max);
        dxffile.Circle(center.x, center.y + VERT_SPACING * ndx, radius* scale[ndx]);
        dxffile.Text(msg, center.x + TEXT_HOFFSET, center.y+ VERT_SPACING *ndx - TEXT_VOFFSET, TEXT_HEIGHT);
    }

    dxffile.SetCurrentLayer("Stars");
    dxffile.SetCurrentColor(2);

    std::vector<CStar*>::iterator      iter = m_vecStars.begin();
    int cntStars = m_vecStars.size();
    int cnt = 1;
    while(m_vecStars.end() != iter)
    {
        if(cntStars > 300)
        {
            if((cnt != 0) && (cnt % 200))
            {
                QString strStatus = QString("exporting %1 / %2 stars (%3 %)").arg(cnt).arg(cntStars). arg((100*(cnt))/cntStars);
                m_statusbar->showMessage(strStatus);
            }
        }
 
        // output based on brightness 0 - 16 stars_1 through stars_5
        if ((*iter)->getLum() <= 0.001)
            dxffile.SetCurrentLayer("stars_1");
        else if ((*iter)->getLum() <= 0.01)
            dxffile.SetCurrentLayer("stars_2");
        else if ((*iter)->getLum() <= 0.1)
            dxffile.SetCurrentLayer("stars_3");
        else if ((*iter)->getLum() <= 1.00)
            dxffile.SetCurrentLayer("stars_4");
        else if ((*iter)->getLum() <= 10.00)
            dxffile.SetCurrentLayer("stars_5");
        else if ((*iter)->getLum() <= 100)
            dxffile.SetCurrentLayer("stars_6");
        else if ((*iter)->getLum() <= 1000)
            dxffile.SetCurrentLayer("stars_7");
        else
            dxffile.SetCurrentLayer("stars_8");


        (*iter)->draw(m_projection, &dxffile);

        ++iter;
        ++cnt;
    }
    dxffile.EndSection();                                    // close ENTITIES section ----------------------------------
    dxffile.Close();

    m_statusbar->showMessage(QString("finished exporting star map to %1").arg(exportName));
}


/**********************************************************************************************************************************************************************************
 * function  : onExit
 *
 * abstract  : 
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onExit()
{
    qDebug() << "in onExit\n";

    if (nullptr != m_pStars)
    {
        m_pStars->hide();
        m_pStars->close();
        delete m_pStars;
    }

    if (nullptr != m_pHelp)
    {
        m_pHelp->hide();
        m_pHelp->close();
        delete m_pHelp;
    }

    QApplication::exit();
}


// 
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
void nightSky::closeEvent(QCloseEvent* evt)
{
    if (nullptr != m_pStars)
    {
        m_pStars->hide();
        m_pStars->close();
        delete m_pStars;
    }

    if (nullptr != m_pHelp)
    {
        m_pHelp->hide();
        m_pHelp->close();
        delete m_pHelp;
    }

    evt->accept();
}



/**********************************************************************************************************************************************************************************
 * function  : onPerfs
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onPrefs()
{
    configDlg   dlg(this);

    dlg.show();
    int nRet = dlg.exec();
    if (QDialog::Accepted == nRet)
    {
        m_displayGrid = dlg.showGrid();
        m_nLatSpacing = dlg.latStep();
        m_nLonSpacing = dlg.lonStep();
        m_nProjType = dlg.projType();
        m_nProjRadius = dlg.projRadius();
        m_fDensity = dlg.density();
        m_fRadius = dlg.radius();

        writeSettings();             // update settings
        init();                      // update graphic components
        if (m_vecStars.size() > 0)
            onViewRedraw();
    }
}


/**********************************************************************************************************************************************************************************
 * function  : onZoomOut
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onZoomOut()
{
    if(NULL != m_canvas)
        m_canvas->scale(0.5, 0.5);
}



/**********************************************************************************************************************************************************************************
 * function  : onZoomIn
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onZoomIn()
{
    if(NULL != m_canvas)
        m_canvas->scale(2.0, 2.0);
}



/**********************************************************************************************************************************************************************************
 * function  : onViewRedraw
 *
 * abstract  : This function forces a redraw of the scene - this is useful after changing the map projection
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onViewRedraw()
{
    if (m_vecStars.size() > 0)
    {
        m_pScene->clear();                              // clear the 2D display
        m_projection->drawGrid(m_pGrid, m_pScene);      // redraw the grid
        drawStarField();
    }

}


/**********************************************************************************************************************************************************************************
 * function  : onViewStars
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onViewStars()
{
    if (nullptr == m_pStars)
    {
        m_pStars = new(std::nothrow) starsDlg(this);
        connect(this, &nightSky::updateList, m_pStars, &starsDlg::onUpdateList);
        connect(this, &nightSky::clearList, m_pStars, &starsDlg::onClearList);
    }

    m_pStars->show();
    m_pStars->raise();
    m_pStars->activateWindow();

    qDebug() << "in onViewStars";

}



/**********************************************************************************************************************************************************************************
 * function  : onView3D
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::onView3D()
{
    if(m_p3Dview == nullptr)                // never instatiated the view yet...
    {
        m_p3Dview = new C3DDisplay(nullptr);
    }

    if(!m_b3DShown)             // 3D dispaly not created yet.
    {
        m_p3Dview->show();
        m_b3DShown = true;
    }
    else
    {
        m_p3Dview->hide();
        m_b3DShown=false;
    }

    qDebug() << "in onView3D";
}


 /**********************************************************************************************************************************************************************************
  * function  : onDispLum
  *
  * abstract  : // TODO : fix this .....
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  ************************************************************************************************************************************************************************************/
void nightSky::onDispLum()
{
    std::vector<CStar*>::iterator    iter;

    int     dist[7];
    std::pair<float, float>    masses[7];
    std::pair<float, float>    lumin[7];

    int     starCnt;


    for(int ndx = 0; ndx < 7; ndx++) 
    {
        dist[ndx] = 0;                // initialize distribution array....
        masses[ndx].first = 100.0;    // initialize mass distribution array....
        masses[ndx].second = 0.0;
        lumin[ndx].first = 35778;      // initialize luminosity distribution array....
        lumin[ndx].second = 0.0;
    }



    if(0 == (starCnt = m_vecStars.size()))
    {
        qDebug() << "you need to generate a distribution first";
    }
    else
    {
        iter = m_vecStars.begin();

        
        while(m_vecStars.end() != iter)
        {
            // get the stellar distribution...
            dist[(*iter)->getType()]++;

            // get the min/max by stellar type...
            if((*iter)->getMass() < masses[(*iter)->getType()].first) masses[(*iter)->getType()].first = (*iter)->getMass();
            if((*iter)->getMass() > masses[(*iter)->getType()].second) masses[(*iter)->getType()].second = (*iter)->getMass();

            // get the min/max luminosity by stellar type...
            if((*iter)->getLum() < lumin[(*iter)->getType()].first) lumin[(*iter)->getType()].first = (*iter)->getLum();
            if((*iter)->getLum() > lumin[(*iter)->getType()].second) lumin[(*iter)->getType()].second = (*iter)->getLum();
            ++iter;
        }

        
        qDebug() << "stellar distribution by type:";
        for(int ndx = 0; ndx < 7; ndx++)
            qDebug() << "    type: " << types[ndx] << " number : " << dist[ndx] << " frequency: "  << 100.0*(float)dist[ndx]/(float)starCnt;

        qDebug() << "stellar masses by type (relative to M-sol):";
        for(int ndx = 0; ndx < 7;ndx++)
            qDebug() << "    type: " << types[ndx] << " min mass: " << masses[ndx].first << ", max mass: " << masses[ndx].second;

        qDebug() << "stellar luminosity by type (relative to sol):";
        for(int ndx = 0; ndx < 7; ndx++)
            qDebug() << "    type: " << types[ndx] << " min luminosity: " << lumin[ndx].first << ", max luminosity: " << lumin[ndx].second;
    }
    
}




/**********************************************************************************************************************************************************************************
 * function  : onHelp
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onHelp()
{
    if (nullptr == m_pHelp)
    {
        m_pHelp = new(std::nothrow) helpDlg(this);
    }

    m_pHelp->show();
    m_pHelp->raise();
    m_pHelp->activateWindow();

}


/**********************************************************************************************************************************************************************************
 * function  : onAbout
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::onAbout()
{
    aboutDlg dlg(this);
    dlg.exec();
}



 /**********************************************************************************************************************************************************************************
  * function  : genStarLocs
  *
  * abstract  : we use the estimate of 0.130262 star per cubic parsec (3.26 cubic light years)
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  ************************************************************************************************************************************************************************************/
void nightSky::genStarLocs()
{
    float    modelSize = m_fRadius;                         // radius of sphere, in parsecs   
    float    radiusSquared = modelSize*modelSize;           // pre-calculate radius squared, so we don't do it in a loop
  
    float  volSphere = (4.0*PI*modelSize*modelSize*modelSize)/3.0;
        
    int    m_nCntStarts = volSphere * m_fDensity;

    for(int ndx = 0; ndx < m_nCntStarts; ndx++)
    {
        if(m_nCntStarts > 300)                   // only show progress bar if generating 300 or more stars
        {
            if((ndx != 0) && (ndx % 200))
            {
                QString strStatus = QString("generated %1 / %2 stars (%3 %)").arg(ndx + 1).arg(m_nCntStarts). arg((100*(ndx+1))/ m_nCntStarts);
                m_statusbar->showMessage(strStatus);
            }
        }

        float tempX, tempY, tempZ;
        
        // TODO : see if there is a better way of generating uniform points in a sphere....
        do
        {
            // generate a random number in range [0, 2*modelSize], then subtract modelSize to get a range of [-modelSize, modelSize]
            tempX = 2*modelSize*static_cast<float>(rand()/static_cast<float>((RAND_MAX+1))) - modelSize;
            tempY = 2*modelSize*static_cast<float>(rand()/static_cast<float>((RAND_MAX+1))) - modelSize;
            tempZ = 2*modelSize*static_cast<float>(rand()/static_cast<float>((RAND_MAX+1))) - modelSize;
        } while((tempX*tempX + tempY*tempY + tempZ*tempZ) > radiusSquared);         // generated point is outside of sphere, discard
        
        CStar*  pTemp = new CStar(tempX, tempY, tempZ, false /* , m_dist*/);        

        m_vecStars.push_back(pTemp);
    }
}


/*
 * insure that two stars are not too close together
 * given the estimate of 0.130262 star per cubic parsec (3.26 cubic light years); minimum distance between stars is 1.25 parsec's (4 light years)
 */
 /**********************************************************************************************************************************************************************************
  * function  :  clearStarLocs
  *
  * abstract  :
  *
  * parameters:
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  ************************************************************************************************************************************************************************************/
void nightSky::cleanStarLocs()
{
    m_statusbar->showMessage(QString("Starting to deconflict star locations"));
    // TODO : this is naive, there should be a better way of doing this

    // for(ndx = 0; ndx < cnt-1; ndx++)
    //   for(jdx = ndx + 1; jdx < cnt; jdx++)
    //     if((star[ndx].radius - star[jdx].radius) < 4       // potential 'hit'
    //         // calculate distance using rectangular coordinates
    //         // 'delete second point if too close  ?? -probably should set to a default value so we do mess with ndx & jdx
    m_statusbar->showMessage(QString("deconfliction done"));
}



 /**********************************************************************************************************************************************************************************
  * function  : debugStarLocs
  *
  * abstract  :  This function positions stars at well defined locations to test the rendering and transformation engines.  We set the star type to 'O' and it mass to 17 for easy
  *              visulation.  These test cases are now stored in files since the saving and reading functions are now implemented.  We assume a radius of 20 parsecs.  Coordinates 
  *              are <radius, latitude, longitude>  where latitude is in range [0, pi] and longitude is in range [-pi, pi].
  *
  * parameters: void 
  *
  * returns   : void
  *
  * written   : GKHuber Apr 2015
  *                     Dec 2021 - Function is depricated now that reading and writing to a file is implemented
  ************************************************************************************************************************************************************************************/
void nightSky::debugStarLocs()
{
    // test case 1: draws points at grid points....
    m_vecStars.push_back(new CStar(             0,             0,              20, 0, 17.0));     // (-90,   0)
    m_vecStars.push_back(new CStar( 14.1421356237,	           0,	14.1421356237, 0, 17.0));   // (-45,   0)
    m_vecStars.push_back(new CStar(            10,	          10,	14.1421356237, 0, 17.0));   // (-45,  45)   
    m_vecStars.push_back(new CStar(             0, 14.1421356237,	14.1421356237, 0, 17.0));       // (-45,  90)
    m_vecStars.push_back(new CStar(           -10,	          10,	14.1421356237, 0, 17.0));   // (-45, 135)
    m_vecStars.push_back(new CStar(-14.1421356237,	           0,	14.1421356237, 0, 17.0));   // (-45, 180)
    m_vecStars.push_back(new CStar(           -10,	         -10,	14.1421356237, 0, 17.0));   // (-45,-135)
    m_vecStars.push_back(new CStar(             0,-14.1421356237,	14.1421356237, 0, 17.0));       // (-45, -90)
    m_vecStars.push_back(new CStar(            10,	         -10,	14.1421356237, 0, 17.0));   // (-45, -45)
    m_vecStars.push_back(new CStar( 14.1421356237,	           0,	14.1421356237, 0, 17.0));   // (-45,   0)
    m_vecStars.push_back(new CStar(            20,	           0,	            0, 0, 17.0));   // (  0,   0)
    m_vecStars.push_back(new CStar( 14.1421356237, 14.1421356237,            	0, 0, 17.0));       // (  0,  45)
    m_vecStars.push_back(new CStar(             0,	          20,	            0, 0, 17.0));   // (  0,  90)
    m_vecStars.push_back(new CStar(-14.1421356237, 14.1421356237,	            0, 0, 17.0));       // (  0, 135)
    m_vecStars.push_back(new CStar(           -20,	           0,	            0, 0, 17.0));   // (  0, 180)
    m_vecStars.push_back(new CStar(-14.1421356237,-14.1421356237,	            0, 0, 17.0));       // (  0,-135)
    m_vecStars.push_back(new CStar(             0,	         -20,	            0, 0, 17.0));   // (  0, -90)
    m_vecStars.push_back(new CStar( 14.1421356237,-14.1421356237,	            0, 0, 17.0));       // (  0, -45)
    m_vecStars.push_back(new CStar(            20,	           0,	            0, 0, 17.0));   // (  0,   0)
    m_vecStars.push_back(new CStar( 14.1421356237,	           0,  -14.1421356237, 0, 17.0));   // ( 45,   0)
    m_vecStars.push_back(new CStar(            10,	          10,  -14.1421356237, 0, 17.0));   // ( 45,  45)
    m_vecStars.push_back(new CStar(             0, 14.1421356237,  -14.1421356237, 0, 17.0));     // ( 45,  90)
    m_vecStars.push_back(new CStar(           -10,	          10,  -14.1421356237, 0, 17.0));   // ( 45, 135)
    m_vecStars.push_back(new CStar(-14.1421356237,	           0,  -14.1421356237, 0, 17.0));   // ( 45, 180)
    m_vecStars.push_back(new CStar(           -10,	         -10,  -14.1421356237, 0, 17.0));   // ( 45,-135)
    m_vecStars.push_back(new CStar(             0,-14.1421356237,  -14.1421356237, 0, 17.0));     // ( 45, -90)
    m_vecStars.push_back(new CStar(            10,	         -10,  -14.1421356237, 0, 17.0));   // ( 45, -45)
    m_vecStars.push_back(new CStar( 14.1421356237,	           0,  -14.1421356237, 0, 17.0));   // ( 45,   0)
    m_vecStars.push_back(new CStar(             0,	           0,             -20, 0, 17.0));   // ( 90,   0)

    // test case 2 - horizontal arrow pointing at prime meridian northren hemisphere, eastern side.
    //m_vecStars.push_back(new CStar(20, 1.832595, -0.523599, 0, 17, true));  // 15N, 30W
    //m_vecStars.push_back(new CStar(20, 1.832595, -0.558505, 0, 17, true));  // 15N, 32W
    //m_vecStars.push_back(new CStar(20, 1.832595, -0.593412, 0, 17, true));  // 15N, 34W
    //m_vecStars.push_back(new CStar(20, 1.832595, -0.628319, 0, 17, true));  // 15N, 36W
    //m_vecStars.push_back(new CStar(20, 1.850048, -0.523599, 0, 17, true));  // 17N, 30W
    //m_vecStars.push_back(new CStar(20, 1.815141, -0.523599, 0, 17, true));  // 13N, 30W

    // test case 3 - horizontal arrow pointing at prime meridian southren hemisphere, western side.
    //m_vecStars.push_back(new CStar(20, 0.785398, 0.523599, 0, 17, true));  // 45S, 30E
    //m_vecStars.push_back(new CStar(20, 0.785398, 0.558505, 0, 17, true));  // 45S, 32E
    //m_vecStars.push_back(new CStar(20, 0.785398, 0.593412, 0, 17, true));  // 45S, 34E
    //m_vecStars.push_back(new CStar(20, 0.785398, 0.628319, 0, 17, true));  // 45S, 36E
    //m_vecStars.push_back(new CStar(20, 0.802851, 0.523599, 0, 17, true));  // 43S, 30E
    //m_vecStars.push_back(new CStar(20, 0.767944, 0.523599, 0, 17, true));  // 47S, 30E

    // test case 4 - verticle arrow pointing south on prime meridian, in northren hemisphere
    //m_vecStars.push_back(new CStar(20, 1.832595, 0, 0, 17, true));            // 15N, 0W
    //m_vecStars.push_back(new CStar(20, 1.867501, 0, 0, 17, true));            // 17N, 0W
    //m_vecStars.push_back(new CStar(20, 1.902408, 0, 0, 17, true));            // 19N, 0W
    //m_vecStars.push_back(new CStar(20, 1.937314, 0, 0, 17, true));            // 21N, 0W
    //m_vecStars.push_back(new CStar(20, 1.972221, 0, 0, 17, true));            // 23N, 0W
    //m_vecStars.push_back(new CStar(20, 1.832595, 0.0349065, 0, 17, true));    // 15N, 2E
    //m_vecStars.push_back(new CStar(20, 1.832595, -0.0349065, 0, 17, true));   // 15N, 2W

    // test case 5 - verticle arrow pointing north on prime meridian, in southern hemisphere
    //m_vecStars.push_back(new CStar(20, 1.308996, 0, 0, 17, true));            // 15S, 0W
    //m_vecStars.push_back(new CStar(20, 1.274089, 0, 0, 17, true));            // 17S, 0W
    //m_vecStars.push_back(new CStar(20, 1.239182, 0, 0, 17, true));            // 19S, 0W
    //m_vecStars.push_back(new CStar(20, 1.204276, 0, 0, 17, true));            // 21S, 0W
    //m_vecStars.push_back(new CStar(20, 1.169369, 0, 0, 17, true));            // 23S, 0W
    //m_vecStars.push_back(new CStar(20, 1.308996, 0.0349065, 0, 17, true));    // 15N, 2E
    //m_vecStars.push_back(new CStar(20, 1.308996, -0.0349065, 0, 17, true));   // 15N, 2W

    populateList();             // populate all views.
    drawStarField();
}
 


/**********************************************************************************************************************************************************************************
 * function  : updateList
 *
 * abstract  : Lets the star display dialog know that it needs to update it's list
 *
 * parameters: void
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void nightSky::populateList()
{
    emit updateList();
}



/**********************************************************************************************************************************************************************************
 * function  : drawStarField
 *
 * abstract  :
 *
 * parameters:
 *
 * returns   : void
 *
 * written   : GKHuber Apr 2015
 ************************************************************************************************************************************************************************************/
void nightSky::drawStarField()
{
    int cnt = 1;
    int cntStars = m_vecStars.size();
    std::vector<CStar*>::iterator    iter = m_vecStars.begin();

    while(m_vecStars.end() != iter)
    {
        float  lat = (*iter)->getLat();
        float  lon = (*iter)->getLon();

        if(cntStars > 300)
        {
            if((cnt != 0) && (cnt % 200))
            {
                QString strStatus = QString("calculating projection coordinates %1 / %2 stars (%3 %)").arg(cnt).arg(cntStars). arg((100*(cnt))/cntStars);
                m_statusbar->showMessage(strStatus);
                QEventLoop   evtLoop(this);
                evtLoop.processEvents(QEventLoop::ExcludeUserInputEvents);           // let any queued events be processed
            }
        }

        CStar  star = *(*iter);
        std::cout << (star);

        (*iter)->draw(m_projection, m_pScene);
        ++iter;
        ++cnt;
    }

    m_statusbar->showMessage(QString("done drawing star map"));
}
