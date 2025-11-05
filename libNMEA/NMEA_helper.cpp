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
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
 * 
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
#include "tools.h"
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
 * Unit Tested on: 03-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
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
 * Unit Tested on: 03-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string EncodeUTCTime(const time_t &now, uint32_t ms)
{
    SET_DEBUG_STACK;
    char txt[128],str[256];
    struct tm *val = localtime(&now);
    strftime( txt, sizeof(txt), "%H%M%S", val);
    snprintf( str, sizeof(str), "%s.%2.2d", txt, ms/10);
    string rv(str);
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :  Latitude in radians with sign. 
 *
 * Returns : string formatted in NMEA style including hemisphere. 
 *           using 8 decimal places, down to ~1mm. 
 *
 * Error Conditions :
 * 
 * Unit Tested on: 03-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string EncodeLatitude(const double &L)
{
    SET_DEBUG_STACK;
    char txt[256];
    char NS = 'N';

    double Latitude = fabs(L * RadToDeg); // take the sign away
    if (L<0.0)
	NS = 'S';

    /*
     * Get the three fields to make it look like:
     * 4129.0793,N
     * DDMM.xxxx
     * small change use upto 8 decimal places. 
     * 
     */
    double deg = floor(Latitude);
    double min = (Latitude - deg) * 60.0;
    // move the degrees over in the field and add in the minutes 
    deg = 100.0*deg + min;
    // format string
    snprintf(txt, sizeof(txt), "%12.8f,%c,", deg, NS);

    return string(txt);
}
/**
 ******************************************************************
 *
 * Function Name : EncodeLongitude
 *
 * Description : Encode NMEA Longitude
 *
 * Inputs : Longitude in radians. 
 *
 * Returns : NMEA0183 message format with 8 decimal places (1mm)
 *
 * Error Conditions :
 * 
 * Unit Tested on: 03-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string EncodeLongitude(const double &L)
{
    SET_DEBUG_STACK;
    char txt[256];
    char EW = 'E';

    double Longitude = fabs(L * RadToDeg); // take the sign away
    if (L<0.0)
	EW = 'W';

    /*
     * Get the three fields to make it look like:
     * 07116.4541
     * DDDMM.xxxx
     * small change use upto 8 decimal places. 
     * 
     */
    double deg = floor(Longitude);
    double min = (Longitude - deg) * 60.0;
    // move the degrees over in the field and add in the minutes 
    //deg = 100.0*deg + min;
    uint32_t ideg = (uint32_t) deg;
    uint32_t imin = (uint32_t) min;
    double sec    = fabs((min - ceil(min)));
    // format string
    snprintf(txt, sizeof(txt), "%03d%2d%.8f,%c,", ideg,imin,sec,EW);

    return string(txt);
}
/**
 ******************************************************************
 *
 * Function Name : EncodeUTCSeconds
 *
 * Description : return an encoded string with the number of
 *               seconds into the current day relative to UTC. 
 *
 * Inputs : 
 *        now - seconds since epoch. 
 *        ms  - milliseconds field. 
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 04-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string EncodeUTCSeconds(const time_t& now, float ms)
{
    SET_DEBUG_STACK;
    char txt[32];
    // get the details of the day. 
    struct tm det;
    localtime_r(&now, &det); // no offset
    // Zero out h,m,s
    det.tm_hour = 0;
    det.tm_min  = 0;
    det.tm_sec  = 0;
    time_t day = mktime(&det);   // midnight
    // set to time since midnight into the day and add on the ms. 
    float result = (float)(now - day) + ms/1000.0;   
    snprintf( txt, sizeof(txt), "%6.3f", result);
    return string(txt);
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs : NMEA sentance
 *
 * Returns : NMEA checksum of sentance provided
 *
 * Error Conditions : None
 * 
 * Unit Tested on: 04-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t Checksum(const string &val)
{
    uint8_t rv = 0;
    uint8_t bb;

    // exclude the begin and end of sentance $ and *
    uint32_t i = 0;
    do
    {
	bb = (uint8_t)val[i];
	if ((bb != '$') && (bb !='*'))
	    rv ^= bb;
	i++;
    } while((i<val.length()) && (val[i] != '*'));  // kicks out if handed a fully formed sentance. 
    return rv;
}
