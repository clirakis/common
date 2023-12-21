/********************************************************************
 *
 * Module Name : Filt.cpp
 *
 * Author/Date : C.B. Lirakis / 22-Jan-23
 *
 * Description : Manage coefficients for filters, base class
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
#include <string>
#include <cmath>
#include <fstream>
#include <iomanip>
using namespace std;

// Local Includes.
#include "debug.h"
#include "Filt.hh"

/**
 ******************************************************************
 *
 * Function Name : Filt constructor
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
Filt::Filt (void)
{
}

/**
 ******************************************************************
 *
 * Function Name : Filt constructor
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
Filt::Filt (const Filt &p)
{
    fType        = p.fType;
    fDenomCoeffs = p.fDenomCoeffs;
    fNumCoeffs   = p.fNumCoeffs;
}

/**
 ******************************************************************
 *
 * Function Name : Filt destructor
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
Filt::~Filt (void)
{
}


/**
 ******************************************************************
 *
 * Function Name : ClearAll
 *
 * Description : Reset all the vectors in prep for a recalculation 
 *
 * Inputs : NONE
 *
 * Returns : NONE
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
void Filt::ClearAll(void)
{
    SET_DEBUG_STACK;
    fDenomCoeffs.clear();
    fNumCoeffs.clear();
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : ReadCSVFile
 *
 * Description : read all the filter data from a file produced 
 *               elsewhere. Most likely python.
 *
 * Inputs : Filename to parse
 *
 * Returns : NONE
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
bool Filt::ReadCSVFile(const char *Filename)
{
    SET_DEBUG_STACK;
    bool     rv = false;
    string   delimiter = ",";
    string   token;
    size_t   pos = 0;
    uint8_t  index;
    uint8_t  linecount = 0;

    ifstream mydata(Filename);
    string   line;

    if (!mydata.fail())
    {
	while(!mydata.eof())
	{
	    mydata >> line;
	    //cout << line << endl;
	    // Skip the first line
	    if(linecount>0)
	    {
		index = 0;
		/*
		 * The original code of just looping while a delimeter
		 * is found is flawed. There is always left at 
		 * the end of the line. 
		 */
		while(line.size() > 0) 
		{
		    pos = line.find(delimiter);
		    if (pos != string::npos) 
		    {
			token = line.substr(0, pos);
		    }
		    else
		    {
			// Get the bit after the last delimiter
			pos   = line.size() - token.length(); 
			token = line;
		    }
		    //cout << "Index: " << (int) index << " Token: " << token << endl;
		    switch(index)
		    {
		    case 0:
			// row number, skip
			break;
		    case 1:
			fDenomCoeffs.push_back(stod(token));
			break;
		    case 2:
			fNumCoeffs.push_back(stod(token));
			break;
#if 0
		    case 3:
			// cutoff in Fc/F
			cout << "FOUR: " << token << endl;
			fLowerCutoff = stod(token);
			break;
		    case 4:
			// cutoff in Fc/F
			fUpperCutoff = stod(token);
			break;
#endif
		    }
		    line.erase(0, pos + delimiter.length());
		    index++;
		}
	    }
	    linecount++;
	}
	rv = true;
    }

    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Butterworth << operator
 *
 * Description : format all the channel information for output
 *
 * Inputs : 
 *       output stream to format 
 *       Butterworth class to print out
 *
 * Returns : fully functional stream
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
ostream& operator<<(ostream& output, const Filt &n)
{
    SET_DEBUG_STACK;
    uint16_t i;
    output << std::fixed << std::setw(6) << std::setprecision(6);

    output << "(a) Denominator (" << n.fDenomCoeffs.size() << "): " 
	   << endl << "     ";
    for (i=0;i<n.fDenomCoeffs.size(); i++)
    {
	output << n.fDenomCoeffs[i] << ", ";
	if ((i>0) && (i%5 == 0)) output << endl << "     ";
    }
    output << endl;

    output << "(b) Numerator (" << n.fNumCoeffs.size() << "): " 
	   << endl << "     ";

    for (i=0;i<n.fNumCoeffs.size(); i++)
    {
	output << n.fNumCoeffs[i] << ", ";
	if ((i>0) && (i%5 == 0)) output << endl << "     ";
    }
    output << endl;

    SET_DEBUG_STACK;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name : Filt function
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
#if 0
void* Filt::function(const char *Name)
{
}
#endif
