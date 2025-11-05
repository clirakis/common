/********************************************************************
 *
 * Module Name : NMEA_Position.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic NMEA_Position
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
#include <sys/time.h>

// Local Includes.
#include "debug.h"
#include "NMEA_Position.hh"
#include "NMEA_helper.hh"
#include "tools.h"

/**
 ******************************************************************
 *
 * Function Name :  NMEA_POSITION Constructor
 *
 * Description : Construct a NMEA Position and zero out everything. 
 *
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
NMEA_Position::NMEA_Position(void)
{
    SET_DEBUG_STACK;
    fLatitude     = fLongitude = 0.0;
    fSeconds      = 0;
    fMilliseconds = 0.0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SetUTCNow
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
void NMEA_Position::SetUTCNow(void)
{
    SET_DEBUG_STACK;
    struct timeval  now;
    struct timezone tz;
    gettimeofday(&now, &tz); 
    // fSeconds is the number of seconds since the epoch in UTC
    fSeconds = now.tv_sec + tz.tz_minuteswest*60;
    fMilliseconds = now.tv_usec/100;
    /*
     * UTC is time into the current day. 
     * figure this out by subtracting from the epoch.
     *
     * first go to struct tm. 
     */
    struct tm det;
    localtime_r(&fSeconds, &det);
    // Zero out h,m,s
    det.tm_hour = 0;
    det.tm_min  = 0;
    det.tm_sec  = 0;
    time_t day = mktime(&det);   // midnight
    // fUTC is the time into the current day
    fUTC = fSeconds - day;
}


/**
 ******************************************************************
 *
 * Function Name : 
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
ostream& operator<<(ostream& output, const NMEA_Position &n)
{

    string UTC = EncodeUTCSeconds(n.fSeconds, n.fMilliseconds);
    output << "NMEA_Position::" << endl
	   << "         Latitude: " << n.fLatitude * RadToDeg << endl
	   << "        Longitude: " << n.fLongitude * RadToDeg << endl
	   << " Seconds into day: " << UTC << endl;
    return output;
}


/**
 ******************************************************************
 *
 * Function Name : 
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
