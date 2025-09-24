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
 * 10-Sep-25    CBL    Updated to version 9 of proj
 *
 * Classification : Unclassified
 *
 * References : http://proj4.org
 * https://proj.org/usage/projections.html
 * https://scienceweb.whoi.edu/marine/ndsf/utility/NDSFutility.html
 * https://proj.org/en/stable/development/quickstart.html
 * https://proj.org/en/stable/development/reference/functions.html
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
    fC   = NULL;
    fProjection = NULL;

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
    proj_destroy(fProjection);
    proj_context_destroy(fC);
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
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 * References:
 * https://proj.org/en/stable/development/reference/functions.html
 * https://proj.org/en/stable/faq.html
 * https://epsg.io/transform#s_srs=4326&t_srs=3857&x=-71.2741039&y=41.4846989
 * 
 *
 *******************************************************************
 */
void Geodetic::SetProjection(const double &LatDegree, const double &LonDegree,
    PROJECTION p)
{
    SET_DEBUG_STACK;

    CLogger *log = CLogger::GetThis();
    /* Create the transform from geodetic to projected coordinates.*/
    const char *src  = "EPSG:4326"; /* This is WGS84 LL */
    PJ_AREA    *A    = NULL;        /* May come back in the future. */
    char dest[32];

    // Calculate zone, don't really need this in the EPSG format. 
    fZone = ceil((LonDegree+180.0)/6.0);

    /*
     *  Store projection center in Degrees
     */
    fLat = LatDegree;
    fLon = LonDegree;

    switch (p)
    {
    case kMERCATOR:
	//dest = (const char *)"EPSG:3857"; /* This is psuedo-mercator */
	snprintf(dest, sizeof(dest), "EPSG:3857");
	break;

    case kTEST:
	//dest = (const char *)"EPSG:32632"; /* UTM ZONE 32N */
	snprintf(dest, sizeof(dest), "EPSG:32632");
	break;
    case kUTM:
	snprintf(dest, sizeof(dest), "EPSG:32618");
	break;
    default:
	/* I don't think this is applicable in a useable way
	 */
#if 0
	// OTHER inputs
	/* This is going to be NAD83 */
	//dest = (const char *)"ESRI:104602"; 
	dest = (const char *)"ESRI:4269"; 
#endif
	break;
    }
    log->Log("# User Selection, src: %s dest: %s\n", src, dest);

    /* Step 1 */
    fC = proj_context_create();

    /* Step 2 */
    PJ *P         = proj_create_crs_to_crs( fC, src, dest, A);
    if(!P)
    {
	log->LogError(__FILE__, __LINE__, 'F',"Geodetic:SetProjection, fail.");
	return;
    }

    /* 
     * This will ensure that the order of coordinates for the input CRS 
     * will be longitude, latitude, whereas EPSG:4326 mandates latitude, 
     * longitude 
     */
    PJ *norm = proj_normalize_for_visualization(fC, P);
    if (!norm) 
    {
        log->Log("# Failed to normalize transformation object.\n");
        return;
    }
    proj_destroy(P);
    fProjection = norm;
    log->Log("# Projection initialized, src: %s dest: %s\n", src, dest);

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
 * Inputs : ARE IN DEGREES!! 
 *
 * Returns : Point class with result. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Point Geodetic::ToXY(const double &Lon, const double &Lat, const double &Z) const
{
    SET_DEBUG_STACK;
    PJ_COORD p, res;
    p.lp.lam = Lon;
    p.lp.phi = Lat;
    p.lpzt.z = Z;
    p.lpzt.t = HUGE_VAL;

    res = proj_trans(fProjection, PJ_FWD, p);
    SET_DEBUG_STACK;
    return Point(res.enu.e, res.enu.n, res.lpzt.z);
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
 * Inputs : ARE IN DEGREES!
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
    PJ_COORD p, res;
    p.lp.lam = Lon_X;
    p.lp.phi = Lat_Y;
    p.lpzt.z = 0.0;
    p.lpzt.t = HUGE_VAL;

    res = proj_trans(fProjection, PJ_FWD, p);
    SET_DEBUG_STACK;
    Lon_X = res.enu.e;
    Lat_Y = res.enu.n;
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
    PJ_COORD p,res;
    p.lpzt.z = Z;
    p.lpzt.t = HUGE_VAL;
    p.enu.e  = X;
    p.enu.n  = Y;
    res = proj_trans(fProjection, PJ_INV, p);

    SET_DEBUG_STACK;
    return Point( res.lp.lam, res.lp.phi, res.lpzt.z);
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
    PJ_COORD p,res;
    p.lpzt.z = 0.0;
    p.lpzt.t = HUGE_VAL;
    p.enu.e  = Lon_X;
    p.enu.n  = Lat_Y;
    res = proj_trans(fProjection, PJ_INV, p);
    Lon_X = res.lp.lam;
    Lat_Y = res.lp.phi;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : TestCase
 *
 * Description : Running regression test cases
 * website: https://proj.org/en/stable/development/quickstart.html
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Geodetic::TestCase(void)
{
    SET_DEBUG_STACK;
    PJ_CONTEXT *C;
    PJ *P;
    PJ *norm;
    PJ_COORD a, b;

    /* or you may set C=PJ_DEFAULT_CTX if you are sure you will     */
    /* use PJ objects from only one thread                          */
    C = proj_context_create();

    P = proj_create_crs_to_crs(
        C, "EPSG:4326", "+proj=utm +zone=32 +datum=WGS84", /* or EPSG:32632 */
        NULL);

    if (0 == P) {
        fprintf(stderr, "Failed to create transformation object.\n");
        return;
    }

    /* This will ensure that the order of coordinates for the input CRS */
    /* will be longitude, latitude, whereas EPSG:4326 mandates latitude, */
    /* longitude */
    norm = proj_normalize_for_visualization(C, P);
    if (0 == norm) {
        fprintf(stderr, "Failed to normalize transformation object.\n");
        return;
    }
    proj_destroy(P);
    P = norm;

    /* a coordinate union representing Copenhagen: 55d N, 12d E */
    /* Given that we have used proj_normalize_for_visualization(), the order */
    /* of coordinates is longitude, latitude, and values are expressed in */
    /* degrees. */
    a = proj_coord(12, 55, 0, 0);

    /* transform to UTM zone 32, then back to geographical */
    b = proj_trans(P, PJ_FWD, a);
    printf("easting: %.3f, northing: %.3f\n", b.enu.e, b.enu.n);

    b = proj_trans(P, PJ_INV, b);
    printf("longitude: %g, latitude: %g\n", b.lp.lam, b.lp.phi);

    /* Clean up */
    proj_destroy(P);
    proj_context_destroy(C); /* may be omitted in the single threaded case */
}
