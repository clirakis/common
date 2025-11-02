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
#include <cstring> 

// Local Includes.
#include "debug.h"
#include "VTG.hh"


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
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
VTG::~VTG (void)
{
}

/**
 ******************************************************************
 *
 * Function Name : VTG function
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
