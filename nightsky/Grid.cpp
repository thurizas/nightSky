#include "Grid.h"
#include "Projection.h"
#include "constants.h"
#include "CadLib.h"                       // for dxf export support


#include <iostream>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>



/***********************************************************************************************************************
 * function  : ctor
 * 
 * abstract  : this function build a grid of points on the surface of a sphere.  THe default step sizes is 15-degree steps
 *             for latitude and in 30 degree steps for longitude.  The points are store in spherical polar coordinates with
 *             an assumed value of one for the radius.  The values are stored in a vector by 'slice'-major form, ie all the
 *             points for 90N are stored first, then the points for 75N (assuing default step values) and so on till 90S.  
 *             Positive values represent north, and south represented by negative values.
 * 
 * parameters: cntSlices -- [in] number of slices (horizontal sections) that the sphere is divied into
 *             cntSegments -- [in] number of segments (verical sections) that the sphere is divided into
 * 
 * return    : nothing
 ***********************************************************************************************************************/
//CGrid::CGrid(void)
CGrid::CGrid(int cntSlices, int cntSegments) : m_cntSlices(cntSlices), m_cntSegments(cntSegments)
{
    int deltaLat = (180 / (cntSlices));               // 180 : total range for latitude values 
    int deltaLong = (360 / (cntSegments));            // 360 : total range for longitude values

    for(int lat = 90; lat >= -90; lat -= deltaLat)
    {
        for(int lon = 180; lon >= -180; lon -= deltaLong)
        {
            ppointT temp = new pointT;
            temp->lat = DEG2RAD*lat;
            temp->lon = DEG2RAD*lon;
            temp->projX = 0;
            temp->projY = 0;

            m_vecPoints.push_back(temp);


        }
    }
}



/***********************************************************************************************************************
 * function  : dtor
 *
 * abstract  : performs clean-up opertions on the vector of point we created.
 *
 * parameters: none
 *
 * return    : nothing
 ***********************************************************************************************************************/
CGrid::~CGrid(void)
{
    if(m_vecPoints.size() > 0)
    {
        std::vector<ppointT>::iterator     iter = m_vecPoints.begin();

        while(m_vecPoints.end() != iter)
        {
            delete *iter;
            *iter = NULL;

            ++iter;
        }
    }
}



/***********************************************************************************************************************
 * function  : convert
 *
 * abstract  : Converts the points on the grid into cartesian coordinates based on the projection that we are using
 *
 * parameters: pproj -- [in] pointer to the project that we are using to display the map in.  Each projection class needs
 *                           to have a method to convert from spherical polar coordinates to cartesian.
 * 
 * return    : nothing
 ***********************************************************************************************************************/
void CGrid::convert(CProjection* pproj)
{
    if(m_vecPoints.size() > 0)
    {
        std::vector<ppointT>::iterator     iter = m_vecPoints.begin();

        while (m_vecPoints.end() != iter)
        {
            std::pair<int, int> loc = pproj->convert((*iter)->lat, (*iter)->lon);

            (*iter)->projX = loc.first;
            (*iter)->projY = loc.second;

            ++iter;
        }
    }
}



/***********************************************************************************************************************
 * function  : draw
 *
 * abstract  : This draws the 2-dimensional representation of the grid based on the projection we are using.  
 *
 * parameters: pScene -- [in] pointer to the graphics scene we are drawing to.
 *
 * return    : nothing
 ***********************************************************************************************************************/
//void CGrid::draw(QGraphicsScene* pScene) 
//{  
//    // set up a pen for the grid system, we want a light-grey pen 
//    QPen   pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
//
//    for(int ndx = 0; ndx < 169; ndx += 13)
//    {
//        // draw the latitude lines, use the two end points, i.e. (lat, -180) and (lat, +180) to draw line
//        qreal x1 = m_vecPoints.at(ndx)->projX, x2 = m_vecPoints.at(ndx+12)->projX;
//        qreal y1 = m_vecPoints.at(ndx)->projY, y2 = m_vecPoints.at(ndx+12)->projY;
//
//        pScene->addLine(x1, y1, x2,y2, pen);
//    }
//
//    for(int ndx = 0; ndx < 13; ndx++)
//    {
//        // draw the longitude lines, construct a path and then draw the path.
//        QPolygonF     pline;
//        QPainterPath  ppath;
//
//        for(int jdx = 0; jdx < 13; jdx++)
//        {
//            qreal x = m_vecPoints.at(ndx+13*jdx)->projX;
//            qreal y = m_vecPoints.at(ndx+13*jdx)->projY;
//
//            pline.append(QPointF(x,y));
//        }
//
//        ppath.addPolygon(pline);
//        pScene->addPath(ppath, pen);
//    }
//}


   //dxffile.Line(1.2, 3.3, 7.5, 7.7);       // draw a line
//void CGrid::draw(CDxfFileWrite* pdxffile)
//{
//    for(int ndx = 0; ndx < 169; ndx += 13)
//    {
//        // draw the latitude lines, use the two end points, i.e. (lat, -180) and (lat, +180) to draw line
//        qreal x1 = m_vecPoints.at(ndx)->projX, x2 = m_vecPoints.at(ndx+12)->projX;
//        qreal y1 = m_vecPoints.at(ndx)->projY, y2 = m_vecPoints.at(ndx+12)->projY;
//
//        pdxffile->Line(x1, y1, x2,y2);
//    }
//
//    for(int ndx = 0; ndx < 13; ndx++)
//    {
//        for(int jdx = 0; jdx < 12; jdx++)
//        {
//            qreal curX = m_vecPoints.at(ndx+13*jdx)->projX;
//            qreal curY = m_vecPoints.at(ndx+13*jdx)->projY;
//
//            qreal nextX = m_vecPoints.at(ndx+13*jdx+13)->projX;
//            qreal nextY = m_vecPoints.at(ndx+13*jdx+13)->projY;
//
//            pdxffile->Line(curX, curY, nextX, nextY);
//        }
//    }
//}

              


void CGrid::printGridPoints(bool showProj)
{
    if(m_vecPoints.size() > 0)
    {
        std::vector<ppointT>::iterator    iter = m_vecPoints.begin();

        while(m_vecPoints.end() != iter)
        {
            QString    msg=QString("point is: (%1, %2)").arg((*iter)->lat).arg((*iter)->lon);

            if(showProj)
            {
                QString  strProj = QString("converst to: (%1, %2)").arg((*iter)->projX).arg((*iter)->projY);
                msg += strProj;
            }

            qDebug() << msg;

            ++iter;
        }
    }
}

