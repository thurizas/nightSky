#pragma once
#include "evDist.h"

#include <iostream>
#include <ostream>
#include <fstream>

class CProjection;
class CDxfFileWrite;
class QGraphicsScene;
class QDataStream;
class QFile;



class CStar
{
public:
    CStar(void);
    CStar(float, float, float, bool isSph = false);
    CStar(float, float, float, int, float, bool isSph = false);
    //CStar(float, float, float, int, bool isSph = false);
    ~CStar(void);

    float    getX() {return carCoords[0];}
    float    getY() {return carCoords[1];}
    float    getZ() {return carCoords[2];}

    const float    getR() {return sphCoords[0];}
    const float    getT() {return sphCoords[1];}
    const float    getP() {return sphCoords[2];}

    const float      getLum() {return m_lum;}
    const float      getMass() {return m_mass;}
    const float      getRadius() {return m_radius;}
    const int        getType() {return m_type;}

    float    getLat();
    float    getLon();

    void    draw(CProjection* pproj, CDxfFileWrite* pdxffile);
    void    draw(CProjection* pproj, QGraphicsScene*);

    void    serialize(int, std::ofstream*);

private:
    void       Car2Sph();
    void       Sph2Car();
    void       calcProps();
    void       calcProps(int, float);
    //CGEVDist*  m_dist;
    float      carCoords[3];           // array of coordinates for the stars
    float      sphCoords[3];           // spherical polar coordinates for the stars
    int        m_type;                 // type of star     0 = O, 1 = B, 2 = A, 3 =  F, 4 =  G, 5 =  K, 6 =  M
    float      m_mass;                 // mass of the start
    float      m_radius;               // radius of star
    float      m_lum;                  // brightness of the star -- used to set size on the maps.
    float      m_temp;                 // temperature of the star -- used to set color on the maps.
};

std::ostream& operator<<(std::ostream&, const CStar&);