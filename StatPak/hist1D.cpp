/********************************************************************
 *
 * Module Name : hist1D.cpp
 *
 * Author/Date : C.B. Lirakis / 03-Mar-24
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
 * https://linux.tips/programming/how-to-install-and-use-json-cpp-library-on-ubuntu-linux-os
 * https://subscription.packtpub.com/book/web-development/9781785286902/1/ch01lvl1sec12/reading-and-writing-json-in-c
 *
 * https://github.com/open-source-parsers/jsoncpp
 * https://stackoverflow.com/questions/4289986/jsoncpp-writing-to-files
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring> 
#include <iomanip>
#include <fstream>
#include <limits.h>

#include <jsoncpp/json/json.h>

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
 * Unit Tested on: 09-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Hist1D::Hist1D (const char *name, uint32_t NBins, double min, double max) :
    Average()
{
    fData    = vector<double>(NBins);
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
 * Unit Tested on: 9-Mar-24 
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
 * Unit Tested on: 9-Mar-24 
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

    // Keep track of statistics
    Add(val*weight);

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
 * Unit Tested on: 09-Mar-24 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
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
    SET_DEBUG_STACK;
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
 * Unit Tested on: 09-Mar-24 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Hist1D::WriteJSON(const char *Filename)
{
    SET_DEBUG_STACK;
    bool rv = false;

    ofstream out(Filename);

    if (!out.fail())
    {
	out << "TESTME" << endl;
	out.close();
    }

    SET_DEBUG_STACK;
    return rv;
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
 * Unit Tested on: 09-Mar-24 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, Hist1D &n)
{
    output << "Hist1D - Name: " << n.fName << " ----------------" << endl
	   << (Average &) n
	   << "      Min: " << n.fMin << " Max: " << n.fMax 
	   << " Bin size: " << n.fBinSize
	   << endl
	   << "      Under: " << n.fUnder << " Over: " << n.fOver << endl
           << endl;
    return output;
}
