#ifndef NIGHSKY_H
#define NIGHSKY_H


#include <QMainWindow>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>
//#include <QGraphicsView>
#include <QGraphicsScene>

#include "starView.h"
#include "Projection.h"
#include "3DDisplay.h"
#include "evDist.h"
#include "starsDlg.h"
#include "helpDlg.h"


class QTableWidget;
class CProjection;
class QResizeEvent;
class CGrid;
class CStar;


/*
 *
 *
 * from http://en.wikipedia.org/wiki/Stellar_density
 * most stars visible from earth are with-in 500 parsec's of Sol
 * one parsec is approximately 3.26 light years
 *
 * from http://abyss.uoregon.edu/~js/ast122/lectures/lec26.html
 * in neighborhood of Sol, the sellar density is about 1 star per cubic parsec
 * at galactic core (about 100 parsec's from galactic center) density rises to 100 stars per cubic parsec
 *
 * absolute luminosity (M)     # stars (d < 20 parsec)    density (stars per cubic parsec)
 *   -1                                1
 *    0                                4
 *    1                                6
 *    2                               25                  0.000746
 *    3                               50                  0.001492
 *    4                               90                  0.002686
 *    5                              110                  0.003283
 *    6                              125                  0.003730
 *    7                              105                  0.003133
 *    8                              140                  0.004178
 *    9                              155                  0.004626
 *   10                              240                  0.007162
 *   11                              330                  0.009848
 *   12                              490                  0.014693
 *   13                              410                  0.012235
 *   14                              160                  0.004775
 *   15                              750                  0.022382
 *   16                              360                  0.010743
 *   17                              415                  0.012385
 *   18                              410                  0.012235
 *  totals                          4365                  0.130262
 *
 * from https://www.youtube.com/watch?v=hG1of0MroM8
 * we assume the density is 0.004 stars per cubic light year (1 parsec = 3.26 light years, 
 *  class |  frequency |   mass, (solar masses) |   temperature, kelvin |  color
 *    O   |  0.00003%  |      \ge 16            |   \ge 30,000          |  9BB0FF
 *    B   |  0.13%     |      2.10 - 16         |   10,000 - 30,000     |  AABFFF
 *    A   |  0.6%      |      1.40 - 2.10       |   7,500  - 10,000     |  CAD7FF
 *    F   |  3%        |      1.04 - 1.40       |   6,000  -  7,500     |  F8F7FF
 *    G   |  7.6%      |      0.80 - 1.04       |   5,200  -  6,000     |  FFF4EA
 *    K   |  12.1%     |      0.45 - 0.80       |   3,700  -  5,200     |  FFD2A1
 *    M   |  76.45%    |      0.08 - 0.45       |   2,400  -  3,700     |  FFCC6F
 *
 *   radius = M^{0.5) if M \ge M_{sol}  
 *            M^{0.8} if M <  M_{sol}
 *
 *   Luminosity = M^{3.5}
 *   Temperature, T = (L/R^2)^{0.25}
 */


class nightSky : public QMainWindow
{
    Q_OBJECT

public:

    enum projType { PROJ_MOLLWEIDE = 1, PROJ_ORTHRO };
    explicit nightSky(QWidget *parent = 0);
    ~nightSky();

    void resizeEvent(QResizeEvent*);

    bool    getDisplayGrid() { return m_displayGrid; }
    int     getLatSpacing() { return m_nLatSpacing; }
    int     getLonSpacing() { return m_nLonSpacing; }
    int     getProjType() { return m_nProjType; }
    int     getProjRadius() { return m_nProjRadius; }
    float   getStellarDensity() { return m_fDensity; }
    float   getSimRadius() { return  m_fRadius; } 

    std::vector<CStar*>* getStarData() { return &m_vecStars; }

signals:
    void updateList();
    void clearList();

private slots:
    void onOpen();
    void onNew();
    void onClose();
    void onExport();
    void onSave();
    void onSaveAs();
    void onExit();
    void onPrefs();
    void onView3D();
    void onZoomOut();
    void onZoomIn();
    void onViewRedraw();
    void onViewStars();
    void onDispLum();
    void onHelp();
    void onAbout();

private:
    void setupUI();
    void setupMenu();
    void setupActions();
    void readSettings();
    void writeSettings();
    void init();

    int  checkSave();
    void initInterface();
    void genStarLocs();
    void debugStarLocs();
    void cleanStarLocs();
    void populateList();
    void drawStarField();

    void closeEvent(QCloseEvent*);

    // settings
    bool           m_displayGrid;    // display grid, default is true
    int            m_nLatSpacing;     // set size of latitude, defalut is 15 degrees
    int            m_nLonSpacing;    // set step of longitude, defalut is 30 degrees
    int            m_nProjType;      // set projection type to use, default is one, mollweide
    int            m_nProjRadius;    // radius of projection, default is 400
    int            m_nCntStarts;     // set number of star to generate, default is 1000
    float          m_fDensity;       // stellar density, default is 0.130262 star per cubic parsec (3.26 cubic light years)
    float          m_fRadius;        // radius to simulate stars in, default is 20 parsecs

    // menu actions...
    QAction*        m_pFileOpen;
    QAction*        m_pFileNew;
    QAction*        m_pFileClose;
    QAction*        m_pFileExport;
    QAction*        m_pFileSave;
    QAction*        m_pFileSaveAs;
    QAction*        m_pFileExit;

    QAction*        m_pViewZoomIn;
    QAction*        m_pViewZoomOut;
    QAction*        m_pViewRedraw;
    QAction*        m_pView3D;
    QAction*        m_pViewStars;
    QAction*        m_pViewPrefs;

    QAction*        m_pDebugLumDist;

    QAction*        m_pHelpHelp;
    QAction*        m_pHelpAbout;

    // ui components
    QMenuBar*       m_menubar;
    QStatusBar*     m_statusbar;
    //QGraphicsView*  m_canvas;
    starView*       m_canvas;
    helpDlg*        m_pHelp;
    starsDlg*       m_pStars;
    //C3DDisplay*     m_p3Dview;

    // variables for state....
    std::vector<CStar*>  m_vecStars;
    CProjection*         m_projection;                   // pointer to projection in use
    CGrid*               m_pGrid;
    QGraphicsScene*      m_pScene;
    CGEVDist*            m_dist;
    QString              m_fileName;
    bool                 m_bDirty;
    bool                 m_b3DShown;

};

#endif // NIGHSKY_H
