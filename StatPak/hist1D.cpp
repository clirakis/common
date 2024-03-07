/********************************************************************
 *
 * Module Name : hist1D.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic hist1D
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
#include <iomanip>

// Local Includes.
#include "debug.h"
#include "hist1D.hh"

/**
 ******************************************************************
 *
 * Function Name : hist1D constructor
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
Hist1D::Hist1D (const char *name, uint32_t NBins, double min, double max) :
    Average(NBins)
{
    fName    = strdup(name);
    fMin     = min;
    fMax     = max;
    fUnder   = fOver = 0;
    fBinSize = (max-min)/((double)NBins);
}

/**
 ******************************************************************
 *
 * Function Name : hist1D destructor
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
Hist1D::~Hist1D (void)
{
    delete fName;
}


/**
 ******************************************************************
 *
 * Function Name : hist1D function
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
void Hist1D::Fill(double val, double weight)
{
    uint32_t bin = 0;
    double   x;

    if (val<fMin)
    {
	fUnder++;
    }
    else if (val>fMax)
    {
	fOver++;
    }
    else
    {
	bin = (uint32_t) floor((val - fMin)/fBinSize);
	x = fData[bin];
	fData[bin] = x + weight;
    }

}
void Hist1D::Print(uint32_t type)
{
    // Find max bin. 
    uint32_t i;
    uint32_t maxbin = 0;
    double   maxval = 0.0;

    // Find where the maximum bin is. 
    for(i=0;i<fData.size();i++)
    {
	if (fData[i]>maxval)
	{
	    maxval = fData[i];
	    maxbin = i;
	}
    }
    cout << "MaxBin: " << maxbin << " value: " << maxval << endl;

    switch(type)
    {
    case 0:
	// Just dump the contents of the array. 
	for (i=0;i<fData.size();i++)
	{
	    if (i%10 == 0)
	    {
		cout << endl << i << ") ";
	    }
	    cout << fData[i] << " " ;
	}
	cout << endl;
	break;
    case 1:
	// vertical histogram
	cout << setfill('*') << setw(maxval) << "" << endl;
	for (i=0;i<fData.size();i++)
	{
	    if (i%10 == 0)
	    {
		cout << i << "+" << setfill(' ') << setw(fData[i]-1) 
		     << "*" << endl;
	    }
	    else if (i%5 == 0)
	    {
		cout << i << "-" << setfill(' ') << setw(fData[i]-1) 
		     << "*" << endl;
	    }
	    else
	    {
		cout << "  -" << setfill(' ') << setw(fData[i]-1) 
		     << "*" << endl;
	    }
	}
	cout << endl;
	break;
    }
}
ostream& operator<<(ostream& output, Hist1D &n)
{
    output << "Hist1D - Name: " << n.fName << " ----------------" << endl
	//super::<< n
	   << "      Min: " << n.fMin << " Max: " << n.fMax 
	   << " Bin size: " << n.fBinSize
	   << endl
	   << "    Under: " << n.fUnder << " Over: " << n.fOver << endl
           << endl;
    return output;
}
