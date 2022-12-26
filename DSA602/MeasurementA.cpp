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
MeasurementA::MeasurementA(const char *l)
{
    SET_DEBUG_STACK;
    fMeas    = new string(l);
    fValue   = 0.0;
    fEnabled = false;
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
MeasurementA::~MeasurementA()
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
    output << n.fMeas << " " 
	   << n.fValue << " ";
    if(n.State())
	output << " Check";
    output << " ,";
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
