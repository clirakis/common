/********************************************************************
 *
 * Module Name : NMEA_Position.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic NMEA_Position
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
#include "NMEA_Position.hh"

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
NMEA_Position::NMEA_Position(void)
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
 * Function Name : module destructor
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


/**
 ******************************************************************
 *
 * Function Name : module function
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
