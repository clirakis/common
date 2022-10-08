/**
 ******************************************************************
 *
 * Module Name : NavDisplay.cpp
 *
 * Author/Date : C.B. Lirakis / 08-Mar-15
 *
 * Description : This is the main display window for all navigation
 *
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>

// local includes
#include "NavUtility.hh"

/**
 ******************************************************************
 *
 * Function Name : FormatDMS
 *
 * Description : format a value (latitude, longitude etc) from base
 * value into a string of degrees minutes and sconds. 
 *
 * Inputs : s - string to fill 
 *          n - size of string that we have
 *          val - value to parse into DMS
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 * Unit Tested  on:  
 * 
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
void FormatDMS(char* s, size_t n, double val, bool space)
{
    if ((s!=NULL) && n>0)
    {
	int sign = 1;
	if (val<0) sign = -1;
	double M = fabs(val);
	int D    = (int)(M);
	M -= ((double)D);
	M *= 60.0;
	double S    = (M - ((int)M))*60.0;

	if (space)
	{
	    snprintf(s, n, "%2d %2d %4.2f", sign*D,(int)M,S);
	}
	else
	{
	    snprintf(s, n, "%2d:%2d:%4.2f", sign*D,(int)M,S);
	}
    }
}
