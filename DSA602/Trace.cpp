/********************************************************************
 *
 * Module Name : Trace.cpp
 *
 * Author/Date : C.B. Lirakis / 06-Feb-11
 *
 * Description : Generic Trace
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * DSA602 Programming Manual Page 291
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cstring>

// Local Includes.
#include "debug.h"
#include "DSA602.hh"
#include "Trace.hh"
#include "GParse.hh"

/**
 ******************************************************************
 *
 * Function Name : Trace constructor
 *
 * Description : Class to hold all traces. Could be up to 8 displayed. 
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 07-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Trace::Trace (void) : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetName("Trace");
    /*
     * Store how many traces are available. -1 means uninitalized.
     */
    fNTrace   = -1;
    for (uint8_t i=0;i<kMaxTraces; i++)
    {
	fAdjTrace[i] = NULL;
	fDefTrace[i] = NULL;
    }
    // Update them all. Pass in true to indicate that we have to recreate
    // everything. 
    Update(true);
#ifdef DEBUG_TRACE
    Test();
#endif
}

/**
 ******************************************************************
 *
 * Function Name : Trace destructor
 *
 * Description : Clean up any allocated data/memory
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 07-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Trace::~Trace (void)
{
    SET_DEBUG_STACK;
    Reset();
}
/**
 ******************************************************************
 *
 * Function Name : Reset
 *
 * Description : Loop over the vector of all current traces on the
 *               screen and clean up the pointers. 
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 07-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Trace::Reset(void) 
{
    SET_DEBUG_STACK;
    for(uint8_t i=0;i<kMaxTraces;i++) 
    {
	delete fAdjTrace[i];
	delete fDefTrace[i];
	fAdjTrace[i] = NULL;
	fDefTrace[i] = NULL;
    }
}

/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Update All Trace information 
 *
 * Inputs : NONE
 *    
 * Returns : true on success. 
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
bool Trace::Update(bool init)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (init)
    {
	// Make sure we are starting with a clean slate. 
	Reset();
	/*
	 * How manuy traces do we have in play? 
	 * trace identifiers are {1:8} and do not contain zero. 
	 */
	fNTrace = GetNTrace();
	for (uint8_t i=0;i<fNTrace;i++)
	{
	    fAdjTrace[i] = new AdjTrace();
	    fAdjTrace[i]->Number(i+1); // Set the trace number. 

	    fDefTrace[i] = new DefTrace();
	    fDefTrace[i]->Number(i+1); // Set the trace number.  
	}
    }

    /*
     * Loop over known traces. 
     * Cleaner method. 
     */
    for (uint8_t i=0;i<fNTrace;i++)
    {
	fAdjTrace[i]->Update(); 
	fDefTrace[i]->Update();
    }

    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : GetNTrace
 *
 * Description : Return the number of traces on the screen
 *
 * Inputs : NONE
 *
 * Returns : Numbe of available traces
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 07-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned int Trace::GetNTrace(void)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    char Response[16], value[8];
    unsigned int rv = 0;

    if(pDSA602->Command("TRANUM?", Response, sizeof(Response)))
    {
	memset( value, 0, sizeof(value));
	strcpy( value, &Response[6]);
	rv = atoi(value);
    }
    SET_DEBUG_STACK;
    return rv;
}


/**
 ******************************************************************
 *
 * Function Name : Find
 *
 * Description :
 *     Find trace n in array, return 0 if not found. 
 *
 * Inputs : 
 *    n - the trace number the user wants to find. 
 *
 * Returns :
 *    The index into the vector of all available traces. 
 *
 * Error Conditions :
 *    NONE
 * 
 * Unit Tested on: 05-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t Trace::Find(uint8_t id)
{
    SET_DEBUG_STACK;
    size_t rv = kMaxTraces+1;   // Not found, maximum of 8 traces
    uint8_t i = 0;

    while((i<kMaxTraces) && (fAdjTrace[i]!=NULL))
    {
	if (fAdjTrace[i]->Number() == id)
	{
	    rv = i;
	    break;
	}
	i++;
    } 
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Trace << operator
 *
 * Description : format all the Trace information for output
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 07-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const Trace &n)
{
    SET_DEBUG_STACK;

    output << "    ========================================" << endl
	   << "Trace data: " << endl 
	   << "  NTrace:   " << (int) n.fNTrace 
	   << endl;
    for (int i=0; i<n.fNTrace;i++)
    { 
	output << "    AdjTrace index:    " << i << endl << *n.fAdjTrace[i];
	output << "    DefTrace index:    " << i << endl << *n.fDefTrace[i];
    }

    output << "    ========================================" << endl;
    SET_DEBUG_STACK;
    return output;
}

#ifdef DEBUG_TRACE
void Trace::Test(void)
{
    SET_DEBUG_STACK;
    //cout << *this;
    size_t trace = 0;
    fAdjTrace[trace]->Test();
}
#endif


