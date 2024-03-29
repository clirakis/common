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
#include "CLogger.hh"

TimeBase* TimeBase::fTimeBase;

/* Command, type, upper bound, lower bound */
const struct t_Commands TimeBase::TBCommands[kEND_LIST+1]= {
    {"LEN",        kCT_FLOAT, 32768.0,   512.0},  // Amplifier offset
    {"TIM",        kCT_FLOAT,   100.0,50.0e-12},
    {"XIN",        kCT_FLOAT,     0.0,     0.0},  // Query only
    {NULL,         kCT_NONE,      0.0,     0.0},
};
#if 0
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
#else
/*
 * These are the possible time base values to set the scope to. 
 * They also have a label associated with them for ease of reference. 
 * The number of samples per setting is shown in the table below. 
 * To determine if a sample size is valid call CheckLength below. 
 */
const struct t_Period TimeBase::Period[kTB_END] = {
    { 50.0e-12, " 50ps"}, 
    {100.0e-12, "100ps"}, 
    {200.0e-12, "200ps"}, 
    {400.0e-12, "400ps"}, 
    {500.0e-12, "500ps"}, 
    {  1.0e-9,  "  1ns"}, 
    {  2.0e-9,  "  2ns"}, 
    {  4.0e-9,  "  4ns"}, 
    {  5.0e-9,  "  5ns"}, 
    { 10.0e-9,  " 10ns"}, 
    { 20.0e-9,  " 20ns"},
    { 25.0e-9,  " 25ns"}, 
    { 50.0e-9,  " 50ns"}, 
    {100.0e-9,  "100ns"}, 
    {200.0e-9,  "200ns"}, 
    {250.0e-9,  "250ns"}, 
    {400.0e-9,  "400ns"}, 
    {500.0e-9,  "500ns"}, 
    {800.0e-9,  "800ns"}, // CHECK 
    {  1.0e-6,  "  1us"}, 
    {  2.0e-6,  "  2us"}, 
    {  2.5e-6,  "2.5us"}, 
    {  4.0e-6,  "  4us"}, 
    {  5.0e-6,  "  5us"}, 
    {  8.0e-6,  "  8us"}, 
    { 10.0e-6,  " 10us"}, 
    { 20.0e-6,  " 20us"}, 
    { 40.0e-6,  " 40us"}, 
    { 50.0e-6,  " 50us"},
    {100.0e-6,  "100us"},
    {200.0e-6,  "200us"},
    {500.0e-6,  "500us"},
    {  1.0e-3,  "  1ms"},
    {  2.0e-3,  "  2ms"},
    {  5.0e-3,  "  5ms"},
    { 10.0e-3,  " 10ms"},
    { 20.0e-3,  " 20ms"},
    { 50.0e-3,  " 50ms"},
    {100.0e-3,  "100ms"},
    {200.0e-3,  "200ms"},
    {500.0e-3,  "500ms"},
    {  1.0   ,  "  1s"},
    {  2.0   ,  "  2s"},
    {  5.0   ,  "  5s"},
    { 10.0   ,  " 10s"},
    { 20.0   ,  " 20s"},
    { 50.0   ,  " 50s"},
    {100.0   ,  "100s"}};

#endif
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


/*
 * Allowed values
 *           1    2     4     8     10    20    40     80    100     200
 *  -+-----+----+----+-----+-----+------+-----+-----+-----+------+--------
 *  0  50ps 512
 *  1 100ps 512, 1024
 *  2 200ps 512, 1024, 2048
 *  3 400ps            2048
 *  ----------------------------------------------------------------------
 *  4 500ps 512, 1024,       4096, 5120
 *  5   1ns 512, 1024, 2048, 4096, 5120, 8129, 10240
 *  6   2ns 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464
 *  7   4ns                                           16384, 20464
 *  ----------------------------------------------------------------------
 *  8   5ns 512, 1024, 2048, 4096, 5120, 8129, 10240,               32768
 *  9  10ns 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768
 * 10  20ns      1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768
 * 11  25ns 512
 *  ----------------------------------------------------------------------
 * 12  50ns 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768
 * 13 100ns 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768 
 * 14 200ns      1024, 2048,             8129, 10240, 16384, 20464, 32768
 * 15 250ns                  4096, 5120
 *  ----------------------------------------------------------------------
 * 16 400ns      1024, 2048
 * 17 500ns 512,             4096, 5120, 8129, 10240, 16384, 20464, 32768
 * 18   1us 512, 1024,       4096, 5120, 8129, 10240, 16384, 20464, 32768 
 * 19   2us 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464
 *  ----------------------------------------------------------------------
 * 20 2.5us                                                         32768
 * 21   4us            2048,             8129, 10240, 16384, 20464
 * 22   5us 512, 1024,       4096, 5120,                            32768
 * 23   8us                                           16384, 20464
 *  ----------------------------------------------------------------------
 * 24  10us 512, 1024, 2048, 4096, 5120, 8129, 10240,               32768 
 * 25  20us 512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768 
 * 26  40us                                           16384, 20464
 * 27  50us 512, 1024, 2048, 4096, 5120, 8129, 10240,               32768 
 *  ----------------------------------------------------------------------
 * 28 100us+512, 1024, 2048, 4096, 5120, 8129, 10240, 16384, 20464, 32768 
 */

/**
 ******************************************************************
 *
 * Function Name : TimeBase constructor
 *
 * Description : create an interface to the setting the timebase
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
TimeBase::TimeBase (void) : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetName("TimeBase");

    fTimeBase = this;
    // Set to no values
    fNPossibleLengths        = 0; 

    memset(fLength    , 0, kEND_FRAME*sizeof(double));
    memset(fTime      , 0, kEND_FRAME*sizeof(double));
    memset(fXIncrement, 0, kEND_FRAME*sizeof(double));

    fMText = new string("NONE");
    fWText = new string("NONE");
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
    SET_DEBUG_STACK;
    delete fMText;
    delete fWText;
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
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();  
    char   cstring[32], Response[64];
    char   frame;
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

    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	if(Main)
	{
	    delete fMText;
	    fMText = new string(Response);
	}
	else
	{
	    delete fWText;
	    fWText = new string(Response);
	}

	if(log->CheckVerbose(1))
	{
	    log->Log("# TimeBase::Query Command: %s, Response: %s\n", 
		     cstring, Response);
	}

	Decode(Response);
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
 * Function Name : Update
 *
 * Description : Update both Main and Window parameters
 *
 * Inputs : NONE
 *
 * Returns : true on success
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
bool TimeBase::Update(void)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();  
    char     Response[64];
    bool     rv = false;
    ClearError(__LINE__);



    memset(Response, 0, sizeof(Response));
    if(pDSA602->Command("TBM?", NULL, 0))
    {
	delete fMText;
	fMText = new string(Response);
	if(log->CheckVerbose(1))
	{
	    log->Log("# TimeBase::Update (Main) Response: %s\n", Response);
	}
	Decode(Response);
	rv = true;
    }

    memset(Response, 0, sizeof(Response));
    if(pDSA602->Command("TBW?", NULL, 0))
    {
	delete fWText;
	fWText = new string(Response);
	if(log->CheckVerbose(1))
	{
	    log->Log("# TimeBase::Update (Window) Response: %s\n", Response);
	}
	Decode(Response);
	rv = true;
    }
    else
    {
	rv = false;
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
 * Inputs : response string to decode. 
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
bool TimeBase::Decode(const char *c)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    string  response(c);
    size_t  start = 0;
    size_t  end   = 0;
    uint8_t index = kMAIN;
    /*
     * A response from TBM? looks like:
     *
     * 'TBMAIN LENGTH:512,TIME:5.0E-7,XINCR:1.0E-8'
     *
     * and TBM? LEN
     *
     * 'TBMAIN LENGTH:512'
     * Determine based on preamble if it is MAIN or WINDOW
     */
    if (response.find("MAIN") != string::npos)
    {
	index = kMAIN;
    }
    else if (response.find("WIN") != string::npos)
    {
	index = kWINDOW;
    }
    else
    {
	index = kMAIN;
    }

    start = response.find(' ',0) +1;
    string cmd;
    string val;
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
	    //cout << "DECODE index: " << index << " TIME: " << val << endl;
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
 * Function Name : 
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
bool TimeBase::SampleLengthsFromTimeIndex(PERIOD index)
{
    SET_DEBUG_STACK;
    bool rc = true;

    //cout << __FUNCTION__ << " index " << index << endl;
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
    case k200us:
    case k500us:
    case k1ms: 
    case k2ms: 
    case k5ms: 
    case k10ms: 
    case k20ms: 
    case k50ms: 
    case k100ms: 
    case k200ms: 
    case k500ms:
    case k1s:
    case k2s: 
    case k5s: 
    case k10s: 
    case k20s: 
    case k50s:
    case k100s:
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
	rc = false;
	    break;
    }
    SET_DEBUG_STACK;
    return rc;
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
    uint32_t index = k50ps;
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
    while ((Time<Period[index].DT) && (index<kTB_END))
    {
	index++;
    }

    /*
     * we are in the discrete end of things. Which index is it
     * closer to. 
     * 
     * index is just above the Time value. 
     */
    T = Period[index].DT;

    if(!SampleLengthsFromTimeIndex((PERIOD)index))
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
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();  
    char   cstring[32];
    char   frame;
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
    sprintf(cstring, "TB%c TIM:%g", frame, Period[Time].DT);
    //cout << "----TIMEBASE TIME SET: " << cstring << endl;
    if(log->CheckVerbose(1))
    {
	log->Log("# TimeBase::SetPeriod Command: %s\n", cstring);
    }

    if(!pDSA602->Command(cstring, NULL, 0))
    {
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return false;
    }

    // Go ahead and setup the allowable sample rate values too. 
    SampleLengthsFromTimeIndex(Time);

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
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();  
    char   cstring[32];
    char   frame;
    int    ival;
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
    if(log->CheckVerbose(1))
    {
	log->Log("# TimeBase::SetLength Command: %s\n", cstring);
    }
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
 * Function Name : IndexFromTime
 *
 * Description : 
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions :
 *
 * Unit Tested on:
 *
 * Unit Tested by:
 *
 *
 *******************************************************************
 */
uint32_t TimeBase::IndexFromTime(double Time)
{
    SET_DEBUG_STACK;
    uint32_t TimeIndex = 0;

    while (TimeIndex<kTB_END)
    {
	if (Time == Period[TimeIndex].DT)
	{
	    break;
	}
	TimeIndex++;
    }
    
    SET_DEBUG_STACK;
    return TimeIndex;
}

/**
 ******************************************************************
 *
 * Function Name : IndexFromLength
 *
 * Description : 
 *
 * Inputs : None
 *
 * Returns : None
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
uint32_t TimeBase::IndexFromLength(double Length)
{
    SET_DEBUG_STACK;
    uint32_t LengthIndex = 0;

    if (Length<=512)
	LengthIndex = k512;
    else if (Length<=1024)
	LengthIndex = k1024;
    else if (Length<=2048)
	LengthIndex = k2048;
    else if (Length<=4096)
	LengthIndex = k4096;
    else if (Length<=5120)
	LengthIndex = k5120;
    else if (Length<=8192)
	LengthIndex = k8192;
    else if (Length<=16384)
	LengthIndex = k16384;
    else if (Length<=20464)
	LengthIndex = k20464;
    else if (Length<=32768)
	LengthIndex = k32768;
    else
	LengthIndex = kTB_END;
    SET_DEBUG_STACK;
    return LengthIndex;
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



