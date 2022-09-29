/********************************************************************
 *\file
 * Module Name : tools.h
 *
 * Author/Date : C.B. Lirakis / 23-Oct-98
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/

#ifndef __TOOLS_h_
#define __TOOLS_h_
#include <limits.h>
#include <float.h>
#include <math.h>
#include "Constants.h"


    /** Make sure the degree angles are bounded from the upper side */
    inline double BoundDegreeAnglePlus (const double x) {return (x > 360.0 ? (x - 360.0): x);};
    /** Make sure the degree angles are bounded from the lower side */
    inline double BoundDegreeAngleMinus(const double x) {return (x < 360.0 ? (x + 360.0): x);};
    /** Convert from Yaw to Heading */
    inline double AngleToHeading(const double rad) 
    {
	double rc=M_PI_2-rad; 
	while(rc < 0) rc += (2.0*M_PI);
	while (rc > (2.0*M_PI)) rc -= (2.0*M_PI); 
	return rc;
    };
    /**
     * Enter Latitude and Longitude in degrees, get back an 
     * estimate of the distance in meters.
     * Y : Latitude, only depends on latitude.
     */
    inline double GY(double lat) {return lat*MetersPerMinute * 60.0;};
    /**
     * Enter Latitude and Longitude in degrees, get back an 
     * estimate of the distance in meters.
     * X : Longitude, depends on latitude and longitude.
     */
    inline double GX( double lat, double lon) {return lon*MetersPerMinute * 60.0 * cos(lat*M_PI/180.0);};

    // Bound radian angles.
    inline double phi_mpi_pi(double x) {while (x >= M_PI){x-=(2.0*M_PI);}
    while (x<-M_PI) {x+=(2.0*M_PI);} return x;};

    // non-inline methods.
    /** Convert radians to degrees and minutes */
    void rtodm( double radians, int *degrees, double *minutes);

    /** Give us a string latitude. User provides an input string. */
    char *str_lat(double lat, char *instr);

    /** Give us a string longitude */
    char *str_lon(double lon, char *instr);

    /*! Convert compass heading to conventional theta. */
    double HeadingToAngle(double Heading);

inline double sign(double x) {return ((x>0.0)?1.0:((x<0.0)?-1.0:0.0));};
    /** Print a traceback when called. */
    void print_trace(void); 
namespace tools {
    /** Test a particular bit in a byte wide bit packed value */
    inline bool TestBit( unsigned char val, const unsigned char bit) 
    {return ((val&(1<<bit))>0);};
    /** Set a bit in a byte wide bit packed value */
    inline void SetBit(unsigned char *val,  const unsigned char bit)
    {*val |= (1<<bit);};
    /** Clear a bit in a byte wide bit packed value */
    inline void ClearBit(unsigned char *val,  const unsigned char bit)
    {*val &= ~(1<<bit);};
}

#endif
