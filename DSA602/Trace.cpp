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
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <ctime>
#include <cfloat> 
#include <unistd.h>

// Local Includes.
#include "debug.h"
#include "DSA602.hh"
#include "Trace.hh"
#include "GParse.hh"

const size_t Trace::kMaxTraces = 8;


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

    fNTrace                  = 0;    // Number of traces detected
    Update(0);
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
    for(size_t i=0;i<fAdjTrace.size();i++) 
    {
	delete fAdjTrace[i];
	fAdjTrace[i]=NULL;
    }
}

/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Update Trace information 
 *
 * Inputs : 
 *    Trace number {1:8}, specifying a 0 means get all traces. 
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
bool Trace::Update(size_t trace)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char   cstring[32], Response[4096];
    DSA602 *pDSA602 = DSA602::GetThis();


    // For the moment just always get everything. 
    Reset();
    fNTrace = GetNTrace();

    memset(cstring, 0, sizeof(cstring));
    sprintf(cstring, "ADJ?");

    memset(Response, 0, sizeof(Response));
    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	if (strlen(Response)>1)
	{
	    //cout << " RESPONSE: " << Response << endl;
	    Decode(Response);
	}
	else
	{
	    SetError(-2,__LINE__);
	    SET_DEBUG_STACK;
	    return false;
	}
    }
    else
    {
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return false;
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
 * Function Name : Decode
 *
 * Description :
 *     Decode the return string for any query.
 *
 * Inputs :
 *     character string from query. Allocated in calling function. 
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
bool Trace::Decode(const char *c)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    const char *search = "ADJTRACE";
    AdjTrace* ptrTrace; 
    string response(c);
    size_t start = 0;
    size_t end   = 0;
    /*
     * A response from ADJ1? HMA looks like:
     *
     * 'ADJTRACE1 HMAG:-1.0E+0'
     *
     * find the trace number, NEED TO TEST WITH MULTIPLE TRACES
     * later - ADD in separate line out per trace response.
     */
    start = response.find(search,0) + sizeof(search);
    end   = start + 1;

    int TraceNumber = atoi(response.substr(start, end).c_str());

    // Does this trace exist? if not make it. 
    size_t tindex = Find(TraceNumber);
    cout << "DECODE trace number. " << TraceNumber 
 	 << "  INDEX " << tindex
 	 << endl;
    if (tindex > kMaxTraces)
    {
	ptrTrace = new AdjTrace();
	ptrTrace->Number(TraceNumber); // Set the trace number. 
	fAdjTrace.push_back(ptrTrace);
    }
    else
    {
	ptrTrace = fAdjTrace[tindex];
    }
    start = end+1; // skip the space. 

    // Now allow the AdjTrace class to fill as needed. 
    return ptrTrace->Decode(response.substr(start));
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
size_t Trace::Find(size_t n)
{
    SET_DEBUG_STACK;
    size_t rv = kMaxTraces+1;   // Not found, maximum of 8 traces
    size_t i = 0;    // counting index. 

    while ((i<fAdjTrace.size()) && (fAdjTrace[i]!=NULL))
    {
	if (fAdjTrace[i]->Number() == n)
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
	   << "  NTrace:   " << n.fNTrace 
	   << endl;
    for (size_t i=0; i<n.fNTrace;i++)
    { 
	output << "    Trace:    " << i << endl << *n.fAdjTrace[i];
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


