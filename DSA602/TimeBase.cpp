/********************************************************************
 *
 * Module Name : TimeBase.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic TimeBase
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
#include <unistd.h>

// Local Includes.
#include "debug.h"
#include "DSA602.hh"
#include "TimeBase.hh"
#include "GParse.hh"


/* Command, type, upper bound, lower bound */
const struct t_Commands TimeBase::TBCommands[kEND_LIST+1]= {
    {"LEN",        kCT_FLOAT, 32768.0,   512.0},  // Amplifier offset
    {"TIM",        kCT_FLOAT,   100.0,50.0e-12},
    {"XIN",        kCT_FLOAT,     0.0,     0.0},  // Query only
    {NULL,         kCT_NONE,      0.0,     0.0},
};

/*
 * These are the numerical values that are accessed by the enum in
 * DSA602_Types.hh
 */
const double TimeBase::Period[kTB_END] = 
{50.0e-12, 100.0e-12, 200.0e-12, 500.0e-12, 
 1.0e-9, 2.0e-9, 4.0e-9, 5.0e-9, 10.0e-9, 20.0e-9,25.0e-9, 50.0e-9, 
 100.0e-9, 200.0e-9, 250.0e-9, 400.0e-9, 500.0e-9, 800.0e-9,
 1.0e-6, 2.0e-6, 2.5e-6, 4.0e-6, 5.0e-6, 8.0e-6, 10.0e-6,
 20.0e-6, 40.0e-6, 50.0e-6, 100.0e-6};

struct t_TBLength TimeBase::PossibleLengths[10] = {
    {  512, false},   // 0
    { 1024, false},   // 1
    { 2048, false},   // 2
    { 4096, false},   // 3
    { 5120, false},   // 4
    { 8192, false},   // 5
    {10240, false},   // 6
    {16384, false},   // 7
    {20464, false},   // 8
    {32768, false}};  // 9

/**
 ******************************************************************
 *
 * Function Name : TimeBase constructor
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
TimeBase::TimeBase (void) : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetName("TimeBase");

    // Set to no values
    fNPossibleLengths        = 0; 

    memset(fLength,0, kEND_FRAME*sizeof(double));
    memset(fTime,0, kEND_FRAME*sizeof(double));
    memset(fXIncrement,0, kEND_FRAME*sizeof(double));

    // Try filling as much as we can

    Query(kEND_LIST, kMAIN);
    Query(kEND_LIST, kWINDOW);
#ifdef DEBUG_TB
    Test();
#endif
}

/**
 ******************************************************************
 *
 * Function Name : TimeBase destructor
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
TimeBase::~TimeBase ()
{
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
/**
 ******************************************************************
 *
 * Function Name : Query
 *
 * Description : Determine what the value is for the associated input 
 *               command. 
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
 * Unit Tested on: 29-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool TimeBase::Query(COMMANDs c, bool Main)
{
    char   cstring[32], Response[64];
    char   frame;
    DSA602 *pDSA602 = DSA602::GetThis();
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if(Main)
    {
	frame = 'M';
    }
    else
    {
	frame = 'W';
    }
    // Query for specific data.

    if (c<kEND_LIST)
    {
	sprintf( cstring, "TB%c? %s", frame, TBCommands[c].Command);
    }
    else
    {
	sprintf( cstring, "TB%c?", frame);
    }
    //cout << "----TIMEBASE Query: " << cstring ;

    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	//cout << " RESPONSE: " << Response << endl;
	Decode(Response, Main);
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
 * Function Name : Decode
 *
 * Description :
 *     Decode the return string for any query.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 05-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool TimeBase::Decode(const char *c, bool Main)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    string response(c);
    size_t start = 0;
    size_t end   = 0;
    /*
     * A response from TBM? looks like:
     *
     * 'TBMAIN LENGTH:512,TIME:5.0E-7,XINCR:1.0E-8'
     *
     * and TBM? LEN
     *
     * 'TBMAIN LENGTH:512'
     * Skip over the preamble. e.g. TBMAIN
     */
    start = response.find(' ',0) +1;
    string cmd;
    string val;
    int index;
    if (Main)
    {
	index = kMAIN;
    }
    else
    {
	index = kWINDOW;
    }

    do {

	end   = response.find(':',start);   // find the command delimeter
	cmd   = response.substr(start, end-start);
	start = end+1;
	end   = response.find(',',start);
	val   = response.substr(start, end-start);
	start = end + 1;
	//cout << " CMD: " << cmd << "  VAL: " << val << endl;

	if (cmd.find("LENGTH") == 0)
	{
	    fLength[index] = atof(val.c_str());
	}
	else if (cmd.find("TIME") == 0)
	{
	    fTime[index] = atof(val.c_str());
	}
	else if (cmd.find("XINCR") == 0)
	{
	    fXIncrement[index] = atof(val.c_str());
	}

    } while (end<response.size());
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Channel << operator
 *
 * Description : format all the TimeBase information for output
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 31-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const TimeBase &n)
{
    SET_DEBUG_STACK;

    output << "============================================" << endl
	   << "TimeBase data: " << endl 
	   << "  MAIN" << endl
	   << "        Length: " << n.fLength[TimeBase::kMAIN]
	   << "          TIME: " << n.fTime[TimeBase::kMAIN]
	   << "    XIncrement: " << n.fXIncrement[TimeBase::kMAIN] << endl
	   << "  Window" << endl
	   << "        Length: " << n.fLength[TimeBase::kWINDOW]
	   << "          TIME: " << n.fTime[TimeBase::kWINDOW]
	   << "    XIncrement: " << n.fXIncrement[TimeBase::kWINDOW] << endl
	   << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : CheckLength
 *
 * Description :
 * Look at page 273 for details. 
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
double TimeBase::CheckLength(double Time)
{
    SET_DEBUG_STACK;
    SET_DEBUG_STACK;
    size_t index = 0;
    double T = -1.0;  // period

    // Nothing permitted. 
    for(int i=0;i<10;i++) PossibleLengths[i].valid = false;


    if(Time>100.0e-6)
    {
	// Everything permitted. 
	fNPossibleLengths = 10;
	for(int i=0;i<10;i++) PossibleLengths[i].valid = true;
	// Continious domain, everything is permitted. 
	return Time;
    }


    /* 
     * Deterimine what is the closed value in the array to the 
     * specified time. 
     */
    while ((Time<Period[index]) && (index<kTB_END))
    {
	index++;
    }
    if (index<kTB_END) 
    {
	/*
	 * we are in the discrete end of things. Which index is it
	 * closer to. 
	 * 
	 * index is just above the Time value. 
	 */
	T = Period[index];
	switch (index)
	{
	case k50ps: 
	    fNPossibleLengths = 1; 
	    PossibleLengths[0].valid = true;
	    break;
	case k100ps: 
	    fNPossibleLengths = 2; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    break;
	case k200ps: 
	    fNPossibleLengths = 3; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    break;
 	case k400ps: 
 	    fNPossibleLengths = 1; 
 	    PossibleLengths[2].valid = true;
 	    break;
	case k500ps: 
	    fNPossibleLengths = 4; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    break;
	case k1ns: 
	    fNPossibleLengths = 6; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    break;
	case k2ns: 
	    fNPossibleLengths = 8; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    break;
	case k4ns: 
	    fNPossibleLengths = 2; 
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    break; 
	case k5ns:
	    fNPossibleLengths = 8; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[9].valid = true;
	    break;
	case k10ns: 
	    fNPossibleLengths = 10; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k20ns: 
	    fNPossibleLengths = 9; 
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k25ns: 
	    fNPossibleLengths = 1; 
	    PossibleLengths[0].valid = true;
	    break; 
	case k50ns: 
	    fNPossibleLengths = 10; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k100ns:
	    fNPossibleLengths = 10; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k200ns: 
	    fNPossibleLengths = 7; 

	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k250ns: 
	    fNPossibleLengths = 2; 
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    break; 
	case k400ns: 
	    fNPossibleLengths = 2; 
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    break; 
	case k500ns: 
	    fNPossibleLengths = 8; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k800ns: 
	    fNPossibleLengths = 1; 
	    PossibleLengths[2].valid = true;
	    break; 
	case k1us: 
	    fNPossibleLengths = 9; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k2us: 
	    fNPossibleLengths = 9; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    break; 
	case k2p5us: 
	    fNPossibleLengths = 1; 
	    PossibleLengths[9].valid = true;
	    break; 
	case k4us:
	    fNPossibleLengths = 5; 
	    PossibleLengths[2].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    break; 
	case k5us: 
	    fNPossibleLengths = 5; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[9].valid = true;
	    break; 
	case k8us:
	    fNPossibleLengths = 2; 
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    break; 
	case k10us:
	    fNPossibleLengths = 8; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    break; 
	case k20us:
	    fNPossibleLengths = 10; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break;
	case k40us:
	    fNPossibleLengths = 2; 
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    break;
	case k50us:
	    fNPossibleLengths = 8; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[9].valid = true;

	    break; 
	case k100us:
	    fNPossibleLengths = 10; 
	    PossibleLengths[0].valid = true;
	    PossibleLengths[1].valid = true;
	    PossibleLengths[2].valid = true;
	    PossibleLengths[3].valid = true;
	    PossibleLengths[4].valid = true;
	    PossibleLengths[5].valid = true;
	    PossibleLengths[6].valid = true;
	    PossibleLengths[7].valid = true;
	    PossibleLengths[8].valid = true;
	    PossibleLengths[9].valid = true;
	    break;
	default:
	    return false;
	    break;
	}
    }
    else
    {
	// We really should not end up here.
	SET_DEBUG_STACK; 
	return -1.0;
    }

    return T;
}

/**
 ******************************************************************
 *
 * Function Name : SetPeriod
 *
 * Description :
 * Look at page 273 for details. 
 *
 * Inputs :
 *   enum for one of the permissible values. 
 *   main   - if set to true, then it is a main window result. 
 *
 * Returns :
 *   true on success
 *
 * Error Conditions : GPIB error
 * 
 * Unit Tested on: 05-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool TimeBase::SetPeriod(PERIOD Time, bool Main)
{
    char   cstring[32];
    char   frame;
    DSA602 *pDSA602 = DSA602::GetThis();
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if(Main)
    {
	frame = 'M';
    }
    else
    {
	frame = 'W';
    }
    /*
     *  make the string for the set. use Time as a pointer 
     * into the PossibleLengths vector. 
     */
    sprintf(cstring, "TB%c TIM:%g", frame, Period[Time]);
    //cout << "----TIMEBASE TIME SET: " << cstring << endl;

    if(!pDSA602->Command(cstring, NULL, 0))
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
 * Function Name : SetLength
 *
 * Description :
 * Look at page 273 for details. 
 *
 * Inputs :
 *   enum for one of the permissible values. 
 *   main   - if set to true, then it is a main window result. 
 *
 * Returns :
 *   true on success
 *
 * Error Conditions : GPIB error
 * 
 * Unit Tested on: 05-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool TimeBase::SetLength(LENGTH Lindex, bool Main)
{
    char   cstring[32];
    char   frame;
    int    ival;
    DSA602 *pDSA602 = DSA602::GetThis();
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if(Main)
    {
	frame = 'M';
    }
    else
    {
	frame = 'W';
    }
    /*
     *  make the string for the set. use Lindex as a pointer 
     * into the PossibleLengths vector. 
     */
    ival = PossibleLengths[Lindex].val;
    sprintf(cstring, "TB%c LEN:%d", frame, ival);
    //cout << "----TIMEBASE LENGTH SET: " << cstring << endl;

    if(!pDSA602->Command(cstring, NULL, 0))
    {
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
#ifdef DEBUG_TB
void TimeBase::Test(void)
{
    SET_DEBUG_STACK;

    cout << *this;

    cout << "Main..... " << endl;
    cout << "  Length: " << MainLength(true) << endl;
    cout << "  SETL  : " << MainLength(k1024);
    cout << "  Length: " << MainLength(true) << endl;
    cout << "  SETL  : " << MainLength(k512);

    cout << "    Time: " << MainTime(true) << endl;
    cout << "    SETT: " << MainTime(k10us) << endl;
    sleep(1);
    cout << "    Time: " << MainTime(true) << endl;
    cout << "    SETT: " << MainTime(k5us) << endl;

    cout << "   XIncr: " << MainXIncrement(true) << endl;
    cout << "Window..... " << endl;
    cout << "  Length: " << WindowLength(true) << endl;
    cout << "    Time: " << WindowTime(true) << endl;
    cout << "   XIncr: " << WindowXIncrement(true) << endl;
}
#endif



