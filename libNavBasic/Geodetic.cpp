/********************************************************************
 *
 * Module Name : Geodetic.cpp
 *
 * Author/Date : C.B. Lirakis / 09-Feb-06
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * pj_fwd and pj_inv are obsolete. 
 * 28-Feb-22    CBL    Change from mercator to UTM
 *
 * Classification : Unclassified
 *
 * References : http://proj4.org
 * https://proj.org/usage/projections.html
 * https://scienceweb.whoi.edu/marine/ndsf/utility/NDSFutility.html
 * 
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <cstdio>

// Local Includes.
#include "debug.h"
#include "Geodetic.hh"
#include "CLogger.hh"
#include "tools.h"

Geodetic* Geodetic::fGeo;

/********************************************************************
 *
 * Function Name : Constructor for geographic class.
 *
 * Description : Base constructor, does no assignment and sets center
 *               at default value. WGS84, 41.5 71.2 mercator.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
Geodetic::Geodetic(const double &LatDegree, const double &LonDegree, PROJECTION p ) 
{
    SET_DEBUG_STACK;
    fGeo = this;
    SetProjection(LatDegree, LonDegree, p);
}
/**
 ******************************************************************
 *
 * Function Name : Destructor
 *
 * Description : just free up any allocated projections
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 22-Feb-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Geodetic::~Geodetic(void)
{ 
    pj_free(fProjection);
}

/**
 ******************************************************************
 *
 * Function Name : SetProjection
 *
 * Description : Initialize projection library
 *
 * Inputs : 
 *     Initial latitude and longitude in degrees
 *     Desired projection - currently only supports UTM or Mercator
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 22-Feb-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Geodetic::SetProjection(const double &LatDegree, const double &LonDegree,
    PROJECTION p)
{
    SET_DEBUG_STACK;
    CLogger *log = CLogger::GetThis();
    char     DefString[128];

    // Calculate zone
    fZone = ceil((LonDegree+180.0)/6.0);

    // Store projection center in RADIANS
    fLat = LatDegree * DegToRad;
    fLon = LonDegree * DegToRad;

    switch (p)
    {
    case MERCATOR:
	/*
	 * mercator projection
	 * latitude of true scale
	 * +proj=merc +lat_ts=LatDegree
	 *
	 * Northern Hemisphere
	 * +proj=utm +lon_0=LonDegree +lat_0=LatDegree +ellps=WGS84 +k0=0.9996
	 *
	 * test strings. 
	 * echo -73.893 41.3084 | proj +proj=merc +lat_ts=41.5
	 * -6169782.39     3772589.63
	 *
	 * const double Lat_0 = 41.3084;
	 * const double Lon_0 = -73.893;
	 */

	sprintf( DefString, "+proj=merc +ellps=WGS84 +lat_ts=%f", LatDegree);
	break;

    case UTM:
    default:
	//sprintf(DefString,"+proj=utm +lon_0=%f +lat_0=%f +ellps=WGS84 +k0=0.9996", LonDegree, LatDegree);
	sprintf(DefString,"+proj=utm +zone=%d +ellps=WGS84", fZone);
	break;
    }
    fProjection = pj_init_plus(DefString);
    if(!fProjection)
    {
	log->LogError(__FILE__, __LINE__, 'F',"Geodetic:SetProjection, fail.");
	return;
    }
    else
    {
	log->Log("# Projection initialized: %s\n", DefString);
    }

    /* 
     * If we get this far, we are good to go. 
     * Store the X,Y associated with the input Lat/Lon as the 
     * projection center. 
     */
    fXY = ToXY( fLon, fLat, 0.0);

    log->Log("#    projection created, Latitude: %f Longitude %f\n", 
	     LatDegree,LonDegree);
    log->Log("#    center X: %f Y: %f\n", fXY.X(), fXY.Y());
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : ToXY
 *
 * Description : Conversion from Lat Lon to YX. 
 *
 * Inputs : ARE IN RADIANS!
 *
 * Returns : Point class with result. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 05-Mar-22, test1
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Point Geodetic::ToXY(const double &Lon, const double &Lat, const double &Z) const
{
    SET_DEBUG_STACK;
    projUV p, res;
    p.u = Lon;
    p.v = Lat;
    res = pj_fwd( p, fProjection);
    SET_DEBUG_STACK;
    return Point(res.u, res.v, Z);
}

/**
 ******************************************************************
 *
 * Function Name : ToXY
 *
 * Description : 
 *    Conversion from Lat Lon to YX using point class. 
 *    point is organized as (Lon, Lat, Z)
 *
 * Inputs : Point in radiains
 *
 * Returns : Point class with result. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 05-Mar-22, Test 5
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Point Geodetic::ToXY(const Point &p) const
{
    SET_DEBUG_STACK;
    double LonX = p.X();
    double LatY = p.Y();
    ToXY( LonX, LatY);
    SET_DEBUG_STACK;
    return Point(LonX, LatY, p.Z());
}

/**
 ******************************************************************
 *
 * Function Name : ToXY
 *
 * Description : Conversion from Lat Lon to YX in place
 *
 * Inputs : ARE IN RADIANS!
 *
 * Returns : XY in meters in replacement
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 05-Mar-22, test3
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Geodetic::ToXY(double &Lon_X, double &Lat_Y) const
{
    SET_DEBUG_STACK;
    projUV p, res;
    p.u = Lon_X;
    p.v = Lat_Y;
    res = pj_fwd( p, fProjection);
    Lon_X = res.u;
    Lat_Y = res.v;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : ToLL
 *
 * Description : Input an XY coordinate and transform it to
 * a latitude and Longitude in radians. 
 *
 * Inputs : X,Y, and Z in meters. Z is not used, but provided.
 *
 * Returns : Point class
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 05-Mar-22, test2
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Point Geodetic::ToLL(const double &X, const double &Y, const double &Z) const
{
    SET_DEBUG_STACK;
    projUV p, res;
    p.u = X;
    p.v = Y;

    res = pj_inv(p, fProjection);
    SET_DEBUG_STACK;
    return Point( res.u, res.v, Z);
}

/**
 ******************************************************************
 *
 * Function Name : ToLL
 *
 * Description : Conversion from XY to Lat Lon 
 *
 * Inputs : X,Y in meters
 *
 * Returns : Lat/Lon in radians in replacement
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 05-Mar-22, test 4
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Geodetic::ToLL(double &Lon_X, double &Lat_Y) const
{
    SET_DEBUG_STACK;
    projUV p, res;
    p.u = Lon_X;
    p.v = Lat_Y;
    res = pj_inv( p, fProjection);
    Lon_X = res.u;
    Lat_Y = res.v;
    SET_DEBUG_STACK;
}
