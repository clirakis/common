/**
 ******************************************************************
 *
 * Module Name : NMEA_GPS.cpp
 *
 * Author/Date : C.B. Lirakis / 19-Feb-22
 *
 * Description : started with GTOP, now stand alone
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions : 
 * 17-Dec-23    CBL The place where we are getting time
 *              is always zero. 
 *              Adding in system time as a parameter as well. 
 * 
 * 20-Dec-23    Never changed filenames. 
 * 10-Mar-24    Added in GPS-pc time delta. 
 * 24-Mar-24    Added in TOD 
 * 28-Apr-24    made into SO library
 * 
 * Classification : Unclassified
 *
 * References : 
 *     https://cdn-shop.adafruit.com/datasheets/GlobalTop+MT3339+PC+Tool+Operation+Manual+v1.1.pdf
 *     https://www.adafruit.com/product/790
 *     GlobalTop+MT3339+PC+Tool+Operation+Manual+v1.1.pdf
 * 
 *
 *
 *******************************************************************
 */  
#include <iostream>
using namespace std;
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

// Local includes
#include "tools.h"        // Units conversions etc. 
#include "debug.h"
#include "NMEA_GPS.hh"

NMEA_GPS* NMEA_GPS::fNMEA;  

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
static time_t DecodeUTCFixTime(const char *p, float *ms, struct tm *now)
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
static time_t DecodeDate(const char *p, struct tm *now)
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
static double DecodeDegMin(const char *p)
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
NMEA_POSITION::NMEA_POSITION(void)
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
GGA::GGA(void) : NMEA_POSITION()
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
 * Function Name :  RMC Constructor
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
RMC::RMC(void) : NMEA_POSITION()
{
    SET_DEBUG_STACK;
    fSpeed  = 0.0;
    fCMG    = 0.0;
    fMagVariation = 0.0;
    fMode   = 'N';
    fDelta  = 0.0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name :  RMC decode
 *
 * Description :  decode an RMC message of format
 *     RMC - Recommende minimum sentance C
 *     HHMMSS.ss - time
 *     A - status (A)ctive or (V)oid
 *     DDMM.mmm  - Latitude
 *     N/S       - Latitude Hemisphere
 *     DDDMM.mmm - Longitude
 *     E/W       - East/west hemisphere
 *     Speed - Knots
 *     Heading
 *     DDMMYY    - date 
 *     Magnetic Variation
 *     Magnetic Variation Direction
 *     A - Mode indicator
 *        (A)utonomous
 *        (D)ifferential
 *        (E)stimated
 *        (F)loat RTK
 *        (M)anual input
 *        (N)o fix
 *        (P)recise
 *        (R)TK
 *        (S)imulator
 *     CHECKSUM
 *
 * Inputs : character line to decode. 
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested on: NOT TESTED
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool RMC::Decode(const char *line)
{
    const float k = 0.95;

    SET_DEBUG_STACK;
    double tmp;
    struct tm now;
    // Capture the PC time of the message. 
    clock_gettime( CLOCK_REALTIME, &fPCTime);

    // found RMC
    char *p = (char *) line;

    // get time
    p = strchr(p, ',')+1;
    /*
     * Return the seconds into the UTC day AND
     * fill H,M,S portion of struct time.
     * The remainder will be filled below 
     */
    fUTC     = atof(p);
    fSeconds = DecodeUTCFixTime( p, &fMilliseconds, &now);

    p = strchr(p, ',')+1;
    fMode = p[0];
#if 0
    //if (p[0] == 'A') 
//	fix = true;
    else if (p[0] == 'V')
	fix = false;
    else
	return false;
#endif
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
    // speed
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fSpeed = atof(p);
    }
    
    // angle
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	fCMG = atof(p);
    }
    
    p = strchr(p, ',')+1;
    if (',' != *p)
    {
	/*
	 * Pass in the partially filled UTC message, from above
	 * H,M,S should be filled. 
	 * This decode should provide DDMMYY
	 */
	fSeconds = DecodeDate(p, &now);
	/* PC Time is local, convert to UTC */
	float dt   = (float) (fPCTime.tv_sec - fSeconds + timezone);
	dt  -= fMilliseconds/100.0;
	tmp     = fPCTime.tv_nsec;
	tmp     /= 1.0e9;
	dt  += tmp;
	/* Super simple LPF */
	fDelta = k*fDelta + (1.0-k)*dt;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name :  
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
VTG::VTG(void)
{
    SET_DEBUG_STACK;
    fTrue       = 0.0;  // Course True
    fMagnetic   = 0.0;  // course magnetic
    fSpeedKnots = 0.0;
    fSpeedKPH   = 0.0; 
    fMode       = 'N';
    SET_DEBUG_STACK;
}
bool VTG::Decode(const char *line)
{
    SET_DEBUG_STACK;
    char *p = (char *) line;

    p = strchr(p, ',')+1;
    fTrue = atof(p);      // Course True

    p = strchr(p, ',')+1;
    if (*p != 'T')
	return false;

    p = strchr(p, ',')+1;
    fMagnetic = atof(p);  // course magnetic

    p = strchr(p, ',')+1;
    if (*p != 'M')
	return false;

    p = strchr(p, ',')+1;
    fSpeedKnots = atof(p);

    p = strchr(p, ',')+1;
    if (*p != 'N')
	return false;

    p = strchr(p, ',')+1;
    fSpeedKPH = atof(p); 

    p = strchr(p, ',')+1;
    if (*p != 'K')
	return false;

    p = strchr(p, ',')+1;
    fMode = *p;
    SET_DEBUG_STACK;
    return true;
}
GSA::GSA(void)
{
    SET_DEBUG_STACK;
    fMode1 = fMode2 = 'N';
    memset( fSatellite, 0, sizeof(fSatellite));
    fPDOP=fHDOP=fVDOP = 0.0;
    SET_DEBUG_STACK;
}
bool GSA::Decode(const char *line)
{
    SET_DEBUG_STACK;
    int  i;
    char *p = (char *) line;

    p = strchr(p, ',')+1;
    fMode1 = *p;

    p = strchr(p, ',')+1;
    fMode2 = atoi(p);

    for (i=0;i<12;i++)
    {
	p = strchr(p, ',')+1;
	fSatellite[i] = atoi(p);
    } 
    p = strchr(p, ',')+1;
    fPDOP = atof(p);

    p = strchr(p, ',')+1;
    fHDOP = atof(p);

    p = strchr(p, ',')+1;
    fVDOP = atof(p);
    SET_DEBUG_STACK;
    return true;
}
ostream& operator<<(ostream& output, const GSA &n)
{
    output << "GSA data ================================= " << endl
	   << "  PDOP: " << n.fPDOP << endl
	   << "  HDOP: " << n.fHDOP << endl
	   << "  VDOP: " << n.fVDOP << endl
	   << " Mode1: " << n.fMode1 << endl
	   << " Mode2: " << n.fMode2 << endl;
    for (int i=0;i<12;i++)
    {
	output << (unsigned int) n.fSatellite[i] << " ";
    }
    output << endl
	   << " ========================================= " << endl;
    return output;
}


/**
 ******************************************************************
 *
 * Function Name : NMEA_GPS
 *
 * Description : Constructor
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
NMEA_GPS::NMEA_GPS( void)
{
    SET_DEBUG_STACK;

    fNMEA = this;
    fRMC = new RMC();
    fGGA = new GGA();
    fVTG = new VTG();
    fGSA = new GSA();
    SET_DEBUG_STACK;
}
NMEA_GPS::~NMEA_GPS(void)
{
    SET_DEBUG_STACK;
    delete fRMC;
    delete fGGA;
    delete fVTG;
    SET_DEBUG_STACK;
}
// read a Hex value and return the decimal equivalent
uint8_t NMEA_GPS::parseHex(char c) 
{
   SET_DEBUG_STACK;
   if (c < '0')
	return 0;
    if (c <= '9')
	return c - '0';
    if (c < 'A')
	return 0;
    if (c <= 'F')
	return (c - 'A')+10;
    // if (c > 'F')
    SET_DEBUG_STACK;
    return 0;
}

bool NMEA_GPS::CheckSum(const char *line)
{
    SET_DEBUG_STACK;

    /*
     * do checksum check
     * first look if we even have one
     */
    if (line[strlen(line)-4] == '*') 
    {
	uint16_t sum = parseHex(line[strlen(line)-3]) * 16;
	sum += parseHex(line[strlen(line)-2]);
    
	// check checksum 
	for (uint32_t i=2; i < (strlen(line)-4); i++) 
	{
	    sum ^= line[i];
	}
	if (sum != 0) 
	{
	    // bad checksum :(
	    return false;
	}
    }
    SET_DEBUG_STACK;
    return true;
}
bool NMEA_GPS::parse(const char *nmea) 
{
    SET_DEBUG_STACK;
    bool    rc = false;

    /*
     * if possible need VTG, XTC, WPL, APB- auto pilot b
     */

    // look for a few common sentences
    if (strstr(nmea, "$GPGGA")) {
	// found GGA
	fLastID = MESSAGE_GGA; // Position
	rc = fGGA->Decode(nmea);
    }
    else if (strstr(nmea, "$GPRMC")) 
    {
	fLastID = MESSAGE_RMC; // Recommended navigation data.
	rc = fRMC->Decode(nmea);
    }
    else if (strstr(nmea, "$GPVTG"))
    {
	// Course and speed. 
	fLastID = MESSAGE_VTG;
	rc = fVTG->Decode(nmea);
    }
    else if (strstr(nmea, "$GPGSA"))
    {
	// Active satellite data.
	fLastID = MESSAGE_GSA;
	rc = fGSA->Decode(nmea);
    }
    else if (strstr(nmea, "$GPGSV"))
    {
	// GNSS Satellites in view.
	fLastID = MESSAGE_GSV;
	rc = true;
    }
    SET_DEBUG_STACK;

    return rc;
}



