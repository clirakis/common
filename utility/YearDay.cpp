/********************************************************************
 *
 * Module Name : YearDay.cpp
 *
 * Author/Date : C.B. Lirakis / 13-Feb-24
 *
 * Description : convert Month day year to day in year
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
#include "YearDay.hh"

/**
 ******************************************************************
 *
 * Function Name : YearDay
 *
 * Description : provide seconds into day from GPS UTC
 *
 * Inputs : 
 *     Year - 2024 is 2024. 
 *     Month - 0:11
 *     Day   - 1:31
 *
 * Returns : Day into year. 
 *
 * Error Conditions :
 * 
 * Unit Tested on: 13-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint32_t YearDay(uint32_t Year, uint8_t Month, uint8_t Day)
{
    static int dmo[12] = {0,31,59,90,120,151,181,212,243,273,304,334};

    uint32_t rv = dmo[Month] + Day;

    if ((Month>1) && (Year%2 == 0))
    {
	rv++;
    }
    return rv;
}

