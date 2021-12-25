#pragma once
#include "projection.h"

/*********************************************************************************************************************************************************************************
 
 * This produces an orhtographic projection of the star field, as if looking from the center of the planet toward infinity.  There are two cicles, one for the northern hemisphere
 * and the second of the southern hemisphere
 * 
 * conversion of latitude (l) and longitude (L) into polar coordinate (r,\theta) is given by:
 * 
 *    r      = 2T\tan(\pi/4 - l/2)
 *    \theta = L
 *  where R is the radius 
*/
class COrthographic : public CProjection
{
public:
    COrthographic(float);
    ~COrthographic(void);

    std::pair<int, int> convert(float, float);
    void drawGrid(CGrid*, QGraphicsScene*);
    void drawGrid(CGrid*, CDxfFileWrite* pdxffile);

private:
    float                  m_fRadius;
    std::vector<ppointT>   m_vecEquator;

    void COrthographic::calcEquator(int segs);
};
