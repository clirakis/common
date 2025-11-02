/********************************************************************
 *
 * Module Name : GGA.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic GGA
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
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring> 

// Local Includes.
#include "debug.h"
#include "GGA.hh"
#include "NMEA_helper.hh"

/**
 ******************************************************************
 *
 * Function Name :  GGA Message Constructor
 *
 * Description : 
 *
 * Inputs : NONE
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested on:
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GGA::GGA(void) : NMEA_Position()
{
    SET_DEBUG_STACK;
    fGeoidheight  = 0.0;
    fAltitude     = 0.0;
    fFixIndicator = 0;
    fHDOP         = 0.0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name :  GGA::Decode
 *
 * Description : given a GGA line of data decode it into the proper 
 *               fields
 *
 * Inputs : GGA data stream. 
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested on:
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool GGA::Decode(const char *line)
{
    SET_DEBUG_STACK;

    /*
     * Capture the PC time of the message.
     */
    clock_gettime( CLOCK_REALTIME, &fPCTime);

    char *p = (char *) line;

    // get time
    p = strchr(p, ',')+1;

    // Changing this up a bit
    // use fUTC as original format. 
    // 
    fUTC     = atof(p);
    fSeconds = DecodeUTCFixTime( p, &fMilliseconds, NULL);

    // parse out latitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fLatitude = DecodeDegMin(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	if (p[0] == 'S') fLatitude *= -1.0;
    }
    
    // parse out longitude
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fLongitude = DecodeDegMin(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	if (p[0] == 'W') fLongitude *= -1.0;
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fFixIndicator = atoi(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fSatellites = atoi(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fHDOP = atof(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fAltitude = atof(p);
    }
    
    p = strchr(p, ',')+1;
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fGeoidheight = atof(p);
    }
    return true;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GGA constructor
 *
 * Description :
 *
 * Inputs :
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
/**
 ******************************************************************
 *
 * Function Name : GGA destructor
 *
 * Description :
 *
 * Inputs :
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


/**
 ******************************************************************
 *
 * Function Name : GGA function
 *
 * Description :
 *
 * Inputs :
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
