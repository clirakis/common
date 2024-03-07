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
 * Unit Tested on: 28-June-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Average::Average(size_t Nele)
{
    SET_DEBUG_STACK;
    cout << "AVERAGE " << Nele << endl;
    fRejectCount    = 0;
    // allocate vector to full size if Nele>0
    fData = vector<double>(Nele, 0.0);
    fCurrentPointer = 0;
    fFull           = false;       // the vector array has not been filled. 
}
/**
 ******************************************************************
 *
 * Function Name : AddElement
 *
 * Description : Add value, but not without first doing some
 * flier rejection. It has to be within 3 sigma or rejected more
 * than 5 times, or within the FirstFill (First NElements)
 *
 * Inputs : value to put in array
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  28-June-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Average::AddElement( double val)
{
    SET_DEBUG_STACK;
    if (fCurrentPointer>5)
    {
        // Protect against temporary fliers.
        double sig  = Sigma();
        double avg  = Get();
        double diff = fabs(val)-fabs(avg);
        if ((fabs(diff) < 3.0*sig) || (fRejectCount > 5))
        {
            Add(val);
            fRejectCount = 0;
        }
        else
        {
            fRejectCount++;
        }
    }
    else
    {
        Add(val);
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Add
 *
 * Description : Add raw element into array
 *
 * Inputs : value to put in array
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  06-Aug-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Average::Add( double val)
{
    SET_DEBUG_STACK;
    //
    // roll the pointer as necessary, kind of a windowed low pass filter
    // without weight. 
    //
    fData[fCurrentPointer] = val;
    fCurrentPointer        = (fCurrentPointer+1)%fData.size();

    fFull = (fCurrentPointer == 0); // its ok if we set it multiple times. 

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GetSigma
 *
 * Description : return the sigma of the array.
 *
 * Inputs : NONE
 *
 * Returns : Sigma
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 28-June-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double Average::Sigma(double *avg)
{
    size_t i, N;
    double sum  = 0.0;
    double sum2 = 0.0;
    double rc   = 0.0;
    double x    = 0.0;


    if (fFull)
    {
	N = fData.size();
    }
    else
    {
	N = fCurrentPointer;
    }
    for(i=0;i<N;i++)
    {
	x     = fData[i];
	sum  += x;
	sum2 += x*x;
    }
    if (avg != NULL)
    {
	*avg = sum/((double) N);
    }
    rc = sqrt(fabs(pow(sum,2.0) - sum2))/((double) N);

    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Get
 *
 * Description : Return average of array
 *               Conditions: 
 *               If number of entries > size full array is used.
 *               Else only entries are used
 *               If no entries have been made return 0
 *
 * Inputs : NONE
 *
 * Returns : Avergage
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 28-June-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double Average::Get(void)
{
    double   rc  = 9999.99;
    uint32_t N;

    if (fFull)
    {
	N = fData.size();
    }
    else
    {
	N = fCurrentPointer;
    }
    if (N>2)
    {
        double sum = 0.0;
        for (size_t i=0;i<N;i++)
        {
            sum += fData[i];
        }
        rc = sum/((double)N);
    }
    return rc;
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
 * Unit Tested on: 28-June-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Average::Reset(void)
{
    fCurrentPointer = 0;
    fFull = false;
    for (size_t i=0;i<fData.size();i++)
    {
	fData[i] = 0.0;
    }
}
ostream& operator<<(ostream& output, Average &n)
{
    double avg;
    double sigma = n.Sigma(&avg);
    output << "Average Data ----------------------------" << endl
           << " Current Pointer: "  << n.fCurrentPointer << endl
           << "    Reject Count: "  << n.fRejectCount    << endl
           << "   Total Entries: "  << n.fData.size()    << endl
	   << "         Average: "  << avg               << endl
	   << "           Sigma: "  << sigma
           << endl;
    return output;
}
