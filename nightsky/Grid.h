#pragma once

#include "constants.h"

#include <vector>

class QGraphicsScene;
class CProjection;
class CDxfFileWrite;

/*
 * This class maintains a list of points in (Latitude, Longitude) in increments of 10 degrees, 30 degrees respectively.  The points will be used
 * to draw the grid on the output map based on the specific projection being used.
 */

typedef struct _point
{
    float       lat;
    float       lon;
    float       projX;
    float       projY;
} pointT, *ppointT;


class CGrid
{
public:
    //CGrid(void);
    CGrid(int cntSlices = CNTSLICES, int cntSegments = CNTSEGMENTS);
    ~CGrid(void);

    const int slices() { return m_cntSlices; }
    const int segments() { return m_cntSegments;  }

    void convert(CProjection* proj);              
    //void draw(QGraphicsScene*);
    void draw(CDxfFileWrite*);    // TODO : more this to the projection class

    std::vector<ppointT> getGrid() { return m_vecPoints; }

    void printGridPoints(bool);

private:
    int                                  m_cntSlices;
    int                                  m_cntSegments;
    std::vector<ppointT>                 m_vecPoints;
    std::vector<std::pair<int, int> >    m_vecProjPoints;
};

