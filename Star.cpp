#include <cstdlib>
#include <math.h>

#include "Star.h"
#include "constants.h"
#include "CadLib.h"                       // for dxf export support
#include "Projection.h"
#include "evDist.h"

#include <map> 
#include <QGraphicsScene>
#include <QDataStream>
#include <QFile>
#include <QDebug>

#include <iostream>


CStar::CStar(void) : carCoords{ 0, 0, 0 }, sphCoords{ 0, 0, 0 }, m_type(1), m_mass(0), m_radius(0), m_lum(1), m_temp(0)
{

}


CStar::CStar(float x, float y, float z/*, CGEVDist* pDist */, bool isSph) : m_type(1), m_mass(0), m_radius(0), m_lum(1), m_temp(0)   
{
    if(isSph)                 // construction an object based on spherical coordinates
    {
        sphCoords[0] = x;
        sphCoords[1] = y;
        sphCoords[2] = z;

        Sph2Car();
    }
    else                      // construction of an object based on cartesian coordinates
    {
        carCoords[0] = x;
        carCoords[1] = y;
        carCoords[2] = z;

        Car2Sph();
    }
    
    calcProps();               // calculate other properties for this star
}



CStar::CStar(float x, float y, float z, int t, float m, bool isSph) : m_type(t), m_mass(m), m_radius(0), m_lum(1), m_temp(0)
{
    if (isSph)                 // construction an object based on spherical coordinates
    {
        sphCoords[0] = x;
        sphCoords[1] = y;
        sphCoords[2] = z;

        Sph2Car();
    }
    else                      // construction of an object based on cartesian coordinates
    {
        carCoords[0] = x;
        carCoords[1] = y;
        carCoords[2] = z;

        Car2Sph();
    }

    calcProps(t, m);               // calculate other properties for this star
}



CStar::~CStar(void)
{
}



void CStar::Car2Sph()
{
    float radius = sqrt(carCoords[0]*carCoords[0] + carCoords[1]*carCoords[1] + carCoords[2]*carCoords[2]);
    
    sphCoords[0] = radius;
    sphCoords[1] = acos(carCoords[2]/radius);                           // theta, in radians
    sphCoords[2] = atan2(carCoords[1], carCoords[0]);                   // phi, in radians
}



void CStar::Sph2Car()
{

}


/*
 * normal spherical coordinates measure phi from the positive x axis counter clockwise giving a range of [0, 360] degrees.
 * in geocentric coordinates, we measure either clockwise or conter-clockwise from the positive x axis giving a range of 
 * [+180, -180], with the negative x-axis being at 180.
 *     axis    phi      geocentric
 *     pos-x   0           0
 *     pos-y   90         90
 *     neg-x   180        180 (or -180)
 *     neg-y   270        -90
 */
float CStar::getLon()
{
    float phi = sphCoords[2];

    return phi < 180 ? phi : phi - 2*PI; 
}


/*
 * normal spherical coordinates measure theta from the positive z-axis down, giving a range of [0, 180], with any
 * point on the positive z-axis having a value of 0.  In geocentric coordinates, we measure either up or down from
 * the equatorial plane giving a range of [+90, -90] degrees.  We make this conversion here, returning the value 
 * in radians.
 */
float CStar::getLat()
{
    return ((PI/2) - sphCoords[1]);

}



/*
 *   Temperature, T = (L/R^2)^{0.25}
 * step 1 -- determine star classification. table of types (main sequence stars from https://www.youtube.com/watch?v=hG1of0MroM8
 * 
 *   class | frequence  |  mass        |range         |  color         | notes
 *     O   |  0.00003   |  \ge 16      |    1 -    1  | 155, 176, 255  | slight underestimate of type O stars
 *     B   |  0.13000   |  2.1 - 16    |    2 -   15  | 170, 191, 255  |
 *     A   |  0.60000   |  1.40 - 2.10 |  16 -   76   | 202, 215, 255  |
 *     F   |  3.00000   |  1.04 - 1.40 |  300 -  377  | 248, 247, 255  |
 *     G   |  7.60000   |  0.80 - 1.04 |  760 - 1138  | 255, 244, 234  |
 *     K   | 12.10000   |  0.45 - 0.80 | 1210 - 2349  | 255, 210, 161  |
 *     M   | 76.45000   |  0.08 - 0.45 | 7645 - 9995  | 255, 204, 111  |
 * 
 * step 2 -- determine the star mass from above table, use a linear approximation to select from range
 *           mass is in range [0.45, 17]
 *
 * step 3 -- generate radius, R = M^{0.5} is M \ge M_{sol} else R = M^{0.8} if M < M_{sol}
 *           radius is in range [0.528, 4.123]  
 *
 * step 4 - generate luminosity, L = M^{3.5}
 *          luminosity is in range [0.0611, 20.256]
 * 
 * step 5 - calculate the effective temperature T = (L/R^2)^{0.25}
 * 
 * in addition to these main sequence stars there will be 96 white dwarfs and 4 giant per 1000 stars
 */
const int types[] = {1,15,76,377,1138,2349,9995};
const float mass[] = {17, 16, 16, 2.1, 2.1, 1.4, 1.4, 1.04, 1.04, 0.80, 0.80, 0.45, 0.45, 0.08};   // (max, min) for each stellar type... O->0,1 B->2,3 A->4,5 F->6,7 G->8,9 K->10,11 M->12,13
void CStar::calcProps()
{
    int ndx = 0;
    int roll = rand()%9995 + 1;                               // generate number in [1,9995]  // TODO : use better RNG method

    for(ndx = 0; ndx <= 6; ndx++)                             // generate type of star ... O,B,A,F,G,K,M
    {
        if(roll <= types[ndx])
            break;
    }
    m_type = ndx;                                             // set stellar type O:0, B:1, A:2, F:3, G:4, K:5, M:6

    int roll1 = rand()%100 + 1;                               // generate number in [0,100]  TODO : use normal dist between min & max?
    int     nMax = 2*m_type;                                  // convert stellar index (O:0,B:1,...M:6) into index's into mass array
    int     nMin = 2*m_type+1;                                // mass index, has maximum mass first and minimum mass second
    float maxMass = mass[nMax];                     
    float minMass = mass[nMin];
    m_mass = (maxMass - minMass)*roll1/100 + minMass;         // generate mass of star - linear interpolation between min and max
    
    if(m_mass > 1)                                            // generate radius of star
        m_radius = sqrt(m_mass);
    else
        m_radius = m_radius = pow(m_mass, (float)0.8);

    m_lum = pow(m_mass, (float)3.5);                          // generate luminosity of star
    
    m_temp = pow((m_lum / m_radius*m_radius), (float)0.25);   // generate temperature / color of star
}


// 
void CStar::calcProps(int type, float mass)
{
    m_type = type;         // set stellar type O:0, B:1, A:2, F:3, G:4, K:5, M:6
    m_mass = mass;         // set mass of star

    if (m_mass > 1)                                            // generate radius of star
        m_radius = sqrt(m_mass);
    else
        m_radius = m_radius = pow(m_mass, (float)0.8);

    m_lum = pow(m_mass, (float)3.5);                          // generate luminosity of star

    m_temp = pow((m_lum / m_radius * m_radius), (float)0.25);   // generate temperature / color of star
}



void CStar::draw(CProjection* pproj, CDxfFileWrite* pdxffile)
{
    std::pair<int, int>  loc = pproj->convert(getLat(),getLon());       // x,y coordinates of center point.

    double radius = 10.0;

    if (m_lum <= 0.001)
        radius = radius * scale[0];
    else if (m_lum <= 0.01)
        radius = radius * scale[1];
    else if (m_lum <= 0.1)
        radius = radius * scale[2];
    else if (m_lum <= 1.00)
        radius = radius * scale[3];
    else if (m_lum <= 10.00)
        radius = radius * scale[4];
    else if (m_lum <= 100)
        radius = radius * scale[5];
    else if (m_lum <= 1000)
        radius = radius * scale[6];
    else
        radius = radius * scale[7];

    pdxffile->Circle(loc.first, loc.second, radius);

}



void CStar::draw(CProjection* pproj, QGraphicsScene* pscene)
{
    QPen    pen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QBrush  brush(Qt::white, Qt::SolidPattern);
    QColor* pColor;

    float  lat = getLat();
    float  lon = getLon();

    std::pair<int, int>  loc = pproj->convert(lat,lon);                // x,y coordinates of center point.

    // set brush color based on type...
    switch (m_type)
    {
    case 0:  // O
        pColor = new QColor(155, 176, 255);
        break;
    case 1:  // B
        pColor = new QColor(170, 191, 255);
        break;
    case 2: // A
        pColor = new QColor(202, 215, 255);
        break;
    case 3: // F
        pColor = new QColor(248, 247, 255);
        break;
    case 4: // G
        pColor = new QColor(255, 244, 234);
        break;
    case 5: // K
        pColor = new QColor(255, 210, 161);
        break;
    case 6: // M
        pColor = new QColor(255, 204, 111);
        break;
    default:
        pColor = new QColor(Qt::white);
    }

    brush.setColor(*pColor);
    pen.setColor(*pColor);

    float fScale;
    if (m_lum < 0.01) fScale = 0.25;
    else if (m_lum < 0.10) fScale = 0.50;
    else if (m_lum < 1.0) fScale = 0.75;
    else if (m_lum < 5.0) fScale = 1.0;
    else if (m_lum < 10.0) fScale = 1.5;
    else fScale = 2.0;


    QPointF tl(-0.5*fScale + loc.first, 0.5 * fScale + loc.second);                                // top left of unit square
    QPointF br(0.5 * fScale + loc.first, -0.5 * fScale + loc.second);                              // bottom right of unit square



    QRectF   rect(tl, br);
    pscene->addEllipse(rect,pen, brush);
    
    delete pColor;
}



/**********************************************************************************************************************
* function  : serialize
*
* abstract  : writes star data out to a file, the data is written out in binay form.  In addition to the values calcuated
*             in this class, the index of the star is also printed out.
*
* parameters: pofile -- [in] ndx, index of the star in the collection 
*                       [in/out] pofile, pointer to the file stream to use for archiving.
*
* retuns    : void
*
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
void CStar::serialize(int ndx, std::ofstream* pofile)
{
    struct star temp;

    temp.star_ndx = ndx;
    temp.star_radius = sphCoords[0];
    temp.star_theta = sphCoords[1];
    temp.star_phi = sphCoords[2];
    temp.star_mass = m_mass;
    temp.star_type = m_type;

    pofile->write((char*)&temp, sizeof(struct star));
}



/**********************************************************************************************************************
* function  : operator<<
*
* abstract  : this is the overloaded stream insertion operator, used for seralizing an object of type CStar to an ouptut
*             stream.  This prints out necessary information in a comma separated list, we have the following fields
*                 radius    - 8 digits max (8.5)                    9
*                 latitude  - 8 digits max (8.5)                    9
*                 longitude - 8 digits max (8.5)                    9
*                 type      - 1 digit  max                          9
*                 mass      - 6 digits max  (6.4)                   7   (48 + 6 + 1 -- numbers/commas/end of line = 55)
*             this is useful for debugging and checking values.
*
* parameters: os --[in] constant reference to the output stream that we are using
* 
* retuns    : reference to the output stream
* 
* written   : GKHuber Dec 2021
**********************************************************************************************************************/
std::ostream& operator<<(std::ostream& os, const CStar& star)
{
    char msg[55];

    // need to do some type casting to shut-up errors.
    float r = const_cast<CStar&>(star).getR();
    float t = const_cast<CStar&>(star).getT();
    float p = const_cast<CStar&>(star).getP();
    int ty = const_cast<CStar&>(star).getType();
    float m = const_cast<CStar&>(star).getMass();

    memset((void*)msg, '\0', 55);                  
    sprintf(msg, "%8.5f,%8.5f,%8.5f,%1d,%6.4f\n", r, t, p, ty, m);

    os << msg;

    return os;
}



