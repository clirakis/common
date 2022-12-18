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
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// Local Includes.
#include "debug.h"
#include "DSA602.hh"
#include "DSAFFT.hh"
#include "GParse.hh"
#include "DSA602_Utility.hh"

/* Command, type, upper, lower */
struct t_Commands DSAFFT::DSAFFTCommands[7]= {
    {"AVG",         kCT_BOOL, 0.0,0.0},
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
DSAFFT::DSAFFT(const char *val) : CObject()
{
    SET_DEBUG_STACK;
    SetName("DSAFFT");
    ClearError(__LINE__);
    fAVG    = false;
    fDCSUP  = false; 
    fFORMat = kFFTNONE;   
    fPHAse  = false; 
    fWINDow = kWINDOW_NONE;
    fFILTer = false;
    if (val!=NULL)
    {
	DecodeString(val);
    }
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
DSAFFT::~DSAFFT()
{
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
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : Floating point value
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
bool DSAFFT::SendCommand(COMMANDs c, bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[64];
    DSA602 *pDSA602 = DSA602::GetThis();


    if (c==CPHASE)
    {
	return SetPhase(t);
    }
    else if (c==CFILTER)
    {
	return SetFilter(t);
    }
    else
    {
	if (t)
	{
	    sprintf( cstring, "FFT %s: ON",DSAFFTCommands[c].Command);
	}
	else
	{
	    sprintf( cstring, "FFT %s: OFF",DSAFFTCommands[c].Command);
	}
    }
    return pDSA602->Command(cstring, NULL, 0);
}
bool DSAFFT::SetPhase(bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();

    if (t)
    {
	sprintf( cstring, "FFT PHASE: UNWRAP");
    }
    else
    {
	sprintf( cstring, "FFT PHASE: WRAP");
    }
    return pDSA602->Command(cstring, NULL, 0);
}
bool DSAFFT::SetFilter(bool t)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[32];
    DSA602 *pDSA602 = DSA602::GetThis();


    cout << "NOT IMPLEMENTED." << endl;
    return false;
    if (t)
    {
	sprintf( cstring, "FFT FILTER: ENABLE");
    }
    else
    {
	sprintf( cstring, "FFT FILTER: DISABLE");
    }
    return pDSA602->Command(cstring, NULL, 0);
}
bool DSAFFT::SetFormat(FFTFORMAT f)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    static char *cformat[] = {(char*) "DBM", (char *) "DBFUND", 
			      (char*) "DBVPEAK", (char*) "DBVRMS",
			      (char*) "VPEAK", (char*) "VRMS"};
    char cstring[64];
    DSA602 *pDSA602 = DSA602::GetThis();

    sprintf( cstring, "FFT FORMAT: %s", cformat[f]);
    return pDSA602->Command(cstring, NULL, 0);
}
bool DSAFFT::SetWindow(FFT_WINDOW w)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    static char *cwindow[] = {(char*)"BLACKMAN", (char*) "BLHARRIS",
			      (char*) "HAMMING", (char*) "HANNING",
			      (char*) "RECTANGULAR", (char*) "TRIANGULAR"};
    char cstring[64];
    DSA602 *pDSA602 = DSA602::GetThis();

    sprintf( cstring, "FFT WINDOW: %s", cwindow[w]);
    return pDSA602->Command(cstring, NULL, 0);
}


/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Fill up the structure from the DSA602 mainframe.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
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
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    bool rc = false;
    char Response[128];

    rc = pDSA602->Command("FFT?", Response, sizeof(Response));
    if (rc)
    {
	DecodeString(Response);
#if 0
	cout << "DSAFFTGPIB:" << Response << endl
	     << *this << endl;
#endif
    }
    return rc;
}

