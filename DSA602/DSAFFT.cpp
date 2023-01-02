/********************************************************************
 *
 * Module Name : DSAFFT.cpp
 *
 * Author/Date : C.B. Lirakis / 19-Dec-14
 *
 * Description : Generic module
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
// #include <cmath>
// #include <string.h>
// #include <stdlib.h>
// #include <arpa/inet.h>

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "DSA602.hh"
#include "DSAFFT.hh"
#include "GParse.hh"
#include "DSA602_Utility.hh"

/* Command, type, upper, lower */
struct t_Commands DSAFFT::DSAFFTCommands[7]= {
    {"AVG",         kCT_BOOL,      0.0,0.0},
    {"DCSUP",       kCT_BOOL,      0.0,0.0},
    {"FORMAT",      kCT_FORMAT,    0.0,0.0},
    {"PHASE",       kCT_BOOL,      0.0,0.0},
    {"WINDOW",      kCT_WINDOW,    0.0,0.0},
    {"FILTER",      kCT_BOOL,      0.0,0.0},
    {NULL,          kCT_NONE,      0.0,0.0},
};


/**
 ******************************************************************
 *
 * Function Name : DSAFFT constructor
 *
 * Description : Construct a DSA602 FFT status from a text input
 *
 * Inputs : input to be decoded
 *
 * Returns :
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  24-Dec-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
DSAFFT::DSAFFT(const char *val) : CObject()
{
    SET_DEBUG_STACK;
    SetName("DSAFFT");
    ClearError(__LINE__);
    fText = new string("NONE");
    fAVG    = false;
    fDCSUP  = false; 
    fFORMat = kFFTNONE;   
    fPHAse  = false; 
    fWINDow = kWINDOW_NONE;
    fFILTer = false;
    fNAvg   = 0;
    if (val!=NULL)
    {
	DecodeString(val);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : DSAFFT destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on:  24-Dec-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
DSAFFT::~DSAFFT(void)
{
    SET_DEBUG_STACK;
    delete fText;
}
/**
 ******************************************************************
 *
 * Function Name : DSAFFT output operator
 *
 * Description : 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on:  24-Dec-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const DSAFFT &n)
{
    SET_DEBUG_STACK;
    const char *Format[] = {"dBm", "dB Fund", "dB Peak", "dBV RMS", 
			    "V Peak", "V RMS"};
    const char *Window[] = {"Blackman", "Blackman-Harris", "Hamming",
			    "Hanning", "Rectangular", "Triangular"};

    output << "============================================" << endl
	   << "DSAFFT data: " << endl
	   << "    Avg: ";
    if(n.fAVG)
    {
	output << "ON";
    }
    else
    {
	output << "OFF";
    }
    output << " Number: " << n.fNAvg;
    output << ", DC supression: ";
    if (n.fDCSUP)
    {
	output << "On";
    }
    else
    {
	output << "OFF";
    }
    output << ", Format: "
	   << Format[n.fFORMat] << endl
	   << "    Phase: ";
    if (n.fPHAse)
    {
	output << " Unwrap";
    }
    else
    {
	output << " Wrap";
    }
    output << ", Window: " << Window[n.fWINDow] 
	   << ", Filter: ";
    if (n.fFILTer)
    {
	output << "On";
    }
    else
    {
	output << "OFF";
    }
    output << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : DecodeString
 *
 * Description : Decode the FFT response string
 *
 * Inputs : string to parse
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void DSAFFT::DecodeString(const char *val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    const char *p = NULL;

    /*
     * Typical channel query data. page 150
     * FFT AVG:ON,FORMAT:DBM,WINDOW:RECTANGULAR,PHASE:WRAP,DCSUP:OFF'
     */

    GParse par(val);
    if ((p = par.Value("AVG")) != NULL)
    {
	fAVG = (strncmp(p,"ON", 2)==0);
    }
    if ((p = par.Value("DCSUP")) != NULL)
    {
	fDCSUP = (strncmp(p,"ON",2 )==0);
    }
    if ((p = par.Value("FORMAT")) != NULL)
    {
	if (strncmp(p,"DBM",3)==0)
	{
	    fFORMat = kDBM;
	}
	else if (strncmp(p,"DBFUND",6)==0)
	{
	    fFORMat = kDBFUND;
	}
	else if (strncmp(p,"DBVPEAK",7)==0)
	{
	    fFORMat = kDBVPEAK;
	}
	else if (strncmp(p,"DBVRMS",6)==0)
	{
	    fFORMat = kDBVRMS;
	}
	else if (strncmp(p,"VPEAK",5)==0)
	{
	    fFORMat = kVPEAK;
	}
	else if (strncmp(p,"VRMS",4)==0)
	{
	    fFORMat = kVRMS;
	}
    }
    if ((p = par.Value("PHASE")) != NULL)
    {
	fPHAse = (strncmp(p,"UNW",3)==0);
    }
    if ((p = par.Value("WINDOW")) != NULL)
    {

	if (strncmp(p,"BLA",3)==0)
	{
	    fWINDow = kBLACKMAN;
	}
	else if (strncmp(p,"BLH",3)==0)
	{
	    fWINDow = kBLHARRIS;
	}
	else if (strncmp(p,"HAM",3)==0)
	{
	    fWINDow = kHAMMING;
	}
	else if (strncmp(p,"HAN",3)==0)
	{
	    fWINDow = kHANNING;
	}
	else if (strncmp(p,"RECT",4)==0)
	{
	    fWINDow = kRECTANGULAR;
	}
	else if (strncmp(p,"TRI",3)==0)
	{
	    fWINDow = kTRIANGULAR;
	}
    }
    if ((p = par.Value("FILTER")) != NULL)
    {
	fFILTer = (strncmp(p,"ENA",3)==0);
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : boolean version
 *
 * Inputs : true/false turn the requested feature on or off. 
 *
 * Returns : true on success
 *
 * Error Conditions : GPIB Write
 * 
 * Unit Tested on: 02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SendCommand(COMMANDs c, bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    char     cstring[64];
    bool     rv = false;

    if (c==kCPHASE)
    {
	return SetPhase(t);
    }
    else if (c==kCFILTER)
    {
	return SetFilter(t);
    }
    else
    {
	if (t)
	{
	    sprintf( cstring, "FFT %s:ON",DSAFFTCommands[c].Command);
	}
	else
	{
	    sprintf( cstring, "FFT %s:OFF",DSAFFTCommands[c].Command);
	}
    }
    rv = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DSAFFT::SendCommand boolean %s, rv: %d\n", cstring, rv);
    }
    SET_DEBUG_STACK;    
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetPhase
 *
 * Description : Alternative to SendCommand
 *
 * Inputs : true/false turn the requested feature on or off. 
 *
 * Returns : true on success
 *
 * Error Conditions : GPIB Write
 * 
 * Unit Tested on: 02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SetPhase(bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[32];
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rv = false;

    if (t)
    {
	sprintf( cstring, "FFT PHASE:UNWRAP");
    }
    else
    {
	sprintf( cstring, "FFT PHASE:WRAP");
    }
    rv = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DSAFFT::SendCommand boolean %s, rv: %d\n", cstring, rv);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetFilter
 *
 * Description : Alternative to SendCommand
 *
 * Inputs : true/false turn the requested feature on or off. 
 *
 * Returns : true on success
 *
 * Error Conditions : GPIB Write
 * 
 * Unit Tested on: 02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SetFilter(bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[32];
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rv = false;


    cout << "NOT IMPLEMENTED." << endl;
    return false;
    if (t)
    {
	sprintf( cstring, "FFT FILTER:ENABLE");
    }
    else
    {
	sprintf( cstring, "FFT FILTER:DISABLE");
    }
    rv = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DSAFFT::SendCommand boolean %s, rv: %d\n", cstring, rv);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetFormat
 *
 * Description : Set the y axis units for the FFT
 *
 * Inputs : See the possible formats listed below. 
 *
 * Returns : true on success
 *
 * Error Conditions : GPIB Write
 * 
 * Unit Tested on: 02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SetFormat(FFTFORMAT f)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    static char *cformat[] = {(char*) "DBM", (char *) "DBFUND", 
			      (char*) "DBVPEAK", (char*) "DBVRMS",
			      (char*) "VPEAK", (char*) "VRMS"};
    char cstring[64];
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rv = false;

    sprintf( cstring, "FFT FORMAT:%s", cformat[f]);

    rv = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DSAFFT::SendCommand boolean %s, rv: %d\n", cstring, rv);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetWindow
 *
 * Description : Set the FFT window
 *
 * Inputs : See the possible formats listed below. 
 *
 * Returns : true on success
 *
 * Error Conditions : GPIB Write
 * 
 * Unit Tested on: 02-Jan-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SetWindow(FFT_WINDOW w)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    static char *cwindow[] = {(char*)"BLACKMAN", (char*) "BLHARRIS",
			      (char*) "HAMMING", (char*) "HANNING",
			      (char*) "RECTANGULAR", (char*) "TRIANGULAR"};
    char cstring[64];
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rv = false;

    sprintf( cstring, "FFT WINDOW:%s", cwindow[w]);
    rv = pDSA602->Command(cstring, NULL, 0);
    if(log->CheckVerbose(1))
    {
	log->Log("# DSAFFT::SendCommand boolean %s, rv: %d\n", cstring, rv);
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Fill up the structure from the DSA602 mainframe.
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : Failure on GPIB side. 
 * 
 * Unit Tested on: 21-Dec-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::Update(void)
{
    SET_DEBUG_STACK;
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    ClearError(__LINE__);
    bool rc = false;
    char Response[128];

    rc = pDSA602->Command("FFT?", Response, sizeof(Response));
    if (rc)
    {
	delete fText;
	fText = new string(Response);
	if(log->CheckVerbose(1))
	{
	    log->Log("# DSAFFT::Update %s\n", Response);
	}
	DecodeString(Response);
    }
    GetNAvg();
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : Fill up the structure from the DSA602 mainframe.
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : Failure on GPIB side. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::SetNAvg(uint16_t val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[32];
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rv = false;

    if ((val>1) && (val<65534))
    {
	sprintf( cstring, "NAVG:%d",val);
	rv = pDSA602->Command(cstring, NULL, 0);
	if(log->CheckVerbose(1))
	{
	    log->Log("# DSAFFT::SetNAVG %s, rv: %d\n", cstring, rv);
	}
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : 
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : Failure on GPIB side. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool DSAFFT::GetNAvg(void)
{
    SET_DEBUG_STACK;
    DSA602 * pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    ClearError(__LINE__);
    bool rc = false;
    char Response[128];
    char *p;

    rc = pDSA602->Command("NAVG?", Response, sizeof(Response));
    if (rc)
    {
	delete fText;
	fText = new string(Response);
	if(log->CheckVerbose(1))
	{
	    log->Log("# DSAFFT::NAVG %s\n", Response);
	}
	p = strstr(Response, "NAVG");
	p += 4; // Skip the word
	fNAvg = atoi(p);
    }
    SET_DEBUG_STACK;
    return rc;
}
