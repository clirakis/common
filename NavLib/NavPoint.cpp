/********************************************************************
 *
 * Module Name : NavPoint.cpp
 *
 * Author/Date : C.B. Lirakis / 26-Nov-15
 *
 * Description : Navigation point, major rewrite using TVector3. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 09-Oct-21   CBL    New version of root and upgrade of gcc, using string_view
 * 25-Mar-22   CBL    add in bit control field for viewing. 
 * 27-Mar-22   CBL    will store track data. 
 * 28-Mar-22   CBL    added in an optional time field. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.
#include <iostream>
#include <ostream>
using namespace std;

// Root includes
#include <TObject.h>
#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>

// Local Includes.
#include "debug.h"
#include "NavPoint.hh"

unsigned int NavPoint::fCount = 0;

/**
 ******************************************************************
 *
 * Function Name : NavPoint constructor
 *
 * Description : This one creates a new point used in navigation
 *               but using all the desired inputs. 
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
NavPoint::NavPoint (Double_t X, Double_t Y, Double_t Z, Int_t Type,
		    Double_t dt, const char *n) : TVector3 (X,Y,Z)
{
    SET_DEBUG_STACK;
    fType = Type;
    if (n)
    {
	fName = new TString(n);
    }
    else
    {
	fName = NULL;
    }
    fTime = NULL;
    fdt = dt;
    SetUniqueID(fCount);
    fCount++;
    fStatusBits = 0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : NavPoint cstructor
 *
 * Description : Used when a formatted string is provided. 
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
NavPoint::NavPoint(const char *s) : TVector3()
{
    SET_DEBUG_STACK;
    fType = kNONE;
    fStatusBits = 0;
    fName = NULL;
    fTime = NULL;
    fdt   = 0.0;
    SetData(s);
}

/**
 ******************************************************************
 *
 * Function Name : SetName
 *
 * Description : Set the name of the point if there is one
 *
 * Inputs : n - desired string to be associated with the point
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NavPoint::SetName(const char *n)
{
    SET_DEBUG_STACK;
    TString temp(n);
    if (fName==NULL)
    {
	fName = new TString(temp.Strip(TString::kBoth));
    }
    else
    {
	*fName = temp.Strip(TString::kBoth);
    }
    SET_DEBUG_STACK;
}

void NavPoint::SetTime(UInt_t seconds, UInt_t milliseconds)
{
    SET_DEBUG_STACK;
    if (fTime)
    {
	delete fTime;
	fTime = NULL;
    }

    fTime = new Double_t(seconds);
    *fTime += (Double_t)milliseconds * 1.0e-3;
}


/**
 ******************************************************************
 *
 * Function Name : SetData
 *
 * Description : Utility function to add the data into the structure
 *               when read from a file. 
 *
 * Inputs : n - character string containing user data. 
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NavPoint::SetData(const char *n)
{
    SET_DEBUG_STACK;
    if ((n!=NULL) || (strlen(n)>2))
    {
	TString     input(n), mine;
	TObjArray*  p = input.Tokenize(NAV_SEPARATOR);
	Int_t       N = p->GetEntriesFast();
	Int_t       index = 0;
	TObjString* obj;
	Double_t    X;

	fType   = -1;
	fName   = NULL;
	SetXYZ(0.0, 0.0, 0.0);
	fdt = 0.0;

#if DEBUG
	cout << "Parsing string: " << n << endl;
#endif
	X = 0.0;

	// determine type
	obj = (TObjString *)(*p)[index]; index++;
	mine = obj->GetString();

	if (mine.Contains("wpt"))
	{
	    fType = kWAYPOINT;
	}
	else if (mine.Contains("evt"))
	{
	    fType = kEVENT;
	}
	else if(mine.Contains("trk"))
	{
	    fType = kTRACK;
	}
	for (index=1;index<N;index++)
	{
	    obj = (TObjString *)(*p)[index];
	    if (obj != NULL)
	    {
		mine = obj->GetString();
		if (mine.BeginsWith("X=", TString::kIgnoreCase))
		{
		    X = mine.Remove(0,2).Atof();
		    SetX(X);
		}
		else if (mine.BeginsWith("Y=", TString::kIgnoreCase))
		{
		    X = mine.Remove(0,2).Atof();
		    SetY(X);
		}
		else if (mine.BeginsWith("Z=", TString::kIgnoreCase))
		{
		    X = mine.Remove(0,2).Atof();
		    SetZ(X);
		}
		else if (mine.BeginsWith("dt=", TString::kIgnoreCase))
		{
		    fdt = mine.Remove(0,2).Atof();
		}
		else if (mine.BeginsWith("Title=", TString::kIgnoreCase))
		{
		    //cout << "Mine: " << mine << endl;
		    if (fName==NULL)
		    {
			delete fName;
		    }
		    fName = new TString(mine.Remove(0,6));
		}
		else if (mine.BeginsWith("Time=", TString::kIgnoreCase))
		{
		    if (fTime)
		    {
			delete fTime;
		    }
		    fTime = new Double_t(mine.Remove(0,5).Atof());
		}
		else if (mine.BeginsWith("BITS=", TString::kIgnoreCase))
		{
		    // Bits are input as hex, need to format.
		    fStatusBits = strtol(mine.Remove(0,5).Data(), NULL,16);
		}
	    }
	}
	SetUniqueID(fCount);
	fCount++;
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : NavPoint destructor
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
NavPoint::~NavPoint(void)
{
    SET_DEBUG_STACK;
    delete fName;
    fName = NULL;
    delete fTime;
    fTime = NULL;
}
/**
 ******************************************************************
 *
 * Function Name : SameName
 *
 * Description : Return true if the name associated with the point are the 
 *               same. Used in searches
 *
 * Inputs : name to compare to
 *
 * Returns : true on match
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Bool_t NavPoint::SameName(const char *name)
{
    SET_DEBUG_STACK;
    Bool_t rc =kFALSE;
    if (fName!=NULL)
    {
	if (strcmp(name, fName->Data()) == 0)
	{
	    rc = kTRUE;
	}
    }
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : SetBit
 *
 * Description : Set or clear the bit at the desired location. 
 *
 * Inputs : 
 *         bit(s) to set see NavPointStatus enum
 *         val - TRUE - set, FALSE CLEAR
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
void NavPoint::SetBit(UInt_t bit, Bool_t val)
{
    SET_DEBUG_STACK;
    if (val)
    {
	// True, set the bit. 
	fStatusBits |= bit;
    }
    else
    {
	UInt_t mask = ~bit;
	fStatusBits &= mask;	
    }
}


/**
 ******************************************************************
 *
 * Function Name : Operator << 
 *
 * Description : overload to output data in file or screen stream
 *
 * Inputs : 
 *      output - ostream to format into
 *      in - NavPoint structure to format
 *
 * Returns : formatted ostream
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: NONE
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<( ostream& output, const NavPoint &in)
{
    char *prefix;
    SET_DEBUG_STACK;
    
    // It is 1852 m per arc minute of latitude. 
    // This allows us to calculate our digit precision
    // so 1e-8 in latitude is 1.11mm 3 digits for degree and 8 for 
    // radix, 11 total. 
    switch(in.fType)
    {
    case NavPoint::kWAYPOINT:
	prefix = (char *) "wpt";
	break;
    case NavPoint::kEVENT:
	prefix = (char *) "evt";
	break;
    case NavPoint::kTRACK:
	prefix = (char *) "trk";
	break;
    }
    output.setf(ios::floatfield,ios::fixed);
    output.precision(8);
    output << prefix << NAV_SEPARATOR << "X=" << in.X() 
	   << NAV_SEPARATOR << "Y=" << in.Y();
    if ((in.fType == NavPoint::kEVENT) || (in.fType == NavPoint::kTRACK))
    {
	output.precision(2);
	output << NAV_SEPARATOR << "Z="
	       << in.Z()
	       << NAV_SEPARATOR << "dt=" 
	       << in.GetDt();
    }
    if (in.GetName())
    {
	output << NAV_SEPARATOR << "Title=" << in.GetName();
    }
    else
    {
	output << NAV_SEPARATOR << "Title=Event " << in.GetUniqueID(); 
    }
    if(in.fTime)
    {
	output << NAV_SEPARATOR << "Time=" << *in.fTime;
    }
    output << NAV_SEPARATOR << "BITS=";
    output.fill('0');
    output.width(8);
    output.setf(ios::hex); 
    output << in.fStatusBits;

    SET_DEBUG_STACK;
    return output;
}
