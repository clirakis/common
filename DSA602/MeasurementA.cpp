/********************************************************************
 *
 * Module Name : MSLIST.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : MSLIST manages all the possible measurements that
 * can be made for the UI. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Dec-22 CBL Removed CERN root references. Moved into library. 
 * 27-Dec-22 CBL Moved MValue attributes into this file. 
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
#include <string.h>
#include <stdlib.h>


// Local Includes.
#include "debug.h"
#include "MeasurementA.hh"
#include "GParse.hh"
#include "DSA602_Types.hh"

/**
 ******************************************************************
 *
 * Function Name : MeasurementA Constructor
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
MeasurementA::MeasurementA(const char *l)
{
    SET_DEBUG_STACK;
    fMeas      = new string(l);
    fValue     = 0.0;
    fEnabled   = false;
    fQualifier = kMNONE;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : 
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
MeasurementA::~MeasurementA(void)
{
    SET_DEBUG_STACK;
//     cout << __FUNCTION__ << " Delete: " << endl;
//     cout << *fMeas << endl;
    delete fMeas; 
    fMeas = NULL;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : Decode a response into a MValue pair
 *     an example of a response is: "-9.942585E-7,EQ"
 *     The command echo has been stripped out. 
 *
 * Inputs : the result of the MEAS? command
 *
 * Returns : none
 *
 * Error Conditions : Only if the Command doesn't equal the response
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool MeasurementA::Decode(const char *Response)
{
    SET_DEBUG_STACK;
    string resp(Response);
    size_t end;
    if (!Response)
    {
	return false;
    }
    /*
     * By the time we have gotten here we have the correct command
     * checked and the command is stripped it out by finding the 
     * first space delimter. 
     *
     *
     * We are now positioned to just before the value. 
     * Parse until the , delimeter that denotes the qualifier data. 
     */
    end   = resp.find_first_of(',');
    fValue = stof(resp.substr(0, end));
    string qual(resp.substr(end+1));
    if (qual.compare("EQ") == 0)
    {
	fQualifier = kEQUAL;
    }
    else if(qual.compare("LT") == 0)
    {
	fQualifier = kLESS_THAN;
    }
    else if(qual.compare("GT") == 0)
    {
	fQualifier =  kGREATER_THAN;
    }
    else if(qual.compare("UN") == 0)
    {
	fQualifier = kUNKNOWN;
    }
    else if(qual.compare("ER") == 0)
    {
	fQualifier = kERROR;
    }

    // If we were asked to decode, then it is really on. 
    fEnabled = true;

// 	cout << "TO PARSE: " << fValue << " qual " << qual 
// 	     << " qual num: " << (int) fQualifier
// 	     << endl;

    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : 
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
const char *MeasurementA::Text(void) const
{
    SET_DEBUG_STACK;
    if (fMeas)
	return fMeas->c_str();
    SET_DEBUG_STACK;
    return NULL;
}
/**
 ******************************************************************
 *
 * Function Name : Match
 *
 * Description : Does the input string name match the sring stored? 
 *
 * Inputs : name - name to compare to. (May want to make more leinient.)
 *
 * Returns : true on success. 
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
bool MeasurementA::Match(const char *name)
{
    SET_DEBUG_STACK;
    if(fMeas->compare(name) == 0)
    {
	return true;
    }
    SET_DEBUG_STACK;
    return false;
}

/**
 ******************************************************************
 *
 * Function Name : 
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
ostream& operator<<(ostream& output, const MeasurementA &n)
{
    SET_DEBUG_STACK;
    output << *n.fMeas << " " 
	   << n.fValue << " ";
    if(n.State())
	output << " Check";
    output << " ,";
    switch(n.fQualifier)
    {
    case kEQUAL:
	output << " Equal.";
	break;
    case kLESS_THAN:
	output << " Less Than.";
	break;
    case kGREATER_THAN:
	output << " Greater Than.";
	break;
    case kUNKNOWN:
	output << " Unknown.";
	break;
    case kERROR:
	output << " Error.";
	break;
    case kMNONE:
	output << " NOPARSE.";
	break;
    default:
	break;
    }

    SET_DEBUG_STACK;
    return output;
}

#if 0
/**
 ******************************************************************
 *
 * Function Name : 
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
void MSLIST::FillState(const char *s)
{
    SET_DEBUG_STACK;
    const char tokens[] = "[,]";
    MeasurementA *m;
    TString     input(s);
    Int_t       n = input.First(" ");

    if (n>0)
    {
	/* Skip command */
	TString Remainder = input(n+1, input.Length()-n-1);
	TStringToken p(Remainder, tokens);
	while(p.NextToken())
	{
	    if((m = Find(p.Data()))!=NULL)
	    {
		//cout << "Found: " << p.Data() << endl;
		m->SetState(kTRUE);
	    }
	}
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : OBSOLETE!
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
void MSLIST::FillValue(const char *s)
{
    SET_DEBUG_STACK;
    // GPARSE will not work here
    // There is a second value that is returned
    // EQ True measurement equals value
    // LT less than
    // GT
    // UN uncertain
    // ER Error occured. 
    // Example data: 
    // MEAS FREQ:0.0E+0,ER,RISETIME:0.0E+0,EQ,PP:5.85937E+1,EQ
    GParse        gp(s);
    TListIter     next(fMeasurements);
    MeasurementA* p;

    /*
     * Loop over the entire list.
     * If the state is true then fill the value with the parsed string. 
     */
    while ((p = (MeasurementA *)next()))
    {
	if (p->State())
	{
	    p->SetValue(atof(gp.Value(p->Text())));
	}
    }
    SET_DEBUG_STACK;
}
#endif
