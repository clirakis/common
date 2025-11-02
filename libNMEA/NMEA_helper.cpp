/********************************************************************
 *
 * Module Name : NMEA_helper.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic NMEA_helper
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
#include <cstdint> 
#include <cstring>

// Local Includes.
#include "debug.h"
#include "NMEA_helper.hh"
#include "Constants.h"

/**
 ******************************************************************
 *
 * Function Name :  DecodeUTCFixTime
 *
 * Description : decode time based on UTC HHMMSS.mm format and
 *               return a time_t structure based on GPS fix time
 *
 * Inputs : 
 *    p - character string input of fix time in format of HHMMSS.ss
 *    ms - return variable for milliseconds
 *    now - structure containing information for tm values (incomplete)
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
time_t DecodeUTCFixTime(const char *p, float *ms, struct tm *now)
{
    SET_DEBUG_STACK;
    /*
     * We don't quite have MMDDYY so we can't convert this to epoch. 
     * lets assume the system clock is ok. 
     */
    time_t current;
    time(&current);
    struct tm *tmnow = gmtime(&current);

    /*
     * Input format should be of the string format HHMMSS.mm
     * Override items in tmnow
     */
    float    timef   = atof(p);   // UTC time reference to current day
    uint32_t time    = timef;
    uint8_t  hour    = time / 10000;
    uint8_t  minute  = (time % 10000) / 100;
    uint8_t  seconds = (time % 100);

    if (ms)
    {
	*ms =  fmod(timef, 1.0) * 1000;
    }

    tmnow->tm_sec  = seconds;
    tmnow->tm_min  = minute;
    tmnow->tm_hour = hour;
    
    if (now)
    {
	memcpy(now, tmnow, sizeof(struct tm));
    }
    SET_DEBUG_STACK;
    return mktime(tmnow);
}
/**
 ******************************************************************
 *
 * Function Name :  DecodeDate
 *
 * Description : Decode Time field that contains Day Month Year
 *
 * Inputs : 
 *     p   - charcter string containing date in the format DDMMYY
 *     now - struct tm from calling function to be filled. Assume this
 *           is partially filled from previous RMC field. 
 *
 * Returns : time_t in seconds from epoch
 *
 * Error Conditions : none
 *
 * Unit Tested on: NOT SURE THIS IS WORKING!!
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
time_t DecodeDate(const char *p, struct tm *now)
{
    /*
     * 
     * Full epoch. assume previous function was used first. 
     * also assum input for p is ddmmyy
     * tm_sec 0-59
     * tm_min 0-59
     * tm_hour 0-23
     * tm_mday 1-31
     * tm_mon 0-11
     * tm_year number of years since 1900
     */
    SET_DEBUG_STACK;

    uint32_t fulldate = atof(p);
    now->tm_mday = fulldate / 10000;
    now->tm_mon  = (fulldate % 10000) / 100 - 1;
    now->tm_year = (fulldate % 100)+100;
    SET_DEBUG_STACK;
    return mktime(now);
}

double DecodeDegMin(const char *p)
{
    SET_DEBUG_STACK;
    double  Degrees, Minutes, number;

    /* 
     * Format we are trying to parse is dddmm.mmmm 
     * I am going to do this vastly differently. 
     * I realize that on an embedded processor, floating point is 
     * a premium, not here though. 
     */
    number = atof(p);
    // Strip off fractional part immediately. 
    Minutes = modf( number, &Degrees);
    /*
     * The remainder of the minutes is embedded in the low two 
     * decimal places of the number variable at this point. 
     */
    Minutes = fmod(number, 100.0);
    // remove the integer part of the minutes. 
    Degrees = floor( Degrees/100.0);

    // Add in minutes portion
    Degrees += Minutes/60.0;

    SET_DEBUG_STACK;
    return (Degrees * DegToRad);   // Return radians
}
