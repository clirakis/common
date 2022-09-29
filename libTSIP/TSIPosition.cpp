/********************************************************************
 *
 * Module Name : TSIPosition.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic TSIPosition,  This contains
 * all the data associated with position packets, 0x4A - single precision
 * position, or 0x8A double precision position. 
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

// Local Includes.
#include "debug.h"
#include "TSIPosition.hh"

/**
 ******************************************************************
 *
 * Function Name : TSIPosition constructor
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
TSIPosition::TSIPosition (void) : DataTimeStamp()
{
    SET_DEBUG_STACK;
    Clear();
}

/**
 ******************************************************************
 *
 * Function Name : TSIPosition destructor
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
TSIPosition::~TSIPosition (void)
{
    SET_DEBUG_STACK;
}


/**
 ******************************************************************
 *
 * Function Name : TSIPosition function
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
void TSIPosition::Clear(void)
{
    SET_DEBUG_STACK;
    fValid = false;
    fLatitude = fLongitude = fAltitude = 0.0;
    fClk = fSec = 0.0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SolutionStatus operator << overload
 *
 * Description :
 *
 * Inputs : output stream to append to
 *          n - Position class
 *
 * Returns : packed ostream to do with as you please. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const TSIPosition &n)
{
    SET_DEBUG_STACK;
    output << " Position, " 
	   << "Latitude: "    << n.Latitude() * 180.0/M_PI
	   << " Longitude: "  << n.Longitude() * 180.0/M_PI
	   << " Altitude: "   << n.Altitude()
	   << " Clock bias: " << n.ClockBias()
	   << " Seconds: "    << n.Seconds()
	   << " Valid: "      << n.Valid();

    SET_DEBUG_STACK;
    return output;
}
