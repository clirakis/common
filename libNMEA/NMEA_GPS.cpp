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



