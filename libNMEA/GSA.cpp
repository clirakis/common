/********************************************************************
 *
 * Module Name : GSA.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic GSA
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 19-Apr-26 New method of decoding. 
 *
 * Classification : Unclassified
 *
 * References :
 * https://docs.fixposition.com/fd/nmea-gp-gsa
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/nmea0183-messages-gsa.html?tocpath=Output%20Messages%7CNMEA-0183%20messages%7C_____8
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <sstream>

// Local Includes.
#include "debug.h"
#include "GSA.hh"
#include "NMEA_helper.hh"

/**
 ******************************************************************
 *
 * Function Name : GSA constructor
 *
 * Description :
 *
 * Inputs : NONE
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
GSA::GSA(void)
{
    SET_DEBUG_STACK;
    Clear();
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : Given a NMEA string, extract the constants
 *
 * Inputs : character string containing NMEA GSA message
 *
 * Returns : True on success
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
bool GSA::Decode(const char *line)
{
    SET_DEBUG_STACK;
    string         token, token2;
    istringstream  sstream(line);
    istringstream  ss2;

    uint32_t  i = 0;
    uint32_t  j = 0;


    Clear();
    while (getline(sstream, token, ','))
    {
        //cout << i << " " << token << " " << token.size() << endl;
	if(token.size()>0)
	{
	    switch(i)
	    {
	    case 0:
		// Should be the GSA preamble. Do nothing
		break;
	    case 1:
		// Mode M or A
		fMode1 = token[0];
		break;
	    case 2:
		// Fix type 1 - not available, 2 - 2D, 3 - 3D
		fMode2 = stoi(token);
		break;
	    case 3:           // 0
		// Start of satellites, upto 12
	    case 4:           // 1
	    case 5:           // 2
	    case 6:           // 3
	    case 7:           // 4
	    case 8:           // 5
	    case 9:           // 6
	    case 10:          // 7
	    case 11:          // 8
	    case 12:          // 9
	    case 13:          // 10
	    case 14:          // 11
		fSatellite[j] = stoi(token);
		j++;
		break;
	    case 15: 
		fPDOP = stof(token);
		break;
	    case 16:
		fHDOP = stof(token);
		break;
	    case 17:
		ss2.str(token);
		// now separate the VDOP from the Checksum
		getline(ss2, token2, '*');
		fVDOP = stof(token2);
		// FIXME - missing CKSUM
		break;
	    }
	}
	i++;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : operator << overload for GSA
 *
 * Description :
 *
 * Inputs :
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
ostream& operator<<(ostream& output, const GSA &n)
{
    output << "GSA data ================================= " << endl
	   << "  PDOP: " << n.fPDOP << endl
	   << "  HDOP: " << n.fHDOP << endl
	   << "  VDOP: " << n.fVDOP << endl
	   << " Mode1: " << n.fMode1 << endl
	   << " Mode2: " << (int) n.fMode2 << endl;
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
 * Function Name : Encode
 *
 * Description : Make a NMEA GSA string
 *
 * Inputs : NONE
 *
 * Returns : std::string GSA message
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 * example from GTOP receiver
 *
 * $GPGSA,A,3,30,19,10,07,13,22,01,08,17,02,14,,1.18,0.88,0.79*03
 *
 *******************************************************************
 */
string GSA::Encode(void)
{
    SET_DEBUG_STACK;
    char txt[16];

    string rv("$GPGSA,");
    rv = rv + fMode1 + ",";
    snprintf(txt,sizeof(txt),"%1.1d,", fMode2);
    rv += txt;

    for(int i=0;i<12;i++)
    {
	snprintf(txt, sizeof(txt), "%2.2d,", fSatellite[i]);
	rv += txt;
    }

    snprintf(txt,sizeof(txt),"%4.2f,",fPDOP);
    rv += txt;

    snprintf(txt,sizeof(txt),"%4.2f,",fHDOP);
    rv += txt;

    snprintf(txt,sizeof(txt),"%4.2f*",fVDOP);
    rv += txt;

    snprintf(txt,sizeof(txt),"%2.2X",Checksum(rv));
    rv += txt;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description :
 *
 * Inputs : NONE
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
void GSA::Clear(void)
{
    SET_DEBUG_STACK;
    fMode1 = fMode2 = 'N';
    memset( fSatellite, 0, sizeof(fSatellite));
    fPDOP=fHDOP=fVDOP = 0.0;
    SET_DEBUG_STACK;
}
