#include "Mollweide.h"
#include "constants.h" 
#include "CadLib.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPainterPath>


/* converts from latitude and longitude to x- and y- coordinates via the following 
 * equations:
 *
 *       R*2*sqrt(2)
 *  x = ------------ (l - l_0)cos(t)                   -2R*sqrt(2) <= x <= 2R*sqrt(2)
 *         PI
 *
 *   y = R*sqrt(2)*sin(t)                              -R*sqrt(2) <= y <= R*sqrt(2)
 *
 *   where 2*t + sin(2*t) = PI*sin(p)                  auxilliary equation.
 *
 * where l is the longitude, l_0 is the central meridian, p is the latitude and R is radius of the globe.
 * and t is an auxillary angle.
 *
 * The auxilliary equation can be solved by Newton-Raphson iteration.
 *
 *     if latitude is +/- pi/2  then t is +/- pi/2
 *
 *     else     t_0 = lat
 *              t_{n+1} = t_n - ((2*t_n + sin(2*t_n) - pi*sin(lat))/(2 + 2*cos(2*t_n)))
 *
 */
CMollweide::CMollweide(float r ) : m_fRadius(r), m_fCoeff(r*sqrt(2.0))
{
}


CMollweide::~CMollweide(void)
{
}


/* precondition : expect lat and lon to be passed in as radians */
std::pair<int, int> CMollweide::convert(float lat, float lon)
{
    float theta;                                                         // solution to auxillary equation....

    if((fabs(lat - PI/2) < EPISILON) || (fabs(PI/2 + lat) < EPISILON))    // base case for auxillary equation
    { 
        theta = lat;
    }
    else
    {
        theta = solveAux(lat, lat);              // iterate on auxillary equation
    }

    float x = 2*m_fCoeff * lon * cos(theta) / PI;
    float y = m_fCoeff * sin(theta);


     return std::pair<int, int>(x,y);
 }



float CMollweide::solveAux(float lat_r, float guess)
{
    float newguess;

    newguess = guess - ((2*guess + sin(2*guess) - PI*sin(lat_r))/(2 + 2*cos(2*guess)));

    if(fabs(guess - newguess) <= 200*EPISILON)
    {
        return newguess;
    }
    else
        return solveAux(lat_r, newguess);
}



void CMollweide::drawGrid(CGrid* pGrid, QGraphicsScene* pScene)
{
    std::vector<ppointT> vecPoints = pGrid->getGrid();


    int cntSlices = pGrid->slices();
    int cntSegments = pGrid->segments();

    // set up a pen for the grid system, we want a light-grey pen 
    QPen   pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);


    // draw latitude lines - horizontal lines use point on +180 and -180 lines, i.e. first and last point in each slice.
    for (int ndx = 0; ndx < (cntSlices + 1)*(cntSegments +1); ndx += (cntSegments+1))
    {
        // draw the latitude lines, use the two end points, i.e. (lat, -180) and (lat, +180) to draw line
        qreal x1 = vecPoints.at(ndx)->projX, x2 = vecPoints.at(ndx + cntSegments/*12*/)->projX;
        qreal y1 = vecPoints.at(ndx)->projY, y2 =vecPoints.at(ndx + cntSegments/*12*/)->projY;

        pScene->addLine(x1, y1, x2, y2, pen);
    }

    // draw longitude lines - curved lines, use a path and need to use each point on a latitude line that defines a longitude
    for (int ndx = 0; ndx < (cntSegments+1); ndx++)                         // iterate over segments
    {
        QPolygonF     pline;
        QPainterPath  ppath;

        for (int jdx = 0; jdx < (cntSlices+1)/*13*/; jdx++)                       // iterate over slices
        {
            qreal x =vecPoints.at(ndx + jdx*(cntSegments + 1)/*13*/ )->projX;
            qreal y = vecPoints.at(ndx + jdx*(cntSegments + 1)/*13*/ )->projY;

            pline.append(QPointF(x, y));
        }

        ppath.addPolygon(pline);

        pScene->addPath(ppath, pen);
    }
}



void CMollweide::drawGrid(CGrid* pGrid, CDxfFileWrite* pdxffile)
{
    std::vector<ppointT> vecPoints = pGrid->getGrid();


    int cntSlices = pGrid->slices();
    int cntSegments = pGrid->segments();

    // draw latitude lines - horizontal lines use point on +180 and -180 lines, i.e. first and last point in each slice.
    for (int ndx = 0; ndx < (cntSlices + 1) * (cntSegments + 1); ndx += (cntSegments + 1))
    {
        // draw the latitude lines, use the two end points, i.e. (lat, -180) and (lat, +180) to draw line
        qreal x1 = vecPoints.at(ndx)->projX, x2 = vecPoints.at(ndx + cntSegments)->projX;
        qreal y1 = vecPoints.at(ndx)->projY, y2 = vecPoints.at(ndx + cntSegments)->projY;

        pdxffile->Line(x1, y1, x2, y2);
    }

    // draw longitude lines - curved lines, use a path and need to use each point on a latitude line that defines a longitude
    for (int ndx = 0; ndx < (cntSegments + 1); ndx++)                         // iterate over segments
    {
        for (int jdx = 0; jdx < (cntSlices); jdx++)                           // iterate over slices
        {
            int curNdx = ndx + (cntSegments + 1)*jdx;
            int nxtNdx = curNdx + (cntSegments + 1);

            qreal curX = vecPoints.at(curNdx)->projX;
            qreal curY = vecPoints.at(curNdx)->projY;

            qreal nextX = vecPoints.at(nxtNdx)->projX;
            qreal nextY = vecPoints.at(nxtNdx)->projY;


            pdxffile->Line(curX, curY, nextX, nextY);
        }
    }

}
