#pragma once

#include "Grid.h"
#include <map>

class QGraphicsScene;
class CDxfFileWrite;

class CProjection
{
public:
    CProjection(void);
    virtual ~CProjection(void);

    virtual std::pair<int, int> convert(float, float) = 0;
    virtual void drawGrid(CGrid*, QGraphicsScene*) = 0;
    virtual void drawGrid(CGrid*, CDxfFileWrite*) = 0;

};

