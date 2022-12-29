/********************************************************************
 *
 * Module Name : AdjTrace.cpp
 *
 * Author/Date : C.B. Lirakis / 13-Feb-11
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

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "DSA602.hh"
#include "AdjTrace.hh"

/* Command, type, upper bound, lower bound */
const struct t_Commands AdjTrace::Commands[kEND_LIST+1]= {
    {"FRES",       kCT_FLOAT, 32768.0,   512.0}, 
    {"FSP",        kCT_FLOAT,   100.0,50.0e-12},
    {"HMA",        kCT_FLOAT,     0.0,     0.0},
    {"HPO",        kCT_FLOAT,     0.0,     0.0},
    {"HVP",        kCT_FLOAT,     0.0,     0.0},
    {"HVS",        kCT_FLOAT,     0.0,     0.0},
    {"PAN",        kCT_BOOL,      0.0,     0.0},
    {"TRS",        kCT_FLOAT,     0.0,     0.0},
    {"VPO",        kCT_FLOAT,     0.0,     0.0},
    {"VSI",        kCT_FLOAT,     0.0,     0.0},
    {NULL,         kCT_NONE,      0.0,     0.0},
};

/**
 ******************************************************************
 *
 * Function Name : AdjTrace constructor
 *
 * Description : Right now just clears the variables. 
 * 
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
AdjTrace::AdjTrace(void) : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetName("AdjTrace");
    Clear();
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : Reset all the AdjTrace values to zero or false. 
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
void AdjTrace::Clear(void)
{
    SET_DEBUG_STACK;
    fTraceNumber              = 0;
    fPanZoom                  = false; 
    fHorizontialMagnification = 0.0;
    fHorizontialPosition      = 0.0; 
    fHVPosition               = 0.0; 
    fHVSize                   = 0.0;
    fTRSEP                    = 0.0; 
    fVPosition                = 0.0; 
    fVSize                    = 0.0; 
    fFSpan                    = 0.0;
    fFResolution              = 0.0;
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
bool AdjTrace::Update(void)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    char     s[32];
    char     Response[256];


    // Command the mainframe to tell me everything about this trace. 
    sprintf(s, "ADJ %d", fTraceNumber); 
    memset(Response, 0, sizeof(Response));
    if (pDSA602->Command(s, Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# AdjTrace::Update %s\n", Response);
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
 *               an AdjTrace call, parse and fill the correct field. 
 *
 * Inputs :
 *     string containing something like:  
 *
 * 'PANZOOM:OFF,HMAG:-1.0E+0,HPOSITION:1.0E+16,HVPOSITION:1.0E+16,HVSIZE:-1.0E+0,TRSEP:1.0E+16,VPOSITION:1.0E+16,VSIZE:-1.0E+0,FSPAN:-1.0E+0,FRESOLUTION:-1.0E+0'
 *
 * Returns :
 *     true on success
 *
 * Error Conditions :
 *     NONE
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::Decode(const string &response) 
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
     * 'ADJTRACE1 HMAG:-1.0E+0'
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
	//cout << " CMD: " << cmd << "  VAL: " << val << endl;

	if (cmd.find("PANZOOM") < string::npos)
	{
	    if (val.find("ON") != string::npos)
	    {
		fPanZoom = true;
	    }
	    else 
	    {
		fPanZoom = false;
	    }
	}
	else if (cmd.find("HMAG") < string::npos)
	{
	    fHorizontialMagnification = stod(val);
	}
	else if (cmd.find("HPOSITION") < string::npos)
	{
	    fHorizontialPosition = stod(val);
	}
	else if (cmd.find("HVPOSITION") < string::npos)
	{
	    fHVPosition = stod(val);
	}
	else if (cmd.find("HVSIZE") < string::npos)
	{
	    fHVSize = stod(val);
	}
	else if (cmd.find("TRSEP") < string::npos)
	{
	    fTRSEP = stod(val);
	}
	else if (cmd.find("VPOSITION") < string::npos)
	{
	    fVPosition = stod(val);
	}
	else if (cmd.find("VSIZE") < string::npos)
	{
	    fVSize = stod(val);
	}
	else if (cmd.find("FSPAN") < string::npos)
	{
	    fFSpan = stod(val);
	}
	else if (cmd.find("FRESOLUTION") < string::npos)
	{
	    fFResolution = stod(val);
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
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::Query(COMMANDs c)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    char   cstring[32], Response[4096];
    ClearError(__LINE__);

    memset(cstring, 0, sizeof(cstring));

    if (c<kEND_LIST)
    {
	sprintf( cstring, "ADJ%d? %s", fTraceNumber, Commands[c].Command);
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
	    log->Log("# AdjTrace::Query: %s\n", Response);
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
 * Function Name : HorizontalMagnification
 *
 * Description : 
 *     Set the horizontal magnification for the current channel. 
 *
 *     MISSING: CHECKING RECORD SIZE. 
 * 
 * Inputs : 
 *    value - HMAG enum of possible values.
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::HorizontalMagnification(HMAG_VAL value)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }

    /*
     * Not sure how to connect this to a record length. 
     */
    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d HMA:%f",fTraceNumber, HMAG_Values[value]);
    if(log->CheckVerbose(1))
    {
	log->Log("# AdjTrace::HMAG: %s\n", cstring);
    }
    rc = pDSA602->Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetPanZoom
 *
 * Description : 
 *     Set the pan/zoom on or off. 
 * 
 * Inputs : 
 *    bool on - true, off - false
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::SetPanZoom(bool val)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool     rc     = false;
    char cstring[32];
    ClearError(__LINE__);

    memset(cstring, 0, sizeof(cstring));
    if(val)
    {
	sprintf( cstring, "ADJ%d PAN:ON",fTraceNumber);
    }
    else
    {
	sprintf( cstring, "ADJ%d PAN:OFF",fTraceNumber);
    }
    //cout << __FUNCTION__ << "  bool : " << cstring << endl;
    rc = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# AdjTrace::PAN: %s %d\n", cstring, rc);
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : HorizontalPosition
 *
 * Description : 
 * Helper functions for HVP, HVS, VPO, VSI
 * 
 * Inputs : 
 *    value - integer value to set for number of waveform points
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::PosSize(bool Horizontal, bool Position, double value)
{
    SET_DEBUG_STACK;
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    const char *HV, *P;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }

    // FIXME
//     if (fabs(value)>1.0e15)
//     {
// 	SET_DEBUG_STACK;
// 	SetError(-1,__LINE__);
// 	return false; // This is not an error condition. 
//    }
    if (Horizontal)
    {
	HV = "HV";
	if (Position)
	{
	    P = "P";
	}
	else
	{
	    P = "S";
	}
    }
    else
    {
	HV = "V";
	if (Position)
	{
	    P = "PO";
	}
	else
	{
	    P = "SI";
	}
    }
    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d %s%s:%g",fTraceNumber, HV, P, value);
    // FIXME
    rc = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# AdjTrace::POSIZE: %s %d\n", cstring, rc);
    }
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : HorizontalPosition
 *
 * Description : 
 *   Set the waveform horizontial position when PANZOOM is true. The HPO
 * value is in waveform points from 0(zero) to the upper value determined
 * by the setting of HMAG and the record length by the following formula. 
 * 
 *    length - ceil(10.24*max_HMAG/HMAG)
 *
 *     MISSING: CHECKING RECORD SIZE. 
 * 
 * 
 * Inputs : 
 *    value - integer value to set for number of waveform points
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::HorizontalPosition(size_t v)
{
    SET_DEBUG_STACK;
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }

    // Not sure hot to get the record size
    /*
     * Not sure how to connect this to a record length.
     * Must be less than length - ceil(10.24*max_HMAG/HMAG
     */
    
    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d HPO:%ld",fTraceNumber, v);
    //cout << __FUNCTION__ << "  int : " << cstring << endl;
    rc = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# AdjTrace::HorizontalPosition: %s %d\n", cstring, rc);
    }
    SET_DEBUG_STACK;
    return rc;
}
#if 0
/**
 ******************************************************************
 *
 * Function Name :  HVPosition
 *
 * Description : 
 *   For XY waveforms created in floating-point mode, HVPosition
 *   sets the graphical position of the horizontal component of the
 *   waveform. 
 * 
 * 
 * Inputs : 
 *    v - value {-1E15,1E15}
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::HVPosition(double v)
{
    SET_DEBUG_STACK;
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }
    if (fabs(v)>1.0e15)
    {
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false; // This is not an error condition. 
    }
    
    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d HVP:%g",fTraceNumber, v);
    //cout << __FUNCTION__ << "  double : " << cstring << endl;
    rc = pDSA602->Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name :  HVSize
 *
 * Description : 
 *   For XY waveforms created in floating-point mode, HVSize
 *   sets the graphical size of the horizontal component of the
 *   waveform. 
 * 
 * 
 * Inputs : 
 *    v - value {-1E15,1E15}
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 13-Feb-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::HVSize(double v)
{
    SET_DEBUG_STACK;
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }
    if (fabs(v)>1.0e15)
    {
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false; // This is not an error condition. 
    }

    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d HVS:%g",fTraceNumber, v);
    cout << __FUNCTION__ << "  double : " << cstring << endl;
    rc = pDSA602->Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return rc;
}
#endif
/**
 ******************************************************************
 *
 * Function Name :  TraceSeparation
 *
 *
 * Description : 
 *   For XY waveforms created in floating-point mode, HVSize
 *   sets the graphical size of the horizontal component of the
 *   waveform. 
 * 
 * 
 * Inputs : 
 *    v - value {-5,5}
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on:  NOT TESTED
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool AdjTrace::TraceSeparation(double v)
{
    SET_DEBUG_STACK;
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (!fPanZoom)
    {
	SET_DEBUG_STACK;
	return false; // This is not an error condition. 
    }
    if (fabs(v)>5.0)
    {
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false; // This is not an error condition. 
    }

    memset(cstring, 0, sizeof(cstring));
    sprintf( cstring, "ADJ%d TRS:%g",fTraceNumber, v);

    rc = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# AdjTrace::TraceSep: %s %d\n", cstring, rc);
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Trace << operator
 *
 * Description : format all the Trace information for output
 *
 * Inputs :
 *     ouptut - the stream to fill with the formatted data. 
 *     n = the AdjTrace class to pull the data from. 
 *
 * Returns :
 *     fully formatted stream output
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
ostream& operator<<(ostream& output, const AdjTrace &n)
{
    SET_DEBUG_STACK;
    

    output << "============================================" << endl
	   << "AdjTrace data: " << endl 
	   << "   Trace Number: " << n.fTraceNumber << endl;
    if (n.fPanZoom)
    {
	output << "       Pan Zoom:ON " << endl;
    }
    else
    {
	output << "       Pan Zoom:OFF" << endl;
    }
    output << "           Horizontial Mag: " << n.fHorizontialMagnification << endl
	   << "       Horizontal Position: " << n.fHorizontialPosition << endl
	   << "               HV Position: " << n.fHVPosition << endl
	   << "                   HV Size: " << n.fHVSize << endl
	   << "          Trace separation: " << n.fTRSEP << endl
	   << "         Vertical Position: " << n.fVPosition << endl
	   << "             Vertical Size: " << n.fVSize << endl
	   << "     Frequency Span (FFT) : " << n.fFSpan << endl
	   << " Frequency Resolution(FFT): " << n.fFResolution
	   << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}
#ifdef DEBUG_TRACE
void AdjTrace::Test(void)
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
    cout << "SET HM 2: " << HorizontalMagnification(AdjTrace::kHM_2) << endl;
    cout << "HM: " << HorizontalMagnification(true) << endl;
    cout << "SET HM 1: " << HorizontalMagnification(AdjTrace::kHM_1) << endl;
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
