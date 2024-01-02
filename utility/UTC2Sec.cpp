/********************************************************************
 *
 * Module Name : UTC2Sec.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic UTC2Sec
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
#include "UTC2Sec.hh"

/**
 ******************************************************************
 *
 * Function Name : UTC2Sec 
 *
 * Description : provide seconds into day from GPS UTC
 *
 * Inputs : GPS UTC
 *
 * Returns : Seconds into the current day. 
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
double UTC2Sec(const double &UTC)
{
    /* NMEA UTC encoding Time of day in UTC
     * Formatted as HHMMSS.sss
     */
    double sec = 0.0;
    double tmp = UTC/10000.0;
    double Hours = floor(tmp);
    tmp          = (tmp - Hours) * 100.0;
    double Min = floor(tmp);
    sec          = (tmp - Min) * 100.0;
    sec += (Min + (Hours *60.0))*60.0;

    return sec;
}

