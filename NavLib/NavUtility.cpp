/**
 ******************************************************************
 *
 * Module Name : NavDisplay.cpp
 *
 * Author/Date : C.B. Lirakis / 08-Mar-15
 *
 * Description : This is the main display window for all navigation
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 22-Mar-22 CBL rather full rewrite.
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
 * Description : Format a degree decimal value in to Degrees, minutes
 * seconds. 
 *
 * Inputs :
 *           s - string to put the data into, provided by parent call
 *           n - size of above string
 *           val - decimal degree value. 
 *
 * Returns : result in s
 *
 * Error Conditions : NONE
 *
 * Unit Tested  on: 25-Mar-22
 * 
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
void FormatDMS(char* s, size_t n, double val)
{
    /*
     * Make sure that the string size is non-zero and
     * the pointer is non-null
     */
    if ((s!=NULL) && n>0)
    {
	/* 
	 * Get the degree value.
	 */
	double   sign, Degrees, Minutes, Seconds, fval;
	if(val<0.0) 
	    sign = -1.0;
	else 
	    sign =  1.0;

	fval = fabs(val);

	// Get the degree value. 
	fval  = modf( fval, &Degrees);
	Degrees *= sign;

	fval    *= 60.0;
	Seconds  = modf(fval, &Minutes);
	Seconds *= 60.0;

	snprintf(s, n, "%2d:%2d:%4.2f", (int) Degrees, (int)Minutes,
		 Seconds);
    }
}
