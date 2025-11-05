/********************************************************************
 *
 * Module Name : GGA.cpp
 *
 * Author/Date : C.B. Lirakis / 03-Nov-25
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
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
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
#include "tools.h"

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
 * Unit Tested on: 04-Nov-25
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
    fSatellites   = 0;
    fHDOP         = 0.0;
    fAge          = 0.0;
    fStationID    = 0;
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
 * Function Name : Encode
 *
 * Description : take the data we have and encode it into a string
 *               message
 *
 * Reference: https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
 * Fields
 *  0 - MESSAGE ID GPGGA
 *  1 - UTC of position fix
 *  2 - Latitude DDMM.ssssssss
 *  3 - N or S
 *  4 - Longitude DDDMM.ssssssss
 *  5 - E or W
 *  6 - Fix quality
 *      0 - fix invalid
 *      1 - GPS fix
 *      2 - Differential (DGNSS, SBAS, OminSTAR, VBS, Beacon, RTX, GVBS
 *      3 - N/A
 *      4 - RTK Fixed
 *      5 - RTK Float
 *      6 - INS dead reconing
 *  7 - NSVs used in solution
 *  8 - HDOP
 *  9 - Orthometric height (MSL)
 * 10 - M units of measure
 * 11 - Geiod separation
 * 12 - M units of geoid sep
 * 13 - Age of differential data 
 * 14 - Reference Station ID {0000:4095}
 * 15 - CKSUM
 *
 * Inputs : NONE
 *
 * Returns : std::string
 *
 * Error Conditions :
 * 
 * Unit Tested on: 4-Nov-25
 *
 * Unit Tested by: CBL
 *
 * Example format from the trimble website:
 * $GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2,6,1.2,18.893,M,-25.669,M,2.0 0031*4F
 *
 *
 * This one is from a GTOP receiver:
 * $GPGGA,003937.000,4129.0793,N,07116.4541,W,2,11,0.94,23.9,M,-34.4,M,0000,0000*6E
 *
 *  Note: 
 *  UTC time is odd
 *  
 *
 *******************************************************************
 */
string GGA::Encode(void)
{
    SET_DEBUG_STACK;
    char txt[16];

    // The first entry is seconds into the day UTC. 
    string utime = EncodeUTCSeconds(fUTC, fMilliseconds);
    string rv = string("$GPGGA,") + utime + string(",") + 
	EncodeLatitude(fLatitude) +
	EncodeLongitude(fLongitude) +
	to_string(fFixIndicator) + "," +
	to_string(fSatellites) + ",";
    snprintf(txt,sizeof(txt),"%4.2f",fHDOP);
    rv = rv + txt + ",";
    snprintf(txt,sizeof(txt),"%4.4f",fAltitude);
    rv = rv + txt + ",M,";
    snprintf(txt,sizeof(txt),"%4.4f",fGeoidheight);
    rv = rv + txt + ",M,";

    snprintf(txt, sizeof(txt),"%3.1f", fAge);
    rv = rv + txt + ","; 
    snprintf(txt, sizeof(txt),"%4.4d", fStationID);
    rv = rv + txt +"*";
    snprintf(txt, sizeof(txt),"%2.2X", Checksum(rv));
    rv = rv + txt +"\n\r";

    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : operator<< for GGA
 *
 * Description : formats the data from the GGA.
 *
 * Inputs : GGA messgae to parse. 
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
ostream& operator<<(ostream& output, const GGA &n)
{
   
    string UTC = EncodeUTCSeconds(n.fUTC, n.fMilliseconds);
    output << "NMEA_Position::" << endl
	   << "         Latitude: " << n.fLatitude * RadToDeg << endl
	   << "        Longitude: " << n.fLongitude * RadToDeg << endl
	   << " Seconds into day: " << UTC << endl
           << "              Fix: " << (uint32_t) n.fFixIndicator << endl
	   << "        Satelites: " << (uint32_t) n.fSatellites << endl
	   << "             HDOP: " << n.fHDOP << endl
	   << "              Age: " << n.fAge << endl
	   << "       Station ID: " << n.fStationID << endl;
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
