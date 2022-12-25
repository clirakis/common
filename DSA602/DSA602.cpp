/********************************************************************
 *
 * Module Name : DSA602.cpp
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
 *     DSA602A Programming Reference Manual
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
#include "DSA602.hh"
#include "debug.h"
#include "CLogger.hh"
#include "WFMPRE.hh"
#include "Module.hh"
#include "DSA602_Utility.hh"
#include "GParse.hh"
#include "gpib/ib.h"
#include "gpib/gpib_user.h"
#include "Version.hh"

DSA602* DSA602::fDSA602;

/**
 ******************************************************************
 *
 * Function Name : DSA602 constructor
 *
 * Description : NULLs out all pointers etc in the class before
 *               they are initalized. 
 *
 * Inputs :
 *    gpib_address - GPIB address of scope {0:31}
 *
 * Returns : ...
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
DSA602::DSA602 (unsigned int gpib_address): GPIB(gpib_address)
{
    SET_DEBUG_STACK;
    SetName("DSA602");
    CLogger::GetThis()->Log("# DSA602 library revision info: %s\n", Version());
    fDSA602 = this;

    fWFMPRE      = NULL;
    fSystem      = NULL;
    fpSE         = NULL;
    fMeasurement = NULL;
    fTrace       = NULL;
    fTimeBase    = NULL;

    if(!Init())
    {
	SetError(-1, __LINE__);
	return;
    }

    fWFMPRE      = new WFMPRE();
    fSystem      = new System();
    // This call will get the module data. 
    fpSE         = new StatusAndEvent();
    fMeasurement = new Measurement();
    fTimeBase    = new TimeBase();
    fTrace       = new Trace();
}

/**
 ******************************************************************
 *
 * Function Name : DSA602 destructor
 *
 * Description : Clean up all allocated pointers. 
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
DSA602::~DSA602 ()
{
    SET_DEBUG_STACK;
    delete fTrace;
    delete fSystem;
    delete fpSE;
    delete fMeasurement;
    delete fWFMPRE;
    CLogger::GetThis()->Log("# DSA602 close\n");
}

/**
 ******************************************************************
 *
 * Function Name : Avg
 *
 * Description : Turn averging on/off
 *
 * Inputs : onoff - true:on, false:off
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
void DSA602::Avg(bool onoff)
{
    SET_DEBUG_STACK;
    const char *Commands[2] = {"AVG OFF", "AVG ON"};
    int index = 0;
    if (onoff)
	index = 1;
    Command(Commands[index],NULL,0);
}
/**
 ******************************************************************
 *
 * Function Name : Avg
 *
 * Description : Get status of  averging
 *
 * Inputs : NONE
 *
 * Returns : onoff - true:on, false:off
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
bool DSA602::Avg(void) 
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    const char *Cmd = (const char *)"AVG?";
    bool rc = false;
    char Answer[32];

    if (Command(Cmd, Answer, sizeof(Answer)))
    {
	const char *rv = SParse(Cmd, (const char *)Answer);
	if (!strncasecmp( rv, "ON", 2)) rc = true;
    }
    else
    {
	SetError(-1, __LINE__);
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Init
 *
 * Description : Do a base setup, someday put this in a text file.
 *
 * Inputs : NONE
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
bool DSA602::Init(void)
{
    SET_DEBUG_STACK;
    bool rc;
    CLogger *log = CLogger::GetThis();

    // My desired setup.
    const char *Commands[] = {
	"DISP GRA:SIN",    // Single graticule
	"DISP INT:60",     // 60% intensity
	"DISP INTERP:LIN", // Linear interpolation
	"DISP MOD:VEC",    // Vector display
	"DISP PERS:0.2",   // 0.2s display persistance
	"DIST 0.90",       // 
	"DSYSTOF HUN",
	"ENC HIST:ASC",    // ASCII format for histogram output
	"ENC MEA:ASC",     // ASCII format for measurement dumps
	"ENC SET:ASC",     // ASCII format for set command
	"ENC WAV:BIN",     // binary output for Curve and waveform cmds
	"ENV OFF",
	"BIT/nr 16",       // 16 bits per point
	"BYT.or LSB",      // MSB/LSB first
	"LON ON",          // longform response on
	"AUTOSET HORIZ:PERIOD", // page 56, how horiz behaves in autoset
	"AUTOSET VERT:PP", // ECL, OFF, PP, TTL
	"AVG OFF",         
	"WFM PT.FMT:Y",        // default Y and time. 
	NULL};
    /* AVGTYPE {BACKWeight| SUMMATION} page 60
     * BASELINE NRx
     * 'CALIBRATOR AMPLITUDE:5.0E-1,FREQ:1.024E+6,IMPEDANCE:5.0E+1' page 72
     * CH - channel parameters: 77
     * CLEAR 89
     * CONDACQ 94 - FILL, REMAINING, TRIGGER, TYPE
     * CONDACQ TYPE:{AVG|BOTH|CONTINuous|DELTA|ENV|FILL|HIST.pt|REPTRIG|
     *               SEQUENCE|SINGLE|WAVFRM} 96
     * DELAY 118
     * DELTA 122
     * DIGITIZER {ARMED|RUN|STOP}
     * ENCDG {CURVE|HISTOGRAM|WAVFRM|SET} page 144
     * ENV {ON|OFF}
     * 'FFT AVG:OFF,FORMAT:DBM,WINDOW:RECTANGULAR,PHASE:WRAP,DCSUP:OFF' 150
     * FILTER {DISABLE|ENABLE}
     * HPGL 170
     * CHANNEL
     *   IMPEDENCE 80
     *   COUPLING
     *   BWLO
     * Example setup
     * TBMAIN TIME:10e-6;MAINPOS -8e-6; CH1 IMPEDENCE:50
     * TBMAIN LENGTH:512,TIME:5.0E-7,XINCR:1.0E-8'
     * 'CHL1 MNSCOUPLING:DC,PLSCOUPLING:DC,PROTECT:OFF,OFFSET:0.0E+0,AMPOFFSET:0.0E+0,BW:2.0E+7,IMPEDANCE:5.0E+1,MNSOFFSET:0.0E+0,MNSPROBE:"NONE",PLSOFFSET:0.0E+0,PLSPROBE:"NONE",SENSITIVITY:1.0E+0,UNITS:"VOLTS",VCOFFSET:0.0E+0'
     * 
     * from page 394 example coding
     * TRACE2 DESCRIPTION: 'L2'
     * CHL2 IMPEDENCE:50;CHL2 SENSITIVITY:200e-3;CHL2 OFFSET:600e-3
     * SELECT TRACE1
     * horiztontal position at 10% of waveform record 
     * LABEL DISPLAY:ON;LABEL TRACE1:'Trace 1';LABABS PCTG:10
     * Display two graticules
     * DISPLAY GRATICULE:DUAL
     * TRACE2 GRL:UPPER
     * TRACE1 GRL:LOWER
     * Single graticule
     * DISPLAY GRA:SIN
     * Remove trace
     * REMOVE TRACE2
     * 
     * Line 398 using signal processing
     * TRACE1 DESCRIPTION:'L2'
     * 10 microseconds/division
     * horizontal position at -1.2us
     * input 1M
     * sensitivity 1V
     * offset 4V
     * main trigger 2.7V
     *
     * TBMAIN TIME:10E-6;MAINPOS -1.2E-6;CHL2 IMPEDENCE:1E6; CHL2 SENS:1;CHL2 OFFSET:4;TRMAIN ANLEVEL:2.7,VOL
     *
     * TRACE1 ACCUMULATE:INFP
     * infinite persistance
     *
     * TRACE1 accumulate OFF
     * turn off persistance
     *
     * average 128  acquisitions
     * NAVG 128
     * CONDACQ TYPE:AVG
     * CONDACQ TYPE:CONTINIOUS
     *
     * Page 407 example of how to compare new waveform with reference
     * SAMPLE intervals and lengths 468
     * FFT 150
    */
    int index = 0;
    while(Commands[index] != NULL)
    {
	rc = Command( Commands[index], NULL, 0);
	if(log->CheckVerbose(1))
	{
	    log->Log("# DSA602::Init() Init: %d %s\n", index, Commands[index]);
	}
	if (rc)
	{
	    index++;
	}
	else
	{
	    SET_DEBUG_STACK;
	    return false;
	}
    }
    log->Log("# DSA602::Init() Initialize, sent %d Commands.\n", index);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Curve
 *
 * Description : Retrieve the trace data. 
 *               page 108 in manual
 *
 * Inputs : trace - which trace to use {0:3}
 *          X - X vector, allocated internally
 *          Y - Y vector
 *
 * Returns :
 *     Number of waveform descriptors that we got from a WFMPRE 
 *     query. 
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
size_t DSA602::Curve(int trace, double **X, double **Y)
{
    SET_DEBUG_STACK;
    // This is the form of the preamble if LONGFORM is set and PATH is off.
    const char Return[] = "CURVE %";  
    const int  nn = sizeof(Return)-1;
    char   msg[256];
    size_t nexpect = 0; // Number of expected bytes
    size_t count;       // 
    size_t i, ix, iy;   // Count variable. 
    char   *val;        // Pointer to allocate space to for acquisition.
    short  *p;          // Pointer to access the data as shorts.
    double *xtmp, *ytmp;
    size_t nbytes = 0;

    val = NULL;
    /*
     * Set which trace to read out. 
     */
    xtmp = NULL;
    sprintf(msg, "OUT TRA%d",trace);   // page 221
    // Setup to read trace. 
    Command(msg, NULL, 0);

    // Query all the data about the current waveforms. 
    fWFMPRE->Update();
#if 0
    // Dump WFMPRE values. 
    cout << *fWFMPRE << endl;
#endif

    count = fWFMPRE->NumberPoints();
    PT_TYPES ptfmt = fWFMPRE->PointFormat(true);

    if (count>0)
    {
	// Allocate the necessary space to hold this data. 
	nbytes = fWFMPRE->Bytes();


	switch (ptfmt)
	{
	case kPT_Y:   // Y
	    val = (char *) calloc(count+nn, nbytes*sizeof(char));
	    nexpect = nbytes * (count+1) + nn;
	    break;
	case kPT_XY: // XY 
	    val = (char *) calloc(count+nn, nbytes*sizeof(char));
	    nexpect = nbytes * 2 * (count+1) + nn;
	    break;
	case kPT_ENV:
	    cout << __FILE__ << " " << __FUNCTION__ << " " 
		 << "FIXME - kPT_ENV"
		 << endl;
	    // Not currently covered.
	    return 0;
	    break;
	case kPT_NONE:
	    // Do nothing
	    CLogger::GetThis()->Log("# %s %s This is odd.\n", __FILE__, 
				    __FUNCTION__);
	    SET_DEBUG_STACK; 
	    return 0;
	    break;
	}

	// Allocate enough space to put return data. 
	ytmp = (double *) calloc(count+1, sizeof(double));
	xtmp = (double *) calloc(count+1, sizeof(double));

	// Data format should be, 
	// C U R V E <sp> % <short n bytes> <short data> ... <terminator><checksum>
	// the conversion is Yn = YZERo + YMULt*Dn
	// Curve is documented on page 108.
	//cout << "NEXPECT: " << nexpect << endl;
	Command("CURVE?", val, nexpect);

	// Make sure we got the response we expected. 
	if (memcmp( val, Return, nn) == 0)
	{
	    p = (short *)&val[nn];  // Skip the header info. 
	    nexpect = ntohs(*p);    // Get the number of expected bytes.
	    nexpect /= 2;           // Number of words.
	    if (nexpect != count)
	    {
		cout << "Expect: " << nexpect << " Count: " << count << endl;
	    }
	    p++; // Skip over byte count. 

	    // iterate to convert.
	    for(i=0;i<count;i++)
	    {
		// Check on the point format, do this with an
                // actual query
		switch (ptfmt)
		{
		case kPT_Y:   // Y
		    iy = i;
		    ix = i;
		    //cout << i << " " << p[i] << " " << ntohs(p[i])<< endl;
		    xtmp[i] = fWFMPRE->ScaleX((double)ix);
		    ytmp[i] = fWFMPRE->ScaleY((double)p[iy]);
		    break;
		case kPT_XY: // XY
		    ix = 2*i;
		    iy = 2*i+1;
		    xtmp[i] = fWFMPRE->ScaleX((double)p[ix]);
		    ytmp[i] = fWFMPRE->ScaleY((double)p[iy]);
		    break;
		case kPT_ENV:
		case kPT_NONE:
		    // Not currently covered.
		    break;
		}
	    }
	    *Y = ytmp;
	    *X = xtmp;
	}
	free(val);
    }
    //cout << __FUNCTION__ << " COUNT: " << count << endl;
    SET_DEBUG_STACK;
    return count;
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
void DSA602::SetDisplayTrace(unsigned char n, bool t)
{
    cout << __FILE__<< " " << __FUNCTION__ << " FIXME!!" << endl;
}


/**
 ******************************************************************
 *
 * Function Name : DQuery
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
double DSA602::DQuery(const char *cc)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    double rc = 0.0;
    char com[64], msg[64];
    sprintf(com, "%s?", cc);
    memset( msg, 0, sizeof(msg));
    if(Command(com, msg, sizeof(msg)))
    {
	const char *rv = SParse(cc, (const char *)msg);
	rc = atof(rv);
    }
    else
    {
	SetError(-1, __LINE__);
    }
    return rc; 
}
/**
 ******************************************************************
 *
 * Function Name : Version
 *
 * Description : Allows the user to query the software version. 
 *
 * Inputs : NONE
 *
 * Returns : a string containing the version and compile date and time. 
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
const char* DSA602::Version(void) const
{
    SET_DEBUG_STACK;
    static char version_string[64];

    sprintf( version_string, "%d.%d %s %s", MAJOR_VERSION,
	     MINOR_VERSION, __DATE__, __TIME__);

    SET_DEBUG_STACK;
    return version_string;
}
/**
 ******************************************************************
 *
 * Function Name : operator << 
 *
 * Description : specifically for DSA602
 *
 * Inputs : 
 *    output stream to insert data into. 
 *    n - DSA602 information to parse into output. 
 *
 * Returns :
 *    formatted output stream
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
ostream& operator<<(ostream& output, const DSA602 &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "DSA602: "  << endl;

    output << *n.fWFMPRE;
    output << *n.fpSE;
    SET_DEBUG_STACK;
    return output;
}
