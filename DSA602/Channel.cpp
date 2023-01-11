/********************************************************************
 *
 * Module Name : Channel.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
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
 * DSA602 programmers reference manual. 
 * This is a subclass to Module.cpp
 *
 *
 * 11A32 Two Channel Amplifier user supplement
 * Tidbits from the manual. 
 *
 * 50 or 1M ohm input
 * (Not sure how to set Volt/Div)
 * Volts/Div Offset Range Stepsize (through GPIB)
 * --------- ------------ --------
 * 1:99.5mV  +/- 1         25e-6
 * 100:995mV +/- 10       250e-6
 * 1:10V     +/- 100      2.5e-3
 *
 * Sensitivity defined as "Sets the deflection factor of the specified channel
 * Sensitivity is a channel-specific command which does not apply to compound
 * waveforms. 
 *
 * BWHI      <NRx>          HF Limit
 * ----      -----          --------
 *           <= 24e6          20e6
 *           >24e6 & <120e6  100e6
 *           >120e6          ??
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <string.h>
#include <stdlib.h>
#include <string>

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "Channel.hh"
#include "GParse.hh"
#include "DSA602.hh"
#include "DSA602_Utility.hh"

/* 
 * Command, type, upper bound, lower bound.
 * Put these in the same order as the enum Commands. 
 * The label here is a tiny bit redundant with Label in the header file. 
 * These are also grouped by 
 * first 2 - Non differential only
 * next 9 - applicable to differential only
 * Final  - applicable to all modules. 
 *
 * Command, type, upper, lower, step, boolean not yet used.
 */
const struct t_Commands Channel::ChannelCommands[kCHNL_END]= {
    {"COUpling",   kCT_COUPLING,    0.0,   0.0,  1.0, true},
    {"PROBe",      kCT_STRING,      0.0,   0.0,  1.0, false},
    {"AMPoffset",  kCT_FLOAT,      20.0, -20.0, 25.0e-6, false},
    {"PLSCoupling",kCT_COUPLING,    0.0,   0.0,  1.0, false},
    {"MNSCoupling",kCT_COUPLING,    0.0,   0.0,  1.0, false},
    {"PLSOffset",  kCT_FLOAT,       1.0,  -1.0, 25.0e-6, false},
    {"MNSOffset",  kCT_FLOAT,       1.0,  -1.0, 25.0e-6, false},
    {"PLSProbe",   kCT_STRING,      0.0,   0.0,  1.0, false},
    {"MNSProbe",   kCT_STRING,      0.0,   0.0,  1.0, false},
    {"PROTect",    kCT_BOOL,        0.0,   0.0,  1.0, false},
    {"VCOffset",   kCT_FLOAT,       0.0,   0.0,  1.0, true},     
    {"BW",         kCT_FLOAT,     2.0e7,   0.0,  1.0, false},
    {"IMPedence",  kCT_IMPEDANCE,   1e6,  50.0,  1.0, true},
    {"BWHi",       kCT_FLOAT,     3.0e6,   0.0,  1.0, true},
    {"OFFSet",     kCT_FLOAT,       0.0,   0.0,  1.0, true},
    {"BWLo",       kCT_FLOAT,     1.0e7,   0.0,  1.0, false},
    {"SENsitivity",kCT_FLOAT,       0.0,   0.0,  1.0, true},
    {"UNI",        kCT_STRING,      0.0,   0.0},
};



/*
 * offset depends on sensitivity per module basis. Page 83 
 * This is laid out as Low, High and stepsize for that range. 
 * I don't quite appreciate what is meant by sensitivity. 
 */
struct t_Sensitivity P11A32[] = {
    {  1e-3,   1.99e-3, 1e-6},
    {  2e-3,   4.98e-3, 20e-6},
    {  5e-3,   9.95e-3, 50e-6},
    { 10e-3,  19.9e-3, 100e-6},
    { 20e-3,  49.8e-3, 200e-6},
    { 50e-3,  99.5e-3, 500e-6},
    {100e-3,  199e-3,    1e-3},
    {200e-3, 498e-3,     2e-3},
    {500e-3, 995e-3,     5e-3},
    {   1.0,   1.99,    10e-3},
    {   2.0,   4.98,    20e-3},
    {   5.0,   10.0,    50e-3}
};
struct t_Sensitivity P11A33[] = {
    {  1.0e-3,   1.99e-3, 10.0e-6},
    {  2.0e-3,   4.98e-3, 20.0e-6},
    {  5.0e-3,   9.95e-3, 50.0e-6},
    { 10.0e-3,  19.9e-3, 100.0e-6},
    { 20.0e-3,  49.8e-3, 200.0e-6},
    { 50.0e-3,  99.5e-3, 500.0e-6},
    {100.0e-3, 199.0e-3,   1.0e-3},   // not available when impedence = 1e9
    {200.0e-3, 498.0e-3,   2.0e-3},
    {500.0e-3, 995.0e-3,   5.0e-3},
    {  1.0,      1.99,    10.0e-3},
    {  2.0,      4.98,    20.0e-3},
    {  5.0,     10.0,     50.0e-3}
};


/**
 ******************************************************************
 *
 * Function Name : Channel constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Channel::Channel(SLOT s, unsigned char n) : CObject()
{

    SetName("Channel");
    ClearError(__LINE__);

    fText        = new string("NONE");
    // Initalize all read variables to some default value.
    fAMPoffset   = 0.0;
    fBW          = 0.0;
    fBWHi        = 0.0;
    fBWLo        = 0.0;
    fCOUpling    = kCOUPLING_OFF;
    fIMPedance   = kFIFTY;
    fMNSCoupling = kCOUPLING_OFF;
    fMNSOffset   = 0.0;
    fMNSProbe    = strdup("NONE");
    fOFFSet      = 0.0;
    fPLSCoupling = kCOUPLING_OFF;
    fPLSOffset   = 0.0;
    fPLSProbe    = strdup("NONE");
    fPROBe       = strdup("NONE");
    fPROTect     = false;
    fSENsitivity = 0.0;
    fUNIts       = "NONE";
    fVCOffset    = 0.0;
    fDifferential = false;

    // Set parent slot
    fSlot = s;

    // Set channel number. 
    fNumber = n;

    Update();
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Channel destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Channel::~Channel()
{
    delete fText;
    delete fMNSProbe;
    delete fPLSProbe;
    delete fPROBe;
}

/**
 ******************************************************************
 *
 * Function Name : Applicable
 *
 * Description : Is this function applicable to this channel? 
 *
 * Inputs : from enum COMMANDS
 *
 * Returns : true if it is. 
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
bool Channel::Applicable(uint8_t c)
{
    SET_DEBUG_STACK;
    bool rc = false;
    if (fDifferential)
    {
	switch(c)
	{
	case kCCOUPLING:
	case kPROBE:
	    rc = false;
	    break;
	case kAMPOFFSET: 
	case kPLSCOUPLING:
	case kMNSCOUPLING: 
	case kPLSOFFSET:
	case kMNSOFFSET:  
	case kPLSPROBE: 
	case kMNSPROBE: 
	case kPROTECT: 
	case kVCOFFSET:
	case kBW: 
	case kIMPEDANCE: 
	case kBWHI: 
	case kOFFSET: 
	case kBWLO:
	case kSENSITIVITY:
	case kUNITS:
	    rc = true;
	    break;
	default:
	    rc = false;
	    break;
	}
    }
    else
    {
 	switch(c)
	{
	case kCCOUPLING:
	case kPROBE:
	    rc = true;
	    break;
	case kAMPOFFSET: 
	case kPLSCOUPLING:
	case kMNSCOUPLING: 
	case kPLSOFFSET:
	case kMNSOFFSET:  
	case kPLSPROBE: 
	case kMNSPROBE: 
	case kPROTECT: 
	case kVCOFFSET:
	    rc = false;
	    break;
	case kBW: 
	case kIMPEDANCE: 
	case kBWHI: 
	case kOFFSET: 
	case kBWLO:
	case kSENSITIVITY:
	case kUNITS:
	    rc = true;
	    break;
	default:
	    rc = false;
	    break;
	}
    }
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : AmpOffset
 *
 * Description : 
 *     Queries the voltages to be subtracted from the input signal, 
 *     after the plus and minus differentential input signals have been
 *     subratracted from each other. AMPoffset vertically positions 
 *     the signal on the display. APPLIES TO DIFFERENTIAL AMPS ONLY. 
 *
 *     CH<slot><ui><sp>AMP:<NRx>
 *     Example command "CHR1 AMP:1.0"
 * Inputs :
 *    q - query if false, just return the last value.
 *
 * Returns :
 *    The offset value or -DBL_MAX if the module is not differential. 
 *
 * Error Conditions : Module is not differential or GPIB error. 
 * 
 * Unit Tested on: 29-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double Channel::AmpOffset(bool q)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if(fDifferential) 
    {
	if(q) 
	{
	    Query(kAMPOFFSET); 
	}
	SET_DEBUG_STACK;
	return fAMPoffset;
    }
    // This represents an error condition
    SET_DEBUG_STACK;
    SetError(-1,__LINE__);
    return -DBL_MAX;
}

/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : 
 *     Query the amplifier coupling. Only applies to non-differential
 *     modules. EG: 11A32    
 *
 * Inputs :
 *   q - issue query if true, otherwise return value from last query. 
 *
 * Returns :
 *    COUPLING value
 *    {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
 *
 * Error Conditions : 
 *     GPIB error or differential amplifier. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
COUPLING Channel::Coupling(bool q) 
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (fDifferential)
    { 
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return kCOUPLING_NONE; 
    }
    if(q) 
    {
	Query(kCCOUPLING);
    }
    SET_DEBUG_STACK;
    return fCOUpling;
}


/**
 ******************************************************************
 *
 * Function Name : Channel << operator
 *
 * Description : format all the channel information for output
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const Channel &n)
{
    SET_DEBUG_STACK;
    const char space[] = "    ";
    output << "============================================" << endl
	   << "Channel: " << (int) n.fNumber << endl << space
	   << "AMPoffset: " << n.fAMPoffset << endl << space
	   << "BW: "        << n.fBW 
	   << " BWHi: "     << n.fBWHi 
	   << " BWLo: "     << n.fBWLo << endl << space
	   << "COUpling: ";
    switch (n.fCOUpling)
    {
    case kCOUPLING_AC:
	output << "AC "; 
	break;
    case kCOUPLING_DC:
	output << "DC "; 
	break;
    case kCOUPLING_OFF:
	output << "OFF "; 
	break;
    case kCOUPLING_VC:
	output << "VC "; 
	break;
    case kCOUPLING_NONE:
	break;
    }
    output << "IMPedance: ";
    switch (n.fIMPedance)
    {
    case kFIFTY:
	output << "50 Ohms";
	break;
    case kONE_MEG:
	output << "1 MOhm";
	break;
    case kONE_GIG:
	output << "1 GOhm";
	break;
    }
    if (n.fDifferential)
    { 
	output << " DIFFERENTIAL TRUE"  
	       << endl << space << "MNSCoupling: "
	       << n.CouplingString(n.fMNSCoupling)
	       << " MNSOffset: " << n.fMNSOffset
	       << " MNSProbe: " ;
	if (n.fMNSProbe != NULL)
	    output << n.fMNSProbe;
	else
	    output << "NONE";
	       
	output << endl << space
	       << "OFFSet: "  << n.fOFFSet
	       << endl << space
	       << "PLSCoupling: " 
	       << n.CouplingString(n.fPLSCoupling)
	       << "PLSOffset: " << n.fPLSOffset
	       << " PLSProbe: "; 
	if (n.fPLSProbe != NULL)
	    output << n.fPLSProbe;
	else
	    output << "NONE";
    }
    output<< endl << space
	  << "PROBe: ";   
    if (n.fPROBe != NULL) 
	output << n.fPROBe;
    else
	output << "NONE";
    output << endl << space
	   << "PROTect: ";
    if (n.fPROTect)
    {
	output << "ON ";
    }
    else
    {
	output << "OFF";
    }
    output << endl << space;
    output << "SENsitivity: " << n.fSENsitivity
	   << endl << space
	   << "UNIts: "    << n.fUNIts
	   << endl << space
	   << "VCOffset: "   << n.fVCOffset
	   << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : Decode the string returned from the query CH?
 *
 * Example read
 * CHL1 MNSCOUPLING:DC,PLSCOUPLING:DC,PROTECT:OFF,OFFSET:0.0E+0,AMPOFFSET:0.0E+0,BW:2.0E+7,IMPEDANCE:5.0E+1,MNSOFFSET:0.0E+0,MNSPROBE:"NONE",PLSOFFSET:0.0E+0,PLSPROBE:"NONE",SENSITIVITY:1.0E+0,UNITS:"VOLTS",VCOFFSET:0.0E+0;     
 *
 * Inputs : string to decode in its consituients
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Channel::Decode(const char *val)
{
    SET_DEBUG_STACK;
    const char *p = NULL;
    char tmp[4];

    GParse par(val);
    if (strncmp(val,"CH",2) == 0)
    {
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, &val[2], 2);
    }
    if ((p = par.Value("AMPOFFSET")) != NULL)
    {
	fAMPoffset = atof(p);
    }
    if ((p = par.Value("BW")) != NULL)
    {
	fBW = atof(p);
    }
    if ((p = par.Value("BWHI")) != NULL)
    {
	fBWHi = atof(p);
    }
    if ((p = par.Value("BWLO")) != NULL)
    {
	fBWLo = atof(p);
    }
    if ((p = par.Value("COUPLING")) != NULL)
    {
	fCOUpling = DecodeCoupling(p);
    }
    if ((p = par.Value("IMPEDANCE")) != NULL)
    {
	fIMPedance = DecodeImpedance(p);
    }
    if ((p = par.Value("MNSCOUPLING")) != NULL)
    {
	fMNSCoupling = DecodeCoupling(p);
    }
    if ((p = par.Value("MNSOFFSET")) != NULL)
    {
	fMNSOffset = atof(p);
    }
    if ((p = par.Value("MNSPROBE")) != NULL)
    {
	delete fMNSProbe;
	fMNSProbe = strdup(p);
    }
    if ((p = par.Value("OFFSET")) != NULL)
    {
	fOFFSet = atof(p);
    }
    if ((p = par.Value("PLSCOUPLING")) != NULL)
    {
	fPLSCoupling = DecodeCoupling(p);
    }
    if ((p = par.Value("PLSOFFSET")) != NULL)
    {
	fPLSOffset = atof(p);
    }
    if ((p = par.Value("PLSPROBE")) != NULL)
    {
	delete fPLSProbe;
	fPLSProbe = strdup(p);
    }
    if ((p = par.Value("PROBE")) != NULL)
    {
	delete fPROBe;
	fPROBe = strdup(p);
    }
    if ((p = par.Value("PROTECT")) != NULL)
    {
	fPROTect = false;
	if (strncmp(p, "ON",2) == 0) fPROTect = true;
    }
    if ((p = par.Value("SENSITIVITY")) != NULL)
    {
	fSENsitivity = atof(p);
    }
    if ((p = par.Value("UNITS")) != NULL)
    {
	fUNIts = string(p);
    }
    if ((p = par.Value("VCOFFSET")) != NULL)
    {
	fVCOffset = atof(p);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : DecodeCoupling
 *
 * Description : given a string containing coupling parameters
 *               decode the string into one of the enums in COPULING
 *
 * Inputs : 
 *   p - string to be decoded, looks something like CHL1 COU:DC
 *
 * Returns : COUPLING enum associated with the decode
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
COUPLING Channel::DecodeCoupling(const char *p)
{
    SET_DEBUG_STACK;
    COUPLING rv = kCOUPLING_OFF;
    if (strncmp(p,"AC",2)==0)
    {
	rv = kCOUPLING_AC;
    }
    else if (strncmp(p,"DC",2)==0)
    {
	rv = kCOUPLING_DC;
    }
    else if (strncmp(p,"OF",2)==0)
    {
	rv = kCOUPLING_OFF;
    }
    else if (strncmp(p,"VC",2)==0)
    {
	rv = kCOUPLING_VC;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : DecodeImpedance
 *
 * Description : given a string containing impedence parameters
 *               decode the string into one of the enums in IMPEDENCE
 *
 * Inputs :
 *   p - string to be decoded, looks something like CHL1 IMP:50
 *
 * Returns : 
 *    IMPEDENCE enum value associated with channel
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
IMPEDANCE Channel::DecodeImpedance(const char *p)
{
    SET_DEBUG_STACK;
    IMPEDANCE rv = kFIFTY;
    double x = atof(p);
    if (x==50.0)
    {
	rv = kFIFTY;
    }
    else if (x==1e6)
    {
	rv= kONE_MEG;
    }
    else if (x==1.0e9)
    {
	rv = kONE_GIG;
    }
    else 
    {
	cout << "Channel, unknown impedance." << endl;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : CouplingString
 *
 * Description : 
 *     given a COUPLING enum value, return the associated string
 *
 * Inputs :
 *     COUPLING 
 *
 * Returns : 
 *    String value assocated with that enum value.
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* Channel::CouplingString(COUPLING c) const
{
    SET_DEBUG_STACK;
    const char *p;
    switch (c)
    {
    case kCOUPLING_AC:
	p = "AC "; 
	break;
    case kCOUPLING_DC:
	p = "DC "; 
	break;
    case kCOUPLING_OFF:
	p = "OFF "; 
	break;
    case kCOUPLING_VC:
	p = "VC "; 
	break;
    case kCOUPLING_NONE:
	p = "NONE";
    }
    SET_DEBUG_STACK;
    return p;
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : Send the specified command - Floating point value
 *
 * Inputs : 
 *    c - command to execute
 *    value - floating point value to set. 
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *    Either the specified value is out of bounds, or the GPIB command
 *    failed
 * 
 * Unit Tested on: 29-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Channel::SendCommand(COMMANDs c, double value)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool     rc     = false;
    char cstring[32];
    ClearError(__LINE__);

    if(log->CheckVerbose(1))
    {
	log->Log("# Channel::SendCommand: %d %f\n", (int)c, value);
    }

    // Has to be a float. 
    if (ChannelCommands[c].Type == kCT_FLOAT)
    {
	cout << ChannelCommands[c].Command << " "
	     << ChannelCommands[c].Lower << " "
	     << ChannelCommands[c].Upper
	     << endl;
	// Check bounds. 
	if((value<ChannelCommands[c].Lower)||(value>ChannelCommands[c].Upper))
	{
	    SET_DEBUG_STACK;
	    SetError(-1,__LINE__);
	    return false;
	}

	sprintf( cstring, "CH%c%d %s:%g",SlotChar(fSlot),
		 fNumber+1, ChannelCommands[c].Command, value);

	if(log->CheckVerbose(1))
	{
	    log->Log("# Channel::SendCommand: %s\n", cstring);
	}

	rc = pDSA602->Command(cstring, NULL, 0);
    }

    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : 
 *    boolean value format
 *
 * Inputs :
 *    c - command to send
 *    true/false = ON/OFF
 *
 * Returns :
 *     true on success
 *
 * Error Conditions :
 *     either the command specified wasn't a bool or the GPIB
 *     command failed. 
 * 
 * Unit Tested on: 28-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Channel::SendCommand(COMMANDs c, bool value)
{
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    const char *p;
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (ChannelCommands[c].Type == kCT_BOOL)
    {
	if (value)
	{
	    p = "ON";
	}
	else
	{
	    p = "OFF";
	}
	sprintf( cstring, "CH%c%d %s:%s",SlotChar(fSlot),
		 fNumber+1, ChannelCommands[c].Command, p);

	if(log->CheckVerbose(1))
	{
	    log->Log("# Channel::SendCommand (bool): %s\n", cstring);
	}
	rc = pDSA602->Command(cstring, NULL, 0);
	if (!rc)
	{
	    SET_DEBUG_STACK;
	    SetError(-1,__LINE__);
	    return false;
	}
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : IMPEDENCE VERSION
 *
 * Inputs :
 *    c - command to send
 *    IMPEDENCE enum 
 *
 * Returns :
 *     true on success
 *
 * Error Conditions : 
 *    for 11A33 amplifier when MNSCOUPLING is set to AC then only 
 *    50 and 1M are permitted. 
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Channel::SendCommand(COMMANDs c, IMPEDANCE value)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool rc = false;
    double val;
    char cstring[32];
    ClearError(__LINE__);

    if (ChannelCommands[c].Type == kCT_IMPEDANCE)
    {
	switch (value)
	{
	case kFIFTY:
	    val = 50.0;
	    break;
	case kONE_MEG:
	    val = 1.0e6;
	    break;
	case kONE_GIG:
	    val = 1.0e9;
	    break;
	default:
	    return false;
	}
	sprintf( cstring, "CH%c%d IMP:%f",SlotChar(fSlot),
		 fNumber+1, val);
	if(log->CheckVerbose(1))
	{
	    log->Log("# Channel::SendCommand (IMP): %s\n", cstring);
	}

	rc = pDSA602->Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return rc;    
}
/**
 ******************************************************************
 *
 * Function Name : SendCommand
 *
 * Description : 
 *    COUPLING value format
 *
 * Inputs :
 *    c - command to send
 *    one of the enum values from COUPLING
 *
 * Returns :
 *     true on success
 *
 * Error Conditions :
 *     either the command specified wasn't a COUPLING or the GPIB
 *     command failed. 
 * 
 * Unit Tested on: 28-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Channel::SendCommand(COMMANDs c, COUPLING  value)
{
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger* log    = CLogger::GetThis();
    bool rc = false;
    char cstring[32];
    const char *cs = NULL;
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (ChannelCommands[c].Type == kCT_COUPLING)
    {
	switch (value)
	{
	case kCOUPLING_AC:
	    cs = "AC";
	    break;
	case kCOUPLING_DC:
	    cs = "DC";
	    break;
	case kCOUPLING_OFF:
	    cs = "OFF";
	    break;
	case kCOUPLING_VC:
	    cs = "VC";
	    break;
	default:
	    return false;
	}
	sprintf( cstring, "CH%c%d %s:%s",
		 SlotChar(fSlot),
		 fNumber+1, 
		 ChannelCommands[c].Command,
		 cs);

	if(log->CheckVerbose(1))
	{
	    log->Log("# Channel::SendCommand (COUPLING): %s\n", cstring);
	}
	rc = pDSA602->Command(cstring, NULL, 0);
    }
    return rc;    
}

/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Update to current values of the channel based on slot
 *               and channel number.   
 *
 * Inputs :
 *     NONE
 *
 * Returns :
 *     true on success
 *
 * Error Conditions :
 *     GPIB communication failed. 
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Channel::Update(void)
{
    SET_DEBUG_STACK;
    DSA602*  pDSA602 = DSA602::GetThis();
    CLogger* log     = CLogger::GetThis();
    bool rc = false;
    char cstring[32],response[1024];
    char pSlot;
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    switch (fSlot)
    {
    case kSLOT_LEFT:
	pSlot = 'L';
	break;
    case kSLOT_CENTER:
	pSlot = 'C';
	break;
    case kSLOT_RIGHT:
	pSlot = 'R';
	break;
    case kSLOT_NONE:
	pSlot = ' ';
	break;
    }

    sprintf(cstring, "CH%c%d?", pSlot,fNumber+1);
    memset(response, 0, sizeof(response));
    rc = pDSA602->Command(cstring, response, sizeof(response));
    if(log->CheckVerbose(1))
    {
	log->Log("# Channel::Update %s\n", response);
    }

    if (rc)
    {
	Decode(response);
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
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
bool Channel::Query(COMMANDs c)
{
    SET_DEBUG_STACK;
    char   cstring[32], Response[64];
    const  char *p;
    DSA602 *pDSA602 = DSA602::GetThis();
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    // Query for specific data.
    sprintf( cstring, "CH%c%d? %s",SlotChar(fSlot),
	     fNumber+1, ChannelCommands[c].Command);
    //cout << "----Channel Query: " << cstring ;

    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	delete fText;
	fText = new string(Response);

	//cout << " RESPONSE: " << Response << endl;
	// Parse the command into value and command subset using GParse
	GParse par(Response);

	// If there is a value then proceed. 
	if ((p = par.Value(ChannelCommands[c].Command)) == NULL)
	{
	    SET_DEBUG_STACK;
	    SetError(-3,__LINE__);
	    return false;
	}
	switch(c)
	{
	case kAMPOFFSET:
	    fAMPoffset = atof(p);
	    break;
	case kBW: 
	    fBW = atof(p);
	    break;
	case kBWHI: 
	    fBWHi = atof(p);
	    break;
	case kBWLO: 
	    fBWLo = atof(p);
	    break;
	case kCCOUPLING:
	    fCOUpling = DecodeCoupling(p);
	    break;
	case kIMPEDANCE:
	    fIMPedance = DecodeImpedance(p);
	    break;
	case kMNSCOUPLING:
	    fMNSCoupling = DecodeCoupling(p);
	    break;
	case kMNSOFFSET: 
	    fMNSOffset = atof(p);
	    break;
	case kMNSPROBE: 
	    // Not implemented
	    break;
	case kOFFSET: 
	    fOFFSet = atof(p);
	    break;
	case kPLSCOUPLING:
	    fPLSCoupling = DecodeCoupling(p);
	    break;
	case kPLSOFFSET: 
	    fPLSOffset = atof(p);
	    break;
	case kPLSPROBE:
	    // Not implemnted
	    break;
	case kPROBE:
	    // Not implemnted
	    break;
	case kPROTECT:
	    if (strstr(p,"ON")) 
		fPROTect = true;
	    else
		fPROTect = false;
	    break;
	case kSENSITIVITY: 
	    fSENsitivity = atof(p);
	    break;
	case kUNITS: 
	    fUNIts = string(p);
	    break;
	case kVCOFFSET:
	    fVCOffset = atof(p);
	    break;
	default:
	    SET_DEBUG_STACK;
	    SetError(-2,__LINE__);
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
