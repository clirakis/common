/********************************************************************
 *
 * Module Name : DefTrace.cpp
 *
 * Author/Date : C.B. Lirakis / 29-Dec-22
 *
 * Description : Define Trace parameters
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
#include <string>

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "DSA602.hh"
#include "DefTrace.hh"

/* Command, type, upper bound, lower bound */
const struct t_Commands DefTrace::Commands[kEND_LIST]= {
    {"ACC",        kCT_FLOAT, 32768.0,   512.0}, 
    {"ACS",        kCT_FLOAT,   100.0,50.0e-12},
    {"DES",        kCT_FLOAT,     0.0,     0.0},
    {"GRL",        kCT_FLOAT,     0.0,     0.0},
    {"GRT",        kCT_FLOAT,     0.0,     0.0},
    {"WFM",        kCT_FLOAT,     0.0,     0.0},
    {"XUN",        kCT_BOOL,      0.0,     0.0},
    {"YUN",        kCT_FLOAT,     0.0,     0.0},
};

/**
 ******************************************************************
 *
 * Function Name : DefTrace constructor
 *
 * Description : Right now just clears the variables. 
 * 
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 30-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
DefTrace::DefTrace(void) : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetName("DefTrace");
    Clear();
}
DefTrace::~DefTrace(void)
{
    SET_DEBUG_STACK;
    delete fDescription;
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : Reset all the DefTrace values to zero or false. 
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
void DefTrace::Clear(void)
{
    SET_DEBUG_STACK;
    fACCumulate  = kACC_OFF;
    fACState     = false;
    fDescription = new string("NONE");
    fGRLocation  = false; // lower
    fWFMCalc     = false;
    fXUNit       = kAMPS;
    fYUNit       = kAMPS;
}
/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Update Trace information, this is part of a collection
 *
 * Inputs : none
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
bool DefTrace::Update(void)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    char     s[32];
    char     Response[256];

    // Command the mainframe to tell me everything about this trace. 
    sprintf(s, "TRA%d?", fTraceNumber+1); 
    memset(Response, 0, sizeof(Response));
    if (pDSA602->Command(s, Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# DefTrace::Update Command: %s, Response: %s\n", 
		     s, Response);
	}
	Decode(Response);
	return true;
    }
    SET_DEBUG_STACK;
    return false;
}

/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : given a string associated with a query about
 *               an DefTrace call, parse and fill the correct field. 
 *
 * Inputs :
 *     string containing something like:  
 *     see page 292
 *
 * Returns :
 *     true on success
 *
 * Error Conditions :
 *     NONE
 * 
 * Unit Tested on: 30-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DefTrace::Decode(const string &response) 
{ 
    SET_DEBUG_STACK;
    /*
     * When we get here this should be stripped to just the tags
     * and values. 
     */
    size_t start = 0;
    size_t end   = 0;
    /*
     * A response from ADJ1? HMA looks like:
     *
     * 'TRACE1 DESCRIPTION:<qstring>, ACCUMLATE:<arg>...
     * ^          
     * Imagine we start here
     */

    string cmd;
    string val;

    do {

	end   = response.find(':',start);   // find the command delimeter
	cmd   = response.substr(start, end-start);
	start = end+1;
	end   = response.find(',',start);
	val   = response.substr(start, end-start);
	start = end + 1;
#if 0
	cout << " DefTrace::Decode Response: " << response
	     << " CMD: " << cmd 
	     << " VAL: " << val << endl;
#endif
	if (cmd.find("ACC") < string::npos)
	{
	    if (val.find("INFP") != string::npos)
	    {
		fACCumulate = kINFPERSIST;
	    }
	    else if (val.find("OFF") != string::npos)
	    {
		fACCumulate = kACC_OFF;
	    }
	    else if (val.find("VAR") != string::npos)
	    {
		fACCumulate = kVARPERSIST;
	    }
	    else 
	    {
		fACCumulate = kACC_OFF;
	    }
	}
	else if (cmd.find("ACS") < string::npos)
	{
	    if (val.find("ENH") != string::npos)
	    {
		fACState = true;
	    }
	    else 
	    {
		fACState = false;
	    }
	}
	else if (cmd.find("DES") < string::npos)
	{
	    delete fDescription;
	    fDescription = new string(val);
	}
	else if (cmd.find("GRL") < string::npos)
	{
	    if (val.find("UPP") != string::npos)
	    {
		fGRLocation = true;
	    }
	    else if (val.find("LOW") != string::npos)
	    {
		fGRLocation = false;
	    }
	    else
	    {
		fGRLocation = false;
	    }
	}
	else if (cmd.find("GRT") < string::npos)
	{
	    // For the moment ignore
	}
	else if (cmd.find("WFMC") < string::npos)
	{
	    if (val.find("HIP") != string::npos)
	    {
		fWFMCalc = true;
	    }
	    else if (val.find("FAS") != string::npos)
	    {
		fWFMCalc = false;
	    }
	    else
	    {
		fWFMCalc = false;
	    }	    
	}
	else if (cmd.find("XUN") < string::npos)
	{
	    fXUNit = DecodeUnits(val.c_str());
	}
	else if (cmd.find("YUN") < string::npos)
	{
	    fYUNit = DecodeUnits(val.c_str());
	}
    } while (end<response.size());

    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Query
 *
 * Description : Determine what the value is for the associated input 
 *               command. This assumes that fNTrace has already been 
 *               populated. (MAYBE a bad construct.)
 *
 * Inputs : 
 *    the enum with the command type. Fills the appropriate value
 *    in the class that the user must subsequently interrogate
 *    through the convienence functions. 
 *
 * Returns :
 *    true on success. 
 *
 * Error Conditions :
 *     GPIB fail. 
 * 
 * Unit Tested on: 30-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DefTrace::Query(COMMANDs c)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    char   cstring[32], Response[4096];
    ClearError(__LINE__);

    memset(cstring, 0, sizeof(cstring));

    if (c<kEND_LIST)
    {
	sprintf( cstring, "TRA%d? %s", fTraceNumber, Commands[c].Command);
    }
    else
    {
	// out of band;
	return false;
    }

    memset(Response, 0, sizeof(Response));
    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# DefTrace::Query: %s\n", Response);
	}
	if (strlen(Response)>1)
	{
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
 * Function Name : GRLocation
 *
 * Description : 
 *     Set the Graticule location.
 *
 * Inputs : 
 *    value - true (upper), false (lower)
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    GPIB command failed. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DefTrace::SetGRLocation(bool value)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    ClearError(__LINE__);

    memset(cstring, 0, sizeof(cstring));
    if (value)
    {
	sprintf( cstring, "ADJ%d GRL:UPP",fTraceNumber);
    }
    else
    if (value)
    {
	sprintf( cstring, "ADJ%d GRL:LOW",fTraceNumber);
    }
    if(log->CheckVerbose(1))
    {
	log->Log("# DefTrace::GRL: %s\n", cstring);
    }
    rc = pDSA602->Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : Description
 *
 * Description : (This is the real meat!
 *       Set the waveform description FIXME - needs more work
 *       limited to 120 characters total. format is like:
 *       TRAce<ui><sp>Descrption<qstring>
 *
 *       qstring can be something like:
 *          <y exp> [VS <x exp>] ON <time base> 
 *       where:
 *          <y exp>, <x exp> ::= Expressions
 *          [VS <x exp>]     ::= Indicates a XY waveform, else YT 
 *          [ON <time base>] ::= indicates time base {MAIN|WIN1|WIN2}
 *                               when omitted, defaults to MAIN. 
 *
 *      Available inputs
 *         <slot><ui> (e.g. CHL1)
 *         STO<ui> - stored waveform (1:420)
 *         <NRx>   - scalar number
 *         <function> Any of the following:
 *             ABS|AVG|CONVOLUTION|CORRELATION|
 *             DEJITTER|DELAY|DIFF|ENV|EXP|FFTIMAG|
 *             FFTMAG|FFTPHASE|FFTREAL|FILTER|IFFT|
 *             INTP|LN|LOG|PIADD|PISUB|PULSE|SIGNUM|
 *             SMOOTH|SQRT
 *             
 *          <operators> {+,-,*,/}   
 *             
 * Inputs : 
 *    string to set
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    the GPIB command
 *    failed
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DefTrace::Description(const char *Description)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char s[256];

    ClearError(__LINE__);

    memset(s, 0, sizeof(s));
    sprintf( s, "TRA%d DES:%s",fTraceNumber, Description);

    rc = pDSA602->Command(s, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DefTrace::DESCRIPTION: %s %d\n", s, rc);
    }
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : DefTrace << operator
 *
 * Description : format all the Trace information for output
 *
 * Inputs :
 *     ouptut - the stream to fill with the formatted data. 
 *     n = the DefTrace class to pull the data from. 
 *
 * Returns :
 *     fully formatted stream output
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 30-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const DefTrace &n)
{
    SET_DEBUG_STACK;
    

    output << "============================================" << endl
	   << "DefTrace data: " << endl 
	   << "   Trace Number: " << n.fTraceNumber << endl;
    switch(n.fACCumulate)
    {
    case kINFPERSIST:
	output << "       Accumulate:Infinite Persistance " << endl;
	break;
    case kACC_OFF:
	output << "       Accumulate:OFF" << endl;
	break;
    case kVARPERSIST:
	output << "       Accumulate:Variable Persistance " << endl;
	break;
    }
    if(n.fACState)
    {
	output << "       ACState: Enhanced" << endl;
    }
    else
    {
	output << "       ACState: Not Enhanced" << endl;
    }

    output << "       Description: " << n.fDescription << endl;
    if (n.fGRLocation)
    {
	output << "       GR Locaton: Upper " << endl;
    }
    else
    {
	output << "       GR Locaton: Lower " << endl;
    }
    if (n.fWFMCalc)
    {
	output << "       WFM Calculation: (slow) High Precision (FP)" << endl;
    }
    else
    {
	output << "       WFM Calculation: (fast) Integer " << endl;
    }
    output << "        XUnit:" << UnitString(n.fXUNit) << endl;
    output << "        YUnit:" << UnitString(n.fYUNit) << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}
#ifdef DEBUG_TRACE
void DefTrace::Test(void)
{
    SET_DEBUG_STACK;
    // Query only, test ok 13-Feb-21
//     cout << "FR: " << FrequencyResolution(true) << " Hz" <<  endl;
//     cout << "FS: " << FrequencySpan( true) << " Hz" <<  endl;

    cout << "PZ: " << PanZoom(true) << endl;
    cout << "PZ ON: " << SetPanZoom(true) << endl;
    cout << "PZ: " << PanZoom(true) << endl;

#if 0
    cout << "HM: " << HorizontalMagnification(true) << endl;
    cout << "SET HM 2: " << HorizontalMagnification(DefTrace::kHM_2) << endl;
    cout << "HM: " << HorizontalMagnification(true) << endl;
    cout << "SET HM 1: " << HorizontalMagnification(DefTrace::kHM_1) << endl;
    cout << "HM: " << HorizontalMagnification(true) << endl;
#endif
#if 0
     cout << "HP: " << HorizontalPosition( true) << endl;
     cout << "SET HP 2: " << HorizontalPosition((size_t) 2) << endl;
     cout << "HP: " << HorizontalPosition( true) << endl;
     cout << "SET HP 0: " << HorizontalPosition( (size_t)0) << endl;
     cout << "HP: " << HorizontalPosition( true) << endl;
#endif
#if 0
     // Haven't really tested with XY waveform. 
     cout << "HVP: " << HVPosition( true) << endl;
     cout << "Set HVP: -8.9e-6 " << HVPosition(-8.9e-6) << endl;
     cout << "HVP: " << HVPosition( true) << endl;
     cout << "HVP: 1.0e16 " << HVPosition( 1.0e16) << endl;
     cout << "HVP: " << HVPosition( true) << endl;
#endif
#if 1
     cout << "HVS: " << HVSize( true) << endl;
     cout << "Set HVS: 1.0" << HVSize( 4.5e-2) << endl;
     cout << "HVS: " << HVSize( true) << endl;
     cout << "Set HVS: -1.0" << HVSize( -1.0) << endl;
     cout << "HVS: " << HVSize( true) << endl;
#endif
#if 0
     cout << "TS: " << TraceSeparation(true) << endl;
     cout << "TS: " << TraceSeparation(true) << endl;
     cout << "TS: " << TraceSeparation(true) << endl;
     cout << "TS: " << TraceSeparation(true) << endl;
#endif
#if 0
     cout << "VP: " << VerticalPosition(true) << endl;
     cout << "SVP: 1.0 " << VerticalPosition(3.9e2) << endl;
     cout << "VP: " << VerticalPosition(true) << endl;
     cout << "VP: " << VerticalPosition(1.0e16) << endl;
     cout << "VP: " << VerticalPosition(true) << endl;

     cout << "VS: " << VerticalSize(true) << endl;
     cout << "VS: " << VerticalSize(4.5e-2) << endl;
     cout << "VS: " << VerticalSize(true) << endl;
     cout << "VS: " << VerticalSize(-1.0) << endl;
     cout << "VS: " << VerticalSize(true) << endl;

#endif
    cout << "PZ OFF: " << SetPanZoom(false) << endl;
    cout << "PZ: " << PanZoom(true) << endl;

    //cout << *this;
}
#endif
