/********************************************************************
 *
 * Module Name : SFilter.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic SFilter
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * https://stackoverflow.com/questions/27106205/low-pass-filter-in-c
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>

// Local Includes.
#include "debug.h"
#include "SFilter.hh"

/**
 ******************************************************************
 *
 * Function Name : SFilter constructor
 *
 * Description :
 *
 * Inputs :
 *    CutoffFrequency in Hz
 *    SampleRate in Hz
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
SFilter::SFilter (double CutoffFrequency, double SampleRate)
{
    SET_DEBUG_STACK;
    fSampleRate = SampleRate;
    fCutoff     = CutoffFrequency;
    ChangeConstant(CutoffFrequency);
    Reset();
}

/**
 ******************************************************************
 *
 * Function Name : SFilter destructor
 *
 * Description : NOT USED AT THIS TIME
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
void SFilter::ChangeConstant(double CutoffFrequency)
{
    SET_DEBUG_STACK;
    double RC = 1.0/(2.0*M_PI*CutoffFrequency);
    double dt = 1.0/fSampleRate;
    fBeta     = dt/(RC+dt);
}

/**
 ******************************************************************
 *
 * Function Name : SFilter function
 *
 * Description : Filter an entire buffer of data. 
 *
 * Inputs :
 *    in  - input buffer to filter
 *    out - resulting filtered data
 *    N   - number of samples in both in and out buffers.     
 *
 * Returns :
 *    NONE
 * Error Conditions :
 *    NONE
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SFilter::Filter(const double *in, double *out, uint32_t N)
{
    SET_DEBUG_STACK;
    /* Works for a full buffer. */
    out[0] = fBeta * in[0] ;
    
    for (uint32_t i=1;i<N;i++)
    {
	out[i] =  fBeta * in[i]  + (1.0-fBeta)*out[i-1];
    }
    fPrev = 0.0;
}
/**
 ******************************************************************
 *
 * Function Name : Filter
 *
 * Description : Kind of a running filter
 *
 * Inputs : input sample
 *
 * Returns : resulting filtered value
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
double SFilter::Filter(const double &in)
{
    SET_DEBUG_STACK;
    fPrev = fBeta * in  + (1.0-fBeta)*fPrev;
    return fPrev;
}
