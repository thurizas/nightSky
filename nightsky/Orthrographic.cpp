#include "Orthographic.h"
#include "constants.h"
#include "CadLib.h"


#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>

// r is radius of projection, default is 400
COrthographic::COrthographic(float r) : m_fRadius(r)
{
    calcEquator(12);                     // default value is for a 30-degree separation.
}


COrthographic::~COrthographic()
{


}



/**********************************************************************************************************************************************************************************
 * function  : calcEquator
 *
 * abstract  : This function builds a vector of points corresponding to where lines of longitude cross the equator.  This is used in exporting the orthographic map to a DXF file. 
 *
 * parameters: segs -- [in] number of segments used, by default this is 12 (i.e. a line of longitude every 30 degrees)
 *
 * returns   : void.  However it does modify the member variable m_vecEquator.
 *
 * written   : GKHuber Dec 2021
 ************************************************************************************************************************************************************************************/
void COrthographic::calcEquator(int segs)
{
    int deltaLong = (360 / segs);

    if (12 != segs)             // if not using the default size...
    {
        std::vector<ppointT>::iterator    iter = m_vecEquator.begin();
        while (m_vecEquator.end() != iter)
        {
            delete (*iter);
            *iter = nullptr;
            ++iter;
        }

        m_vecEquator.clear();
    }

    for (int lon = 180; lon >= -180; lon -= deltaLong)
    {
        ppointT temp = new pointT;
        temp->lat = 0;                // point on equation
        temp->lon = DEG2RAD * lon;
        temp->projX = 0;
        temp->projY = 0;

        m_vecEquator.push_back(temp);
    }

}

/**********************************************************************************************************************************************************************************
 * function  : convert
 *
 * abstract  : This function converts a coordinate in spherical polar (i.e. latatude, longatude) into cartesian.  The northern hemisphere is drawn on the left of the map while 
 *             the southern is drawn on the right.  The conversions from spherical polar to cartesian is given by
 *             
 *             r = R * cos(lat)                            where R is the radius of the projection
 *             x = r*cos(lon) = R*cos(lat)*cos(lon)
 *             y = r*sin(lon) = R*cos(lat)*sin(lon)
 * 
 * parameters: lat -- [in] latitude of the object, range is [pi/2, -pi/2] positive values are in the northren hemisphere
 *             lng -- [in] longitude of the object, range is [pi, -pi] positive values are in the western hemisphere
 * 
 * returns   : std::pair<int, int> the cartesian coordinates of the point
 * 
 * written   : GKHuber Dec 2021
 ************************************************************************************************************************************************************************************/
std::pair<int, int> COrthographic::convert(float lat, float lng)
{
    float x, y;

   //QPointF lcenter(1120 / 2 - m_fRadius, 755 / 2);                      // set center of left hand map...
   //QPointF rcenter(1120 / 2 + m_fRadius, 755 / 2);                      // set center of right hand map...
   QPointF lcenter(m_fRadius, m_fRadius);
   QPointF rcenter(3 * m_fRadius, m_fRadius);
    
    // calculate radius of sphere at the given latatude
    float r = m_fRadius * cos(lat);

    //if (lat >= 0)                                                        // object in northren hemisphere 
    if(lat <= 0 )
    {
        x = r * cos(lng) + lcenter.x();
        y = r * sin(lng) + lcenter.y();
    }
    else                                                                 // object in right hemisphere
    {
        x = r * cos(lng) + rcenter.x();
        y = r * sin(lng) + rcenter.y();
    }

    return std::pair<int, int>((int)x, (int)y);
}



/**********************************************************************************************************************************************************************************
 * function  : drawGrid
 *
 * abstract  : This function draws the map grid on the output window.  The grid is divided into two halves, with the northern hemisphere on the left and the southern hemisphere
 *             on the right.  The conversion rules are the same as in convert.
  *
 * parameters: pGrid -- [in] pointer to an object of type CGrid which represents the grid points on a sphere.  Internally the points are maintained as a vector of qreal's layout
 *                      in 'slice' major form.
 *             pScene -- [in/out] pointer to an object of type QGraphicsScene that contains the QGraphicsItem's making up the scene.  It is eventually rendered onto the QGraphicsView
 *                       to display the image.
 *
 * returns   : void
 *
 * written   : GKHuber Nov 2021
 ************************************************************************************************************************************************************************************/
void COrthographic::drawGrid(CGrid* pGrid, QGraphicsScene* pScene)
{
    std::vector<ppointT> vecPoints = pGrid->getGrid();

    // set up a pen for the grid system, we want a light-grey pen 
    QPen   pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    int lonSteps = pGrid->segments() + 1;
    int latSteps = pGrid->slices() + 1;

    // draw northern hemisphere, left side of drawing  
    QPointF  center(m_fRadius, m_fRadius);                        // set center of spheres...
    // draw latatide lines -- these are curved lines, and occur in groups of slices length.
    for(int slice = 0; slice < (pGrid->slices()/2)+1; slice++)
    {
        QPolygonF    pline;
        QPainterPath ppath;

        float   r = m_fRadius * cos(vecPoints.at((size_t)slice * (size_t)lonSteps)->lat);

        if (fabs(r) < EPISILON)               // at one of the poles
            continue;
        else
        {
            for (int segment = 0; segment <= pGrid->segments(); segment++)
            {
                size_t ndx = (size_t)slice * (size_t)lonSteps + (size_t)segment;

                qreal x1 = r * cos(vecPoints.at(ndx)->lon);
                qreal y1 = r * sin(vecPoints.at(ndx)->lon);

                pline.append(QPointF(x1, y1) + center);
            }

            ppath.addPolygon(pline);
            pScene->addPath(ppath);
        }
    }

    // draw longitude lines -- these are straight lines 
    for (int segment = 0; segment < pGrid->segments(); segment++)                       // iterate over each segment first...
    {
        QPolygonF    pline;
        QPainterPath ppath;

        for (int slice = 0; slice <= pGrid->slices(); slice++)
        {
            // calculate radius to use
            float r = m_fRadius * cos(vecPoints.at((size_t)slice * (size_t)lonSteps)->lat);

            if (fabs(r) < EPISILON)
            {
                pline.append(QPointF(0, 0) + center);
            }
            else
            {
                int ndx = slice * lonSteps + segment;
                qreal x1 = r * cos(vecPoints.at(ndx)->lon);
                qreal y1 = r * sin(vecPoints.at(ndx)->lon);

                pline.append(QPointF(x1, y1) + center);

                qDebug() << "using point: (" << segment << ", " << slice << ") point at: " << ndx;
            }
        }        
        
        ppath.addPolygon(pline);
        pScene->addPath(ppath);
    }

    // draw southern hemisphere, right side of drawing
    center.setX(3.0 * m_fRadius);                          // adjust center of sphere....

    // draw latatide lines -- these are curved lines, and occur in groups of slices length.
    //for (int slice = 0; slice < pGrid->slices(); slice++)
    for(int slice = pGrid->slices()/2; slice <= pGrid->slices(); slice++)
    {
        QPolygonF    pline;
        QPainterPath ppath;

        float   r = m_fRadius * cos(vecPoints.at((size_t)slice * (size_t)lonSteps)->lat);

        if (fabs(r) < EPISILON)               // at one of the poles
            continue;
        else
        {
            for (int segment = 0; segment <= pGrid->segments(); segment++)
            {
                size_t ndx = (size_t)slice * (size_t)lonSteps + (size_t)segment;

                qreal x1 = r * cos(vecPoints.at(ndx)->lon);
                qreal y1 = r * sin(vecPoints.at(ndx)->lon);

                pline.append(QPointF(x1, y1) + center);
            }

            ppath.addPolygon(pline);
            pScene->addPath(ppath);
        }
    }

    // draw longitude lines -- these are straight lines 
    for (int segment = 0; segment < pGrid->segments(); segment++)                       // iterate over each segment first...
    {
        QPolygonF    pline;
        QPainterPath ppath;

        for (int slice = 0; slice <= pGrid->slices(); slice++)
        {
            // calculate radius to use
            float r = m_fRadius * cos(vecPoints.at((size_t)slice * (size_t)lonSteps)->lat);

            if (fabs(r) < EPISILON)
            {
                pline.append(QPointF(0, 0) + center);
            }
            else
            {
                int ndx = slice * lonSteps + segment;
                qreal x1 = r * cos(vecPoints.at(ndx)->lon);
                qreal y1 = r * sin(vecPoints.at(ndx)->lon);

                pline.append(QPointF(x1, y1) + center);

                qDebug() << "using point: (" << segment << ", " << slice << ") point at: " << ndx;
            }
        }

        ppath.addPolygon(pline);
        pScene->addPath(ppath);
    }
}



/**********************************************************************************************************************************************************************************
 * function  : drawGrid
 *
 * abstract  : This function writes the grid out to a DXF (drawing eXchange foramt) file.  This allows the map to be exchanged between various programs.  This is an overloaded 
 *             form of the above function
 * 
 * parameters: pGrid -- [in] pointer to an object of type CGrid which represents the grid points on a sphere.  Internally the points are maintained as a vector of qreal's layout
 *                      in 'slice' major form.
 *            pdxffile -- [in/out] pointer to an object of type CDxfFileWrite that contains the objects to display in DXF (drawing eXchange format).  
 *
 * returns   : void
 *
 * written   : GKHuber Dec 2021
 ************************************************************************************************************************************************************************************/
void COrthographic::drawGrid(CGrid* pGrid, CDxfFileWrite* pdxffile)
{
    std::vector<ppointT> vecPoints = pGrid->getGrid();

    calcEquator(pGrid->segments());

    //int lonSteps = pGrid->segments() + 1;
    //int latSteps = pGrid->slices() + 1;

    
    QPoint lcenter = QPoint(m_fRadius, m_fRadius);                // determine center of each map
    QPoint rcenter = QPoint(3 * m_fRadius, m_fRadius);

    // draw latitude lines
    for (int slice = 0; slice < (pGrid->slices() / 2) + 1; slice++)       // draw latatide lines 
    {
        float   r = m_fRadius * cos(vecPoints.at((size_t)slice * (size_t)(pGrid->segments() + 1)/*lonSteps*/)->lat);

        if (fabs(r) < EPISILON)               // at one of the poles
            continue;
        else
        {
            pdxffile->Circle(lcenter.x(), lcenter.y(), r);              // draw northern hemisphere....
            pdxffile->Circle(rcenter.x(), rcenter.y(), r);              // draw southern hemisphere....
        }
    }

    //draw longitude lines
    for (int ndx = 0; ndx < pGrid->segments(); ndx++)
    {
        qreal x1 = m_fRadius* cos(vecPoints.at(ndx)->lon);
        qreal y1 = m_fRadius * sin(vecPoints.at(ndx)->lon);

        pdxffile->Line(x1 + lcenter.x(), y1 + lcenter.y(), lcenter.x(), lcenter.y());               // draw northern hermisphere...
        pdxffile->Line(x1 + rcenter.x(), y1 + rcenter.y(), rcenter.x(), rcenter.y());               // draw southern hermisphere...
    }
}