#pragma once
#include "projection.h"


class CMollweide : public CProjection
{
public:
    CMollweide(float);
    ~CMollweide(void);

    std::pair<int, int> convert(float, float);
    void drawGrid(CGrid*, QGraphicsScene*);
    void drawGrid(CGrid*, CDxfFileWrite* pdxffile);

private:
    float      m_fRadius;
    float      m_fCoeff;
    float      solveAux(float, float);
};

