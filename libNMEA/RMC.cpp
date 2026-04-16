/********************************************************************
 *
 * Module Name : RMC.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic RMC
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/nmea0183-messages-rmc.html?tocpath=Output%20Messages%7CNMEA-0183%20messages%7C_____23
 *
 * https://docs.fixposition.com/fd/nmea-gp-rmc
 * 
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>

// Local Includes.
#include "debug.h"
#include "RMC.hh"
#include "NMEA_helper.hh"

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
RMC::RMC(void) : NMEA_Position()
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
#if 0
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
#else
    string         token;
    istringstream  sstream(line);

    // Clear out contents
    Clear();

    // loop over all fields and put the result into token. 
    uint32_t i = 0;
    while (getline(sstream, token, ','))
    {
        cout << i << " " << token << " " << token.size() << endl;
	if(token.size()>0)
	{
	    switch(i)
	    {
	    case 0:
		// Should be the $GPRMC 
		break;
	    case 1:
		// Time field
		/*
		 * Return the seconds into the UTC day AND
		 * fill H,M,S portion of struct time.
		 * The remainder will be filled below 
		 */
		fUTC     = atof(token.c_str());
		fSeconds = DecodeUTCFixTime( token.c_str(), &fMilliseconds, &now);
		break;
	    case 2:
		fMode = token[0];
		break;
	    case 3:
		fLatitude = DecodeDegMin(token.c_str());
		break;
	    case 4:
		if (token[0] == 'S') fLatitude *= -1.0;
		break;
	    case 5:
		fLongitude = DecodeDegMin(token.c_str());
		break;
	    case 6:
		if (token[0] == 'W') fLongitude *= -1.0;
		break;
	    case 7:
		fSpeed = stof(token);
		break;
	    case 8:
		fCMG = stof(token);
		cout << "CMG: " << token << " " << fCMG << endl;
		break;
	    case 9:
		/*
		 * Pass in the partially filled UTC message, from above
		 * H,M,S should be filled. 
		 * This decode should provide DDMMYY
		 */
		fSeconds = DecodeDate(token.c_str(), &now);
		/* PC Time is local, convert to UTC */
		float dt   = (float) (fPCTime.tv_sec - fSeconds + timezone);
		dt  -= fMilliseconds/100.0;
		tmp     = fPCTime.tv_nsec;
		tmp     /= 1.0e9;
		dt  += tmp;
		/* Super simple LPF */
		fDelta = k*fDelta + (1.0-k)*dt;
		break;
	    }
	}
	i++;
    }


#endif
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
void RMC::Clear(void)
{
    SET_DEBUG_STACK;
    /** difference between PC time and GPS time  in seconds */
    fDelta = 0.0;
    /**
     * Speed in knots
     */
    fSpeed = 0.0;
    /**
     * Course made good
     */
    fCMG = 0.0; 
    /**
     * Magnetic Variation. 
     */
    fMagVariation = 0.0;
    /**
     * FIX MODE
     */
    fMode = 'V';
    memset(fPlaceholder, 0, 3);
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
ostream& operator<<(ostream& output, const RMC &n)
{
    SET_DEBUG_STACK;

    string UTC = EncodeUTCTime(n.fSeconds, n.fMilliseconds);
    output << "NMEA_Position::" << endl
	   << "         Latitude: " << n.fLatitude * RadToDeg << endl
	   << "        Longitude: " << n.fLongitude * RadToDeg << endl
	   << "              UTC: " << UTC << endl
           << "              Fix: " << (uint32_t) n.fMode << endl;
    output << "RMC:" << endl
	   << "      Speed (KTS): " << n.fSpeed << endl
	   << "              CMG: " << n.fCMG << endl
	   << "        Mag. Var.: " << n.fMagVariation << endl
	   << "             Mode: " << n.fMode 
	   << endl;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name :  Encode RMC value in NMEA format
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
 * GTOP line:
 *
 * $GPRMC,003936.000,A,4129.0793,N,07116.4541,W,0.10,12.71,011125,,,D*42
 *
 *******************************************************************
 */
string RMC::Encode(void)
{
    SET_DEBUG_STACK;
    char txt[16];

    string rv("$GPRMC,");
    string UTC = EncodeUTCTime( fSeconds, fMilliseconds);
    rv = rv + UTC + ",A," + 
	EncodeLatitude(fLatitude) + 
	EncodeLongitude(fLongitude);

    snprintf(txt,sizeof(txt),"%5.2f,",fSpeed);
    rv += txt;
    
    snprintf(txt,sizeof(txt),"%5.2f,",fCMG*RadToDeg);
    rv += txt;
    
    struct tm tme;
    localtime_r(&fSeconds, &tme);
    strftime( txt, sizeof(txt), "%d%m%y", &tme);
    rv = rv + txt + ",";
    
    float var = fabs(fMagVariation);
    snprintf(txt,sizeof(txt),"%3.2f,", var);
    rv += txt;
    
    if(fMagVariation>0.0)
    {
	rv += "E,";
    }
    else
    {
	rv += "W,";
    }

    rv += fMode;
    rv += "*";

    snprintf(txt,sizeof(txt),"%2.2X",Checksum(rv));
    rv += txt;

    return rv;
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
void RMC::SetPosition(const NMEA_Position &p)
{
    SET_DEBUG_STACK;
    fPCTime    = p.PCTime();
    fLatitude  = p.Latitude();
    fLongitude = p.Longitude();   // In radians. 
    fSeconds   = p.Seconds();     // epoch if decoded correctly
    fUTC       = p.UTC();         // seconds in current day, UTC
    fMilliseconds = p.Milli();         // ms on time of fix

}
