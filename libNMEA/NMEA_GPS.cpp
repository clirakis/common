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
 * 20-Apr-26    put Checksum up front before doing the full decode.
 * 
 * Classification : Unclassified
 *
 * References : 
 *     https://cdn-shop.adafruit.com/datasheets/GlobalTop+MT3339+PC+Tool+Operation+Manual+v1.1.pdf
 *     https://www.adafruit.com/product/790
 *     GlobalTop+MT3339+PC+Tool+Operation+Manual+v1.1.pdf
 * 
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/NMEA-0183messages_GGA.html
 * 
 * https://docs.fixposition.com/fd/nmea-gp-gsa
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
#include <sstream>

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
/**
 ******************************************************************
 *
 * Function Name : NMEA_GPS
 *
 * Description : Destructor
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


/**
 ******************************************************************
 *
 * Function Name : Checksum
 *
 * Description : 
 *
 * Inputs : line to parse,
 *
 * Returns : false if checksum is bad or missing. 
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
bool NMEA_GPS::CheckSum(const char *input)
{
    SET_DEBUG_STACK;
    bool     rv = false;     // assume failure.
    string   line(input);    // make a local copy.
    uint8_t  sum =0;         // sum at end

    size_t n = line.find("*");
    if (n != string::npos)
    {
	/*
	 * get the checksum value. 
	 */
	string val = line.substr(n+1,string::npos);
	if (val.size()<=3)
	{
	    uint32_t expected = stoi(val, nullptr, 16);

	    size_t start = line.find("$") + 1;
	    /* 
	     * do everything between $ and *, the line delimiters. 
	     */
	    for (uint32_t i=start; i < n; i++) 
	    {
		sum ^= line[i];
	    }
	    rv = (sum == expected);
	}
    }
    SET_DEBUG_STACK;
    return rv;
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
bool NMEA_GPS::parse(const char *nmea) 
{
    SET_DEBUG_STACK;
    bool    rc = false;
    string line(nmea);        // make a local copy

    /*
     * if possible need VTG, XTC, WPL, APB- auto pilot b
     *
     * Find and perform the checksum first. 
     */
    if (!CheckSum(nmea))
    {
	return false; // bad checksum
    }
    
    // look for a few common sentences
    if (strstr(nmea, "$GPGGA")) 
    {
	// found GGA
	fLastID = kMESSAGE_GGA; // Position
	rc = fGGA->Decode(nmea);
    }
    else if (strstr(nmea, "$GPRMC")) 
    {
	fLastID = kMESSAGE_RMC; // Recommended navigation data.
	rc = fRMC->Decode(nmea);
    }
    else if (strstr(nmea, "$GPVTG"))
    {
	// Course and speed. 
	fLastID = kMESSAGE_VTG;
	rc = fVTG->Decode(nmea);
    }
    else if (strstr(nmea, "$GPGSA"))
    {
	// Active satellite data.
	fLastID = kMESSAGE_GSA;
	rc = fGSA->Decode(nmea);
    }
    else if (strstr(nmea, "$GPGSV"))
    {
	// GNSS Satellites in view.
	fLastID = kMESSAGE_GSV;
	rc = true;
    }
    SET_DEBUG_STACK;

    return rc;
}


