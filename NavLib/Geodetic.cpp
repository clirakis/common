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
 * 12-Mar-22   CBL Updated. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <cstdio>

// Root includes
#include "TMath.h"

// Local Includes.
#include "debug.h"
#include "Geodetic.hh"
#include "CLogger.hh"

// Root includes
#include <TEnv.h>

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
Geodetic::Geodetic(double LatDegree, double LonDegree, ePROJECTION p) 
{
    fSelectedProjection = p;
    SetProjection(LatDegree, LonDegree);
    fGeo = this;
}
void Geodetic::SetProjection(double LatDegree, double LonDegree)
{
    SET_DEBUG_STACK;
    CLogger *log = CLogger::GetThis();
    char     DefString[128];

    // Calculate zone
    fZone = ceil((LonDegree+180.0)/6.0);

    // Store projection center in RADIANS
    fLat = LatDegree * TMath::DegToRad();
    fLon = LonDegree * TMath::DegToRad();

    switch (fSelectedProjection)
    {
    case kMERCATOR:
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

    case kUTM:
    default:
	//sprintf(DefString,"+proj=utm +lon_0=%f +lat_0=%f +ellps=WGS84 +k0=0.9996", LonDegree, LatDegree);
	sprintf(DefString,"+proj=utm +zone=%d +ellps=WGS84", fZone);
	break;
    }

    fCurrentProjection = pj_init_plus(DefString);
    if(!fCurrentProjection)
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
    fXY = ToXY( fLon, fLat);

    log->Log("#    projection created, Latitude: %f Longitude %f\n", 
	     LatDegree,LonDegree);
    log->Log("#    center X: %f Y: %f\n", fXY.X(), fXY.Y());
    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : Constructor for geographic class.
 *
 * Description : Load using the root enviroment file. 
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
Geodetic::Geodetic(TEnv *p)
{
    SET_DEBUG_STACK;
    LoadPrefs(p);
    SetProjection(fLat, fLon);
    fGeo = this;
}

/**
 ******************************************************************
 *
 * Function Name : ToXY
 *
 * Description :
 *
 * Inputs : ARE IN RADIANS!
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
TVector2 Geodetic::ToXY(double Lat, double Lon) const
{
    SET_DEBUG_STACK;
    projUV in,out;
    in.u = Lon;
    in.v = Lat;
    out  = pj_fwd(in, fCurrentProjection);
    SET_DEBUG_STACK;
    return TVector2 (out.u,out.v);
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs : ARE IN RADIANS!
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
TVector2 Geodetic::ToLL(double X, double Y) const
{
    SET_DEBUG_STACK;
    projUV in,out;
    in.u = X;
    in.v = Y;
    out= pj_inv(in, fCurrentProjection);
    // Output is lon, lat but people usually think in Lat Lon.
    SET_DEBUG_STACK;
    return TVector2(out.u,out.v);
}
void Geodetic::LoadPrefs(TEnv* p)
{
    SET_DEBUG_STACK;
    fLon  = p->GetValue("Geodetic.center.Longitude", -73.8930);
    fLat  = p->GetValue("Geodetic.center.Latitude" , 41.3083);
    fSelectedProjection = (ePROJECTION) p->GetValue("Geodetic.ProjectionType", (Int_t) kUTM);
    CLogger::GetThis()->Log("# Geodetic: Load prefs\n");
    SET_DEBUG_STACK;
}
void Geodetic::SavePrefs(TEnv* p)
{
    SET_DEBUG_STACK;
    p->SetValue("Geodetic.center.Longitude", fLon*TMath::RadToDeg());
    p->SetValue("Geodetic.center.Latitude" , fLat*TMath::RadToDeg());
    p->SetValue("Geodetic.ProjectionType"  , fSelectedProjection);
    CLogger::GetThis()->Log("# Geodetic: Save prefs\n");
    SET_DEBUG_STACK;
}
