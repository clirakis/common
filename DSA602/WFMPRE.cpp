/********************************************************************
 *
 * Module Name : WFMPRE.cpp
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
#include "DSA602.hh"
#include "DSA602_Utility.hh"
#include "WFMPRE.hh"
#include "Units.hh"
#include "GParse.hh"
#include "CLogger.hh"

/* Command, type, upper bound, lower bound */
const struct t_Commands WFMPRE::WFMPRECommands[kENDCOUNT+1]= {
    {"ACSTATE",   kCT_BOOL,      0.0,   0.0},  // Amplifier offset
    {"BIT",       kCT_BOOL,      0.0,   0.0},
    {"BN.",       kCT_BOOL,      0.0,   0.0},
    {"BYT/" ,     kCT_BOOL,      0.0,   0.0},
    {"BYT.",      kCT_BOOL,      0.0,   0.0},
    {"CRVCHK",    kCT_INT,       0.0,   0.0},
    {"DATE",      kCT_STRING,    0.0,   0.0},
    {"ENCDG",     kCT_BOOL,      0.0,   0.0},
    {"LAB",       kCT_STRING,    0.0,   1.0},
    {"NR.",       kCT_INT,   32768.0,   1.0},
    {"PT.",       kCT_INT,       2.0,   0.0},
    {"TIM",       kCT_STRING,    0.0,   0.0},
    {"TST",       kCT_FLOAT,     0.0,   0.0},
    {"WFID",      kCT_STRING,    0.0,   0.0},
    {"XIN",       kCT_FLOAT,     0.0,   0.0},
    {"XMU",       kCT_FLOAT,     1.0,  -1.0},
    {"XUNIT",     kCT_INT,       0.0,   0.0},
    {"XZERO",     kCT_FLOAT,  1.0e15,-1.0e15},
    {"YMULT",     kCT_FLOAT,     0.0,   0.0},
    {"YUNIT",     kCT_FLOAT,    10.0,   1.0e-3},
    {"YZERO",     kCT_FLOAT,     0.0,   0.0},
    {NULL,         kCT_NONE,     0.0,   0.0},
};

/**
 ******************************************************************
 *
 * Function Name : WFMPRE constructor
 *
 * Description : Decode the waveform prefix data. 
 *
 * Inputs : A string that is returned from a WFMPRE? command. 
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
WFMPRE::WFMPRE(const char *val) : CObject()
{
    SET_DEBUG_STACK;
    SetName("WFMPRE");
    ClearError(__LINE__);
    fText = new string("NONE");
    Reset();
    if (val!=NULL) Decode(val);
}

/**
 ******************************************************************
 *
 * Function Name : Reset
 *
 * Description : Set all known values to false or none. 
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 24-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void WFMPRE::Reset(void)
{
    SET_DEBUG_STACK;
    fACState = false;
    fBIT     = false;
    fBYTE    = false;
    fBYTor   = false;
    fCRVchk  = 0;
    //fDate    = NULL;
    fENCdg   = false;
    //fLABel   = NULL;
    fNRpt    = 0;
    fPTfmt   = kPT_NONE;   
    //fTime    = NULL;
    fTSTime  = 0.0;  
    //fWFId    = NULL;
    fXINcr   = 0.0;
    fXMUlt   = 0.0;
    fXUNit   = kUNITSNONE;
    fXZEro   = 0.0;
    fYMUlt   = 0.0;
    fYUNit   = kUNITSNONE;
    fYZEro   = 0.0;
}

/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : Decode the waveform prefix data. 
 *
 * Inputs : A string that is returned from a WFMPRE? command. 
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
bool WFMPRE::Decode(const char *val)
{
    SET_DEBUG_STACK;
    const char *p = NULL;
    ClearError(__LINE__);

    /* Example read
     * 'WFMPRE ACSTATE:NENHANCED,BIT/NR:16,BN.FMT:RI,BYT/NR:2,BYT.OR:MSB,
     *  CRVCHK:NULL,ENCDG:BINARY,NR.PT:512,PT.FMT:Y,WFID:TRACE1,
     * XINCR:1.0E-8,XMULT:1.5625E-4,XUNIT:SECONDS,XZERO:-1.06E-6,
     * YMULT:1.5625E-5,YUNIT:VOLTS,YZERO:2.35E-1,LABEL:"",
     * TIME:"19:39:49.26",DATE:" 2-FEB-14",TSTIME:1.6894E-8'
     */
    GParse par(val);

    if ((p = par.Value("ACSTATE")) != NULL)
    {
	if (strncasecmp( p, "ENH", 3) == 0)
	{
	    fACState = true;
	}
    }
    if ((p = par.Value("BIT/NR")) != NULL)
    {
	fBIT = (atoi(p) == 16);
    }
    if ((p = par.Value("BN")) != NULL)
    {
	if (strstr(p, "RI"))
	    fBN = true;
	else
	    fBN = false;
    }

    if ((p = par.Value("BYT/NR")) != NULL)
    {
	fBYTE = (atoi(p) == 2);
    }
    if ((p = par.Value("BYT.or")) != NULL)
    {
	if (strncasecmp( p, "MSB", 3) == 0)
	{
	    fBYTor = true;
	}
	else
	{
	    fBYTor = false;
	}
    }
    if ((p = par.Value("CRVchk")) != NULL)
    {
	fCRVchk = -1;
	if (strncasecmp( p, "CHK", 3) == 0)
	{
	    fCRVchk = 0;
	}
	else if(strncasecmp( p, "NON", 3) == 0)
	{
	    fCRVchk = 1;
	}
	else if(strncasecmp( p, "NUL", 3) == 0)
	{
	    fCRVchk = 2;
	}
    }
    if ((p = par.Value("DATE")) != NULL)
    {
	// FORMAT HOSED
	fDate = strdup(p);
    }
    if ((p = par.Value("ENCDG")) != NULL)
    {
	if (strncasecmp( p, "BINA", 4) == 0)
	{
	    fENCdg = true;
	}
    }
    if ((p = par.Value("LABEL")) != NULL)
    {
	fLABel = strdup(p);
    }
    if ((p = par.Value("NR.PT")) != NULL)
    {
	fNRpt = atoi(p);
    }
    if ((p = par.Value("PT.FMT")) != NULL)
    {
	//cout << "PT.fmt is : " << p << endl;
	if (strncasecmp( p, "ENV", 3) == 0)
	{
	    fPTfmt = kPT_ENV;
	}
	else if (strncasecmp( p, "XY", 2) == 0)
	{
	    fPTfmt = kPT_XY;
	}
	else if (strncasecmp( p, "Y", 1) == 0)
	{
	    fPTfmt = kPT_Y;
	}
    }
    if ((p = par.Value("TIME")) != NULL)
    {
	fTime = strdup(p);
    }
    if ((p = par.Value("TSTIME")) != NULL)
    {
	fTSTime = atof(p);
    }
    if ((p = par.Value("WFID")) != NULL)
    {
	fWFId = strdup(p);
    }
    if ((p = par.Value("XINCR")) != NULL)
    {
	fXINcr = atof(p);
    }
    if ((p = par.Value("XMULT")) != NULL)
    {
	fXMUlt = atof(p);
    }
    if ((p = par.Value("XUNIT")) != NULL)
    {
	fXUNit = DecodeUnits(p);
    }
    if ((p = par.Value("XZERO")) != NULL)
    {
	fXZEro = atof(p);
    }
    if ((p = par.Value("YMULT")) != NULL)
    {
	 fYMUlt = atof(p);
    }
    if ((p = par.Value("YUNIT")) != NULL)
    {
	fYUNit = DecodeUnits(p);
    }
    if ((p = par.Value("YZERO")) != NULL)
    {
	fYZEro = atof(p);
    }
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : WFMPRE destructor
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
WFMPRE::~WFMPRE()
{
    SET_DEBUG_STACK;
    delete fText;
}
/**
 ******************************************************************
 *
 * Function Name : operator << 
 *
 * Description : specifically for WFMPRE
 *
 * Inputs : 
 *    output stream to insert data into. 
 *    WFMPRE - WFMPRE information to parse into output. 
 *
 * Returns :
 *    formatted output stream
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 23-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const WFMPRE &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "WFMPRE: "  << endl 
	   << "ACState: ";
    if(n.fACState)
    {
	output << "Enhanced";
    }
    else
    {
	output << "Enhanced";
    }
    output << " BIT: "; 
    if(n.fBIT)
    {
	output << "16";
    }
    else
    {
	output << " 8";
    }
    output << " BYTE: ";
    if ( n.fBYTE)
    {
	output << "2";
    }
    else
    {
	output << "1";
    }
    output << " BYTor: ";
    if (n.fBYTor)
    {
	output << "MSB";
    }
    else
    {
	output << "LSB";
    }
    output << endl
	   << "CRVchk: "  << (int) n.fCRVchk
	   << " Date: "   << n.fDate
	   << " ENCdg: ";
    if(n.fENCdg)
    {
	output << "BINARY";
    }
    else
    {
	output << "ASCII";
    }
    output << " Label: "  << n.fLABel << endl
	   << "NRpt: "    << n.fNRpt
	   << " PTfmt: "  << PT_String(n.fPTfmt)
	   << " Time: "   << n.fTime
	   << " TSTime: " << n.fTSTime << endl
	   << "WFID: "    << n.fWFId << endl
	   << "XINcr: "   << n.fXINcr
	   << " XMult: "  << n.fXMUlt
	   << " XUnit: "  << UnitString(n.fXUNit) 
	   << " XZEro: "  << n.fXZEro << endl
	   << "YMult: "   << n.fYMUlt
	   << " YUnit: "  << UnitString(n.fYUNit)
	   << " YZEro: "  << n.fYZEro << endl
	   << "============================================" << endl;

    SET_DEBUG_STACK;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : Bytes
 *
 * Description : returns the binary data field width. Either 
 * one or two bytes per binary waveform point. (See more on page 70)
 * QUERY ONLY.
 *
 * Inputs : q - perform the query or just use existing value. 
 *
 * Returns : number of bytes {1,2}
 *
 * Error Conditions : 
 *     GPIB fails
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned char WFMPRE::Bytes(bool q) 
{
    SET_DEBUG_STACK;
    if (q)
    {
	Query(kBYTE);
    }
    if(fBYTE) 
    {
	return 2; 
    }
    else 
    {
	return 1;
    }
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : ScaleY
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
double WFMPRE::ScaleY(double y)
{
    SET_DEBUG_STACK;
    double rv = 0.0;
    switch (fPTfmt)
    {
    case kPT_Y:   // Y
    case kPT_XY:  // XY
 	rv = fYMUlt * y + fYZEro;
	break;
    case kPT_ENV:
    case kPT_NONE:
	// Not currently covered.
	break;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : ScaleY
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
double WFMPRE::ScaleX(double x)
{
    SET_DEBUG_STACK;
    double rv = 0.0;
    switch (fPTfmt)
    {
    case kPT_Y:   // Y
	rv = fXINcr * x + fXZEro;
	break;
    case kPT_XY: // XY
	rv = fXMUlt * x + fXZEro;
	break;
    case kPT_ENV:
    case kPT_NONE:
	// Not currently covered.
	break;
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : ScaleY
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
double WFMPRE::ScaleXY( int i, short *DataIn, double &y)
{
    SET_DEBUG_STACK;
    double x;
    size_t ix, iy;  // Count variable. 

    switch (fPTfmt)
    {
    case kPT_Y:   // Y
	iy = i;
	ix = i;
	x = fXINcr * (double) ix         + fXZEro;
 	y = fYMUlt * (double) DataIn[iy] + fYZEro;
	break;
    case kPT_XY: // XY
	ix = 2*i;
	iy = 2*i+1;
	x = fXMUlt * (double) DataIn[ix] + fXZEro;
	y = fYMUlt * (double) DataIn[iy] + fYZEro;
	break;
    case kPT_ENV:
    case kPT_NONE:
	// Not currently covered.
	break;
    }
    SET_DEBUG_STACK;
    return x;
}
/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Send the command to get the waveform prefix data. 
 *               This subsequently decodes it and puts it into
 *               the WFMPRE class structure. 
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 14-Dec-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::Update(void)
{
    SET_DEBUG_STACK;
    DSA602  *pDSA602 = DSA602::GetThis();
    CLogger *log     = CLogger::GetThis();
    ClearError(__LINE__);
    bool rc         = false;
    char Response[1024];

    // Send command
    rc = pDSA602->Command("WFMPRE?", Response, sizeof(Response));
    if (rc)
    {
	// Save a copy of the response. 
	delete fText;
	fText = new string(Response);
	// Decode the response. 
	Decode(Response);
	if(log->CheckVerbose(1))
	{
	    log->Log("# WFMPRE Update Response: %s\n", Response);
	}
#if 0
	cout << "WFMPRE_GPIB:" << Response << endl
	     << *this << endl;
#endif
    }
    SET_DEBUG_STACK;
    return rc;
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
bool WFMPRE::Query(COMMANDs c)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char   cstring[32], Response[64];
    DSA602 *pDSA602 = DSA602::GetThis();
    CLogger *log    = CLogger::GetThis();

    // Query for specific data.
    sprintf( cstring, "WFM? %s", WFMPRECommands[c].Command);
//    cout << "----WFMPRE Query: " << cstring ;

    if(pDSA602->Command(cstring, Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# WFMPRE Query Response: %s\n", Response);
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
 * Function Name : ACState
 *
 * Description : helper funciton to set the ACState value
 *
 * Inputs :
 *    enh - if true set to enhanced. 
 *
 * Returns :
 *    true on success.
 * 
 * Error Conditions :
 *    GPIB error
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetACState(bool enh)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    const char *p;
    DSA602 *pDSA602 = DSA602::GetThis();

    if (enh)
    {
	p = "WFM ACS:ENH";
    }
    else
    {
	p = "WFM ACS:NEN";
    }
    cout << " SETAC NOT WORKING" << p << endl;
    return pDSA602->Command(p, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : DATE
 *
 * Description : 
 *     Set the date on the current waveform as the current computer
 *     date
 *
 * Inputs :
 *    NONE
 * 
 * Returns :
 *    true if successful
 *
 * Error Conditions :
 *    GPIB failure
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetDATE(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    time_t now;
    time(&now);
    const struct tm *local = localtime(&now); 
    strftime( cmd, sizeof(cmd), "WFM DATE:%d-%b-%y", local);

    //cout << " SET DATE " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Label
 *
 * Description : Set the label on the current trace. 
 *
 * Inputs : 
 *     character label to use, limited to 10 characters
 *
 * Returns :
 *    true on success
 *  
 *
 * Error Conditions :
 *     GPIB failure. 
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::Label(const char *label)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32], tmp[16];
    DSA602 *pDSA602 = DSA602::GetThis();

    strncpy( tmp, label, 10);
    snprintf( cmd, sizeof(cmd), "WFM LAB:\'%s\'", tmp);
    //cout << " SET LABEL " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetNumberPoints
 *
 * Description :
 *     Set Number of points in the waveform record. 
 * 
 * Inputs :
 *     Number of points, specified in bit length
 *     a power of 2 in the set {512:32768}
 * 
 * Returns :
 *
 * Error Conditions :
 *    GPIB write error, or the number of points requested is out of bounds. 
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetNumberPoints(unsigned int p)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    DSA602 *pDSA602 = DSA602::GetThis();
    if ((p<9) || (p>15))
    {
	SET_DEBUG_STACK;
	SetError(-1, __LINE__);
	return false;
    }
    unsigned int pts = pow(2.0, (double) p);

    snprintf( cmd, sizeof(cmd), "WFM NR.:%d", pts);
    //cout << " SET number points " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : PointFormat
 *
 * Description : 
 *     Set the point format, one of {ENV,XY,Y}
 *
 * Inputs :
 *     p - {kPT_ENV=0, kPT_XY, kPT_Y, kPT_NONE}
 * Returns :
 *
 * Error Conditions :
 *     GPIB fails or requested format out of bounds. 
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::PointFormat(PT_TYPES p)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    const char *ptfmt;
    DSA602 *pDSA602 = DSA602::GetThis();

    switch (p)
    {
    case kPT_ENV:
	ptfmt = "ENV";
	break;
    case kPT_XY:
	ptfmt = "XY";
	break;
    case kPT_Y:
	ptfmt = "Y";
	break;
    case kPT_NONE:
    default:
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false;
    }

    snprintf( cmd, sizeof(cmd), "WFM PT.:%s", ptfmt);
    //cout << " SET waveform format" << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetTime
 *
 * Description : 
 *     Set the time on the current waveform as the current computer
 *     date
 *
 * Inputs :
 *    NONE
 * 
 * Returns :
 *    true if successful
 *
 * Error Conditions :
 *    GPIB failure
 * 
 * Unit Tested on: 30-Jan-21 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetTime(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char fmt1[32], cmd[64];
    DSA602 *pDSA602 = DSA602::GetThis();
    struct timespec now;
    // Permits to get beyond 1 second resoluton. 
    clock_gettime( CLOCK_REALTIME, &now);

    const struct tm *local = localtime(&now.tv_sec);
    unsigned int dsec = now.tv_nsec/1e7;
    strftime( fmt1, sizeof(fmt1), "%H:%M:%S", local);
    sprintf( cmd, "WFM TIM:\'%s.%d\'", fmt1, dsec);
    //cout << " SET TIME " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetIncrement
 *
 * Description : 
 *     Helper function used by XINCR and YINCR functions. 
 *     set the incement, minimum value = 1.0e-12 
 *
 * Inputs :
 *   X = True if X 
 *   val - seconds/point
 *
 * Returns :
 *    true if successful
 *
 * Error Conditions :
 *    GPIB failure
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::XIncrement(double val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    DSA602 *pDSA602 = DSA602::GetThis();

    if (val<1.0e-12) 
    {
	SET_DEBUG_STACK;
	SetError(-1, __LINE__);
	return false;
    }

    sprintf( cmd, "WFM XIN:%G", val);
    //cout << " SET X INCREMENT " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}

/**
 ******************************************************************
 *
 * Function Name : SetUnits
 *
 * Description :
 *   Helper function to set Units for X and Y
 *
 * Inputs :
 *   X = true if for x axis. 
 *   one of the values in the enum
 *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
 *		kWATT, kUNITSNONE};
 *
 * Returns :
 *
 * Error Conditions :
 *    GPIB failure or requested unit out of range. 
 * 
 * Unit Tested on: 30-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetUnits(bool X, UNITS u)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    const char *p, *q;
    DSA602 *pDSA602 = DSA602::GetThis();
   
    if (X)
    {
	p = "XUN:";
    }
    else
    {
	p = "YUN:";
    }

    switch(u)
    {
    case kAMPS:
	q = "AMPS";
	break;
    case kDB:
	q = "DB";
	break;
    case kDEGREES:
	q = "DEG";
	break; 
    case kDIVS:
	q = "DIVS";
	break; 
    case kHERTZ:
	q = "HER";
	break;
    case kOHMS:
	q = "OHM";
	break; 
    case kSECONDS:
	q = "SEC";
	break;
    case kVOLT:
	q = "VOL";
	break;
    case kWATT:
	q = "WAT";
	break; 
    case kUNITSNONE:
    default:
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false;
	break;
    }

    sprintf( cmd, "WFM %s%s", p,q);
    //cout << " SET UNITS " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetZero
 *
 * Description :
 *   Helper function to set Zero for X and Y
 *
 * Inputs :
 *   X = true if for x axis. 
 *   val - {-1e15:1e15}
 *
 * Returns :
 *  true on success
 *
 * Error Conditions :
 *    GPIB error or value out of bounds. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool WFMPRE::SetZero(bool X, double val )
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cmd[32];
    const char *p;
    DSA602 *pDSA602 = DSA602::GetThis();
   
    if (fabs(val) > 1.0e15)
    {
	SET_DEBUG_STACK;
	SetError(-1,__LINE__);
	return false;
    }
    if (X)
    {
	p = "XZE:";
    }
    else
    {
	p = "YZE:";
    }

    sprintf( cmd, "WFM %s%G", p,val);
    //cout << " SET ZERO " << cmd << endl;
    return pDSA602->Command(cmd, NULL, 0);
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

#ifdef DEBUG_WFM
void WFMPRE::Test(void)
{
    SET_DEBUG_STACK;

//     cout << "ACSTATE: " << endl
// 	 << "   QUERY: " << ACState(true) << endl;
//     cout << "   SET nenh " << SetACState(false) << endl;
//     cout << "   QUERY: " << ACState(true) << endl;
//     cout << "   SET enh " << SetACState(true) << endl;
//     cout << "   QUERY: " << ACState(true) << endl;

//     cout << "   BIT: " << BIT(true) << endl;
//     cout << "   BN: "  << BN(true)  << endl;
//     cout << "   Bytes: "  << (int) Bytes(true)  << endl;
//     cout << "   Byte Order: "  << (int) ByteOrder(true)  << endl;
//     cout << "   Checksum type: "  << (int) CRVchk(true)  << endl;
//     cout << "   waveform date: "  << DATE(true)  << endl;
//     cout << "   set waveform date: "  << SetDATE()  << endl;
//     cout << "   waveform date: "  << DATE(true)  << endl;
//     cout << "   waveform encoding: "  << Encoding(true)  << endl;
//      cout << "   waveform label: "  << Label(true)  << endl;
//      cout << "   Set waveform label: "  << Label("MINE")  << endl;
//      cout << "   waveform label: "  << Label(true)  << endl;
//      cout << "   number points: "  << NumberPoints(true)  << endl;
//      cout << "   Set number points: "  << SetNumberPoints(10)  << endl;
//      cout << "   number points: "  << NumberPoints(true)  << endl;
//      cout << "   point format: "  << (int) PointFormat(true)  << endl;
//      cout << "   Set point format: " << PointFormat(kPT_ENV) << endl;
//      cout << "   point format: "  << (int) PointFormat(true)  << endl;

//      cout << "   waveform time: "  << Time(true)  << endl;
//      cout << "   Set waveform time: "  << SetTime()  << endl;
//      cout << "   waveform time: "  << Time(true)  << endl;
//     cout << "   waveform trigger time: "  << TSTime(true)  << endl;
//     cout << "   waveform id: "  << WaveformID(true)  << endl;
//     cout << "   X time increment: "  << XIncrement(true)  << endl;
//     cout << " Set  X time increment: "  << XIncrement(1.0e-7)  << endl;
//     cout << "   X time increment: "  << XIncrement(true)  << endl;
//     cout << " Set  X time increment: "  << XIncrement(5.0e-7)  << endl;
//     cout << "   X time increment: "  << XIncrement(true)  << endl;
//     cout << "   X Multiplier: " << XMultiplier(true) << endl;
//     cout << "   X Units: " << XUnits(true) << endl;
//     cout << "SET   X Units: " << XUnits(kVOLT) << endl;
//     cout << "   X Units: " << XUnits(true) << endl;
//     cout << "SET   X Units: " << XUnits(kSECONDS) << endl;
//     cout << "   X Units: " << XUnits(true) << endl;
     cout << "   X Zero: " << XZero(true) << endl;
     cout << "SET   X Zero: " << XZero(1.0) << endl;
     cout << "   X Zero: " << XZero(true) << endl;
     cout << "SET   X Zero: " << XZero(0.0) << endl;
     cout << "   X Zero: " << XZero(true) << endl;
//     cout << "   Y Multiplier: " << YMultiplier(true) << endl;
//      cout << "   Y Units: " << YUnits(true) << endl;
//      cout << "SET   Y Units: " << YUnits(kSECONDS) << endl;
//      cout << "   Y Units: " << YUnits(true) << endl;
//      cout << "SET   Y Units: " << YUnits(kVOLT) << endl;
//      cout << "   Y Units: " << YUnits(true) << endl;
     cout << "   Y Zero: " << YZero(true) << endl;
     cout << "SET   Y Zero: " << YZero(1.0) << endl;
     cout << "   Y Zero: " << YZero(true) << endl;
     cout << "SET   Y Zero: " << YZero(0.0) << endl;
     cout << "   Y Zero: " << YZero(true) << endl;

    SET_DEBUG_STACK;
}
#endif
