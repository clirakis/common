/********************************************************************
 *
 * Module Name : TSIPVelocity.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic Velocity
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
#include "TSIPVelocity.hh"

/**
 ******************************************************************
 *
 * Function Name : TSIPVelocity constructor
 *
 * Description : Just set the private variables to zero
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
TSIPVelocity::TSIPVelocity (void) : DataTimeStamp()
{
    SET_DEBUG_STACK;
    Clear();
}

/**
 ******************************************************************
 *
 * Function Name : TSIPVelocity destructor
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
TSIPVelocity::~TSIPVelocity ()
{
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : reset private variables to zero
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void TSIPVelocity::Clear(void)
{
    SET_DEBUG_STACK;
    fEast = fNorth = fUp = 0.0f;
    fClkBiasRate = fSec = 0.0f;
}

/**
 ******************************************************************
 *
 * Function Name : operator<<
 *
 * Description : ostream operator overload to format the Velocity
 * data into a stream. 
 *
 * Inputs : RawTracking class. 
 *
 * Returns : ostream
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
ostream& operator<<(ostream& output, const TSIPVelocity &n)
{
    SET_DEBUG_STACK;

    output << " Velocity, North:" << n.North()
	   << " East:"            << n.East()
	   << " Up:"              << n.Up() << endl
	   << " Clock Bias Rate:" << n.ClockBiasRate()
	   << " Time:"            << n.Seconds();

    SET_DEBUG_STACK;
    return output;
}
