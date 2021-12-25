#ifndef _constants_h_
#define _constants_h_

#include <cstdint>

#define PI      3.14159265359
#define HALFPI  1.57079632679
#define RAD2DEG 180.0/PI
#define DEG2RAD PI/180.0
#define EPISILON 2E-5
#define CNTSLICES 13    // todo: change these to 12
#define CNTSEGMENTS 13

// parameters / data structures for serialization

#define ID_LEN 4           // length of the ID string (nsky)

struct hdr
{
	char hdr_id[8];         // 8 bytes - file identification string 4 bytes magic number, 1 byte version (major: high nybble, minor: low nybble), 1 byte byte order (0: little, 1: big), 2 bytes padding
	char hdr_display;       // 1 byte  - display mode this was saved under 0: mollweide, 1: orthographic
	char pad;               // 1 byte  - padding 
	uint16_t hdr_cbRecord;  // 2 bytes - size of a star record
	uint32_t hdr_cntStars;  // 4-bytes - number of records
	uint32_t hdr_offsetRcd; // 4-bytes - offset from beginning of file to first record
}; // 20 bytes total


struct star
{
	uint32_t star_ndx;             // 4-bytes
    float    star_radius;          // 4-bytes, radial distance
    float    star_theta;           // 4-bytes, theta component
    float    star_phi;             // 4-bytes, phi component
    float    star_mass;            // 4-bytes, mass of the star
    uint32_t star_type;            // 4-bytes, type of star
}; // 6 fields * 4-bytes/field = 24 bytes total

static const uint8_t VER_MAJOR = 1;
static const uint8_t VER_MINOR = 3;
static const int16_t testVal = 0x1234;

#define ENDIAN_L 0
#define ENDIAN_B 1

// parameters for use in export to dxf format
#define MSG_LEN 25
#define VERT_SPACING 30
#define TEXT_VOFFSET 3
#define TEXT_HOFFSET 20
#define TEXT_HEIGHT 10

#define LUM_BINS 8

static const double    scale[] = { 0.06, 0.12, 0.25, 0.37, 0.50, 0.75, 1.0, 1.5 };      // scale factors to set circle radius

#endif