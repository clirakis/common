/********************************************************************
 *
 * Module Name : VTG.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic VTG
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 16-Apr-26 changed how decoding is done and added a clear function. 
 *
 * Classification : Unclassified
 *
 * References :
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/nmea0183-messages-vtg.html?Highlight=VTG
 * https://docs.fixposition.com/fd/nmea-gp-vtg
 *
 * From fixposition site:
 *    $GNVTG,0.0000,T,,M,0.01316,N,0.02437,K,D*3F\r\n
 *
 * From GTOP:
 *    $GPVTG,12.71,T,,M,0.10,N,0.18,K,D*05
 *
 * Records
 * 0 - Message ID
 * 1 - Track made good, degrees true
 * 2 T: trck made good relative to true north
 * 3 Track made good - magnetic
 * 4 M - specify magnetic north
 * 5 Speed in knots
 * 6 N - specifying knots
 * 7 Speed over ground in kph
 * 8 K - specifying that KPH
 * 9 Mode:
 *    A: Autonomous
 *    D: Differential
 *    E: Estimated or Dead reconing
 *    M: Manual input
 *    S: Simulator
 *    N: Invalid data. 
 *    
 * 10 Checksum
 *    
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
#include "VTG.hh"
#include "NMEA_helper.hh"
#include "tools.h"


/**
 ******************************************************************
 *
 * Function Name :  VTG constructor
 *
 * Description : 
 *
 * Inputs : NONE
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested on: 05-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
VTG::VTG(void)
{
    SET_DEBUG_STACK;
    Clear();
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : VTG destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 05-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
VTG::~VTG (void)
{
    // Nothing to be done
}

/**
 ******************************************************************
 *
 * Function Name : VTG Decode given a string
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
bool VTG::Decode(const char *line)
{
    SET_DEBUG_STACK;
    bool rc = true;
    string         token, token2;
    istringstream  sstream(line);
    istringstream  ss2;


    // loop over all fields and put the result into token. 
    uint32_t i = 0;
    while (getline(sstream, token, ','))
    {
        //cout << i << " " << token << " " << token.size() << endl;
	if(token.size()>0)
	{
	    switch(i)
	    {
	    case 0:
		// Should be VTG preamble. 
		break;
	    case 1:
		// CMG True
		fTrue = stof(token) * DegToRad;
		break;
	    case 2:
		// should be T meaning true
		if (token[0] != 'T')
		    return false;
		break;
	    case 3:
		fMagnetic = stof(token) * DegToRad; // CMG
		break;
	    case 4:
		if (token[0] != 'M')
		    return false;
		break;
	    case 5:
		fSpeedKnots = stof(token);
		break;
	    case 6:
		if (token[0] != 'N')
		    return false;
		break;
	    case 7:
		fSpeedKPH = stof(token);
		break;
	    case 8:
		if (token[0] != 'K')
		    return false;
		break;
	    case 9:
		ss2.str(token);
		getline(ss2, token2, '*');
		if (token2.size()>0)
		{
		    // mode and no space checksum
		    fMode = token2[0];

		    // Checksum FIXME
		}
		else
		{
		    rc = false;
		}
		break;
	    default:
		break;
	    }
	}
	i++;
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Encode
 *
 * Description : Make a NMEA VTG string
 *
 * Inputs : NONE
 *
 * Returns : string with NMEA VTG message 
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
string VTG::Encode(void)
{
    SET_DEBUG_STACK;

    char txt[16];
    string rv("$GPVTG,");

    snprintf(txt,sizeof(txt),"%5.2f,T,", fTrue );
    rv += txt;
    snprintf(txt,sizeof(txt),"%5.2f,M,", fMagnetic);
    rv += txt;
    snprintf(txt,sizeof(txt),"%4.2f,N,", fSpeedKnots);
    rv += txt;
    snprintf(txt,sizeof(txt),"%4.2f,K,", fSpeedKPH);
    rv += txt;
    switch(fMode)
    {
    case kAUTONOMOUS:
	rv += "A*";
	break;
    case kDIFFERENTIAL:
	rv += "D*";
	break;
    case kDR:
	rv += "E*";
	break;
    case kMANUAL:
	rv += "M*";
	break;
    case kSIMULATION:
	rv += "S*";
	break;
    case kNONE:
    default:
	rv += "N*";
	break;
    }
    snprintf(txt, sizeof(txt), "%2.2X", Checksum(rv));
    rv += txt;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : operator<< for VTG
 *
 * Description : formats the data from the VTG class
 *
 * Inputs : VTG class. 
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
ostream& operator<<(ostream& output, const VTG &n)
{
    SET_DEBUG_STACK;
    output << "VTG::" << endl
	   << "         COG: " << n.fTrue * RadToDeg << endl
	   << "    Magnetic: " << n.fMagnetic * RadToDeg << endl
	   << "    Speed(K): " << n.fSpeedKnots << endl
	   << "  Speed(kPH): " << n.fSpeedKPH << endl
	   << "        Mode: " << n.ModeStr() << endl;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : SetKnots
 *
 * Description : Set the speed in knots. Since the speeds are related
 *               sets KPH at the same time. 
 *
 * Inputs : v - speed in knots
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
void VTG::SetKnots(float v)
{
    SET_DEBUG_STACK;
    fSpeedKnots = v;
    fSpeedKPH   = 1.852*v;
}
/**
 ******************************************************************
 *
 * Function Name : SetKPH
 *
 * Description : Sets KPH, but also updates Knots field
 *
 * Inputs : v - KPH value to update to
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 04-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void VTG::SetKPH(float v)
{
    SET_DEBUG_STACK;
    fSpeedKPH = v;
    fSpeedKnots = v/1.852;
}
/**
 ******************************************************************
 *
 * Function Name : ModeStr
 *
 * Description : Give a string descriptor for the current mode used.
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  04-Nov-25
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string VTG::ModeStr(void) const
{
    SET_DEBUG_STACK;
    string rv;
    //cout << fMode << endl;
    switch(fMode)
    {
    case kAUTONOMOUS:
	rv = "Autonomous";
	break;
    case kDIFFERENTIAL:
	rv = "Differential";
	break;
    case kDR:
	rv = "Dead reconing";
	break;
    case kMANUAL:
	rv = "Manual";
	break;
    case kSIMULATION:
	rv = "Simulation";
	break;
    case kNONE:
    default:
	rv = "NONE or Data not valid";
	break;
    }
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name :  Clear
 *
 * Description : 
 *
 * Inputs : NONE
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested on: 16-Apr-26
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void VTG::Clear(void)
{
    SET_DEBUG_STACK;
    fTrue       = 0.0;  // Course True
    fMagnetic   = 0.0;  // course magnetic
    fSpeedKnots = 0.0;
    fSpeedKPH   = 0.0; 
    fMode       = 'N';  // Data no set
    SET_DEBUG_STACK;
}
