/********************************************************************
 *
 * Module Name : Average.cpp
 *
 * Author/Date : C.B. Lirakis / 28-June-08
 *
 * Description : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 03-Mar-24 changed over to CPP vector template
 * 09-Mar-24 major rework, no local storage. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.
#include <iostream>
#include <cmath>
#include <string.h>
using namespace std;
#include <cstdint>

// Local Includes.
#include "debug.h"
#include "Average.hh"

/**
 ******************************************************************
 *
 * Function Name : Average Constructor
 *
 * Description : 
 *
 * Inputs : Number of elements to allocate for system. 
 *
 * Returns : constructed class
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 09-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Average::Average(void)
{
    SET_DEBUG_STACK;
    Reset();
}
/**
 ******************************************************************
 *
 * Function Name :Reset
 *
 * Description : Reset the current system.
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 09-Mar-24 
*
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Average::Reset(void)
{
    SET_DEBUG_STACK;
    fN    = 0; 
    fSum  = 0.0;
    fSum2 = 0.0;
}

/**
 ******************************************************************
 *
 * Function Name : Add
 *
 * Description : Add an element
 *
 * Inputs : value to track
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 09-Mar-24  
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Average::Add( double val)
{
    SET_DEBUG_STACK;
    fN++;
    fSum  += val;
    fSum2 += val*val;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Sigma
 *
 * Description : return the sigma of the array.
 *
 * Inputs : NONE
 *
 * Returns : Sigma
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 09-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double Average::Sigma(double *avg)
{
    SET_DEBUG_STACK;
    double rc   = 0.0;
    double den  = ((double) fN);
    double xbar = fSum/den;

    if (avg != NULL)
    {
	*avg = xbar;
    }
    rc = sqrt(fSum2/den - xbar*xbar);

    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Get
 *
 * Description : Return average of array
 *
 * Inputs : NONE
 *
 * Returns : Average \f$ \bar{x} \f$
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 09-Mar-24 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double Average::Get(void)
{
    SET_DEBUG_STACK;
    return (fSum/((double)fN));
}

ostream& operator<<(ostream& output, Average &n)
{
    SET_DEBUG_STACK;
    double avg;
    double sigma = n.Sigma(&avg);
    output << "Average Data ----------------------------" << endl
           << "   Total Entries: "  << n.fN              << endl
	   << "         Average: "  << avg               << endl
	   << "           Sigma: "  << sigma
           << endl;
    return output;
}
