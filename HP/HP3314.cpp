/********************************************************************
 *
 * Module Name : HP3314A.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic Keithley
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 17-Jan-21     CBL    Updated
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
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Local Includes.
#include "debug.h"
#include "HP3314.hh"
#include "CLogger.hh"
#include "Version.hh"

/**
 ******************************************************************
 *
 * Function Name : HP3314A constructor
 *
 * Description : constructor for the HP3314A function generator. 
 * Currently it only initalizes the base GPIB class
 *
 * Inputs : gpib_address - the GPIB address of function generator. 
 *
 * Returns :
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
HP3314A::HP3314A (int gpib_address) : 
    GPIB(gpib_address, false)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    fRangeHold = false;
}

/**
 ******************************************************************
 *
 * Function Name : HP3314A destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
HP3314A::~HP3314A ()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
}

/**
 ******************************************************************
 *
 * Function Name : Amplitude
 *
 * Description : Set the output Amplitude Can't set using floating 
 *               point numbers so we must parse then send. 
 *
 * Inputs : none
 *
 * Returns : current value
 *
 * Error Conditions : if requested voltage is out of bounds. 
 * 
 * Unit Tested on: 17-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::Amplitude(double A)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (A<0.01e-3)
    {
	/* The value is too low. */
	SetError(kERR_AMPLITUDE, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    if (A>10.0)
    {
	/* The value is too high. */
	SetError(kERR_AMPLITUDE, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    if (A<1.0)
    {
	sprintf(cstring, "AP%fMV", A*1000.0);
    }
    else
    {
	sprintf(cstring, "AP%fVO", A);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}

/**
 ******************************************************************
 *
 * Function Name : DCOffset
 *
 * Description : Set the DC offset on the output. 3 digits of precision
 *
 * Inputs : val {-5.0, 5.0} volts 
 *
 * Returns : current value
 *
 * Error Conditions : on out of bounds. 
 * 
 * Unit Tested on: 17-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::DCOffset(double val)
{
    char cstring[32];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val>5.0) || (val<-5.0))
    {
	SetError(kERR_OFFSET, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    sprintf(cstring,"OF0000000%2.1fVO", val);

    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}


/**
 ******************************************************************
 *
 * Function Name : SetN
 *
 * Description : Used in N-Cycle modes. Sets the number of cycles 
 * in a burst. 
 *
 * Inputs : val {1:1999}
 *
 * Returns : true on success
 *
 * Error Conditions : value out of range
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::NCycles(unsigned int val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val<1) || (val>1999))
    {
	SET_DEBUG_STACK;
	SetError( kERR_NCYCLE, __LINE__);
	return false;
    }
    ClearError(__LINE__);
    sprintf(cstring, "NM%dEN", val);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetSymmetry
 *
 * Description : 
 *
 * Inputs : percentage {5:95}%
 *
 * Returns : current value
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::Symmetry(unsigned char percent)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((percent<5) || (percent>95))
    {
	return false;
    }
    sprintf(cstring, "SY%dPC",percent);
    return Command(cstring, NULL, 0);
}

/**
 ******************************************************************
 *
 * Function Name : Phase
 *
 * Description : Set the phase of the output waveform
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::Phase(double deg)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (fabs(deg)>90.0)
    {
	return false;
    }
    sprintf(cstring, "PH%fDG",deg);
    return Command(cstring, NULL, 0);
}


/**
 ******************************************************************
 *
 * Function Name : ARBMode
 *
 * Description : Set the mode of for the ARB subsystem. 
 *
 *
 * Inputs : val - kARB_OFF, kON, kARB_ON_INIT
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
bool HP3314A::ARBMode(ARB_MODE val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    sprintf(cstring, "AR%1d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Waveform
 *
 * Description : Select the desired output waveform. 
 *
 * Inputs : val - desired waveform, the choices are:
 *       kDCONLY
 *       kSINE
 *       kSQUARE
 *       kTRIANGE
 *
 * Returns : true on success
 *
 * Error Conditions : Only if command fails. 
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::Waveform(WAVEFORM val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    sprintf(cstring, "FU%1d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Mode
 *
 * Description : 
 *   The modes are enumerated in the MODE enum above. Modes available are:
 *   FREERUN - run sine, square and triangle waves etc. continious or 
 *             swept. This is the most common use. 
 *
 *   GATE    - The output signal is gated
 *
 *   N_CYCLE - The output signal is a counted burst of N cycles
 *
 *   HALF_CYCLE - The output signal is alternate half cycles
 *
 *   FinXN   - The output frequency is locked to and N times the 
 *             reference frequency. 
 *
 *   FinOverN - The output frequency is locked to and 1/N times the 
 *              reference frequency. 
 *
 *   ARB      - the user provides buffers of arbitrary waveforms. 
 *
 * Inputs : val - one of the modes in the ENUM in the header. 
 *
 * Returns : True on success. 
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
bool HP3314A::Mode(MODE val) 
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    /* Error checking */
    sprintf(cstring, "MO%1d",val);
    SET_DEBUG_STACK;
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::PLLMask(PLLMASK val)
{
    char cstring[16];
    ClearError(__LINE__);
    sprintf(cstring, "PM%1d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Sweep
 *
 * Description :  Set the sweep type. Allowed values are:
 * 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::Sweep(SWEEP s)
{
    char cstring[16];
    ClearError(__LINE__);
    sprintf(cstring, "SW%1d",s);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SweepMask
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::SweepMask(SWEEP_MASK val)
{
    char cstring[16];
    ClearError(__LINE__);
    sprintf(cstring, "SM%1d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : TriggerThreshold
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::TriggerThreshold(TRIGGER_THRESHOLD val)
{
    char cstring[16];
    ClearError(__LINE__);
    sprintf(cstring, "LV%d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::TriggerSlope(TRIGGER_SLOPE val)
{
    char cstring[16];
    ClearError(__LINE__);
    sprintf(cstring, "SL%1d",val);
    return Command( cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : TriggerSource
 *
 * Description : Set the trigger input source
 *
 * Inputs : 
 *
 * Returns : current value
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
bool HP3314A::TriggerSource(TRIGGER_SOURCE val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    sprintf(cstring, "SR%1d",val);
    return Command( cstring, NULL, 0);
}

/**
 ******************************************************************
 *
 * Function Name : SetF
 *
 * Description : Basic function for all frequency sets
 *
 * Inputs : none
 *
 * Returns : current value
 *
 * Error Conditions : Frequency out of range. 
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::SetF(const char *prefix, double F)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (F>19.99e6)
    {
	SetError(kERR_FREQUENCY, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    else if (F>=1.0e6)
    {
	sprintf(cstring, "%s%fMZ", prefix, F/1.0e6);
    }
    else if(F>=1.0e3)
    {
	sprintf(cstring, "%s%fKZ", prefix, F/1.0e3);
    }
    else
    {
	sprintf(cstring, "%s%fHZ", prefix, F);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetValue
 *
 * Description : Basic function for all double value sets
 *
 * Inputs : none
 *
 * Returns : current value
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
bool HP3314A::SetValue(const char *prefix, double value, 
		       const char *suffix, double upper)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (value>upper)
    {
	return false;
    }
    else
    {
	sprintf(cstring, "%s%fHZ%s", prefix, value, suffix);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetTime
 *
 * Description : time 
 *
 * Inputs : dt 
 *
 * Returns : current value
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
bool HP3314A::SetTime(const char *prefix, double dt, double upper)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (dt>upper)
    {
	return false;
    }
    else if (dt>1.0)
    {
	sprintf(cstring, "%s%fSN", prefix, dt);
    }
    else 
    {
	sprintf(cstring, "%s%fMS", prefix, dt*1.0e3);
    }

    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : VectorHeight
 *
 * Description : Set the current vector height. This is based on the 
 * current pointer location which is set by vector modify.
 *
 * Inputs : vector height value {-1999:1999}
 *
 * Returns : true on success
 *
 * Error Conditions : If value is out of range or GPIB command fails. 
 * 
 * Unit Tested on: 22-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::VectorHeight(int val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val>1999) || (val<-1999))
    {
	SET_DEBUG_STACK;
	SetError(kERR_VECTOR_HEIGHT, __LINE__);
	return false;
    }
    else 
    {
	sprintf(cstring, "VH%05dEN", val);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : VectorLength
 *
 * Description : Set the current vector length. This is based on the 
 * current pointer location which is set by vector modify.
 *
 * Inputs : vector length value {0:127}
 *
 * Returns : true on success
 *
 * Error Conditions : if value is out of range or GPIB fails. 
 * 
 * Unit Tested on: 22-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::VectorLength(unsigned int val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val>127) || (val<1))
    {
	SET_DEBUG_STACK;
	SetError(kERR_VECTOR_LENGTH, __LINE__);
	return false;
    }
    else 
    {
	sprintf(cstring, "VL%03dEN", val);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : VectorMarker
 *
 * Description : Set the pointer to a desired vector element
 *
 * Inputs : vector length value {0:127}
 *
 * Returns : true on success
 *
 * Error Conditions : if value is out of range or GPIB fails
 * 
 * Unit Tested on: 22-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::VectorMarker(unsigned int val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val>127) || (val<0))
    {
	SET_DEBUG_STACK;
	SetError(kERR_VECTOR_MODIFY, __LINE__);
	return false;
    }
    else 
    {
	sprintf(cstring, "VM%03dEN", val);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : ARB_DT
 *
 * Description : Set the time interval per sample in ARB mode. 
 *
 * Inputs : dt in milli-seconds {0.200:19.99}
 *
 * Returns : true on success
 *
 * Error Conditions : Value out of range or GPIB fails. 
 * 
 * Unit Tested on: 22-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool HP3314A::ARB_DT(double val)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if ((val>19.99) || (val<0.2))
    {
	SET_DEBUG_STACK;
	SetError(kERR_ARB_DT, __LINE__);
	return false;
    }
    else 
    {
	sprintf(cstring, "DT%2.2fMS", val);
    }
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}


/**
 ******************************************************************
 *
 * Function Name : ParseValue
 *
 * Description : Given the resulting return from a query and the
 * prefix and suffix assocated with expected response, determine the
 * return value. 
 *
 * Inputs : 
 *           result - result obtained from query
 *           prefix - expected prefix from query
 *           suffix - expected suffix from query
 *
 * Returns : current value
 *
 * Error Conditions : if an expectation is not met, like getting
 *                    a different result from the query than we expected. 
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double HP3314A::ParseValue(const char *result, 
			   const char *prefix, 
			   const char *suffix) const
{
    SET_DEBUG_STACK;
    const char kCR = 0x0D;  /* \r */
    const char kLF = 0x0A;  /* \n */

    char   *p, *bol, *eol;
    char   test[4];
    char   sVal[32];       /* String value    */
    double val   = 0.0;    /* converted value */

    memset(sVal, 0, sizeof(sVal));

    bol = (char *) result;  // Beginning of line

    /* Null terminate when we find the CRLF */
    eol = strchr( bol, kCR);  // End of line, CR
    if (eol != NULL)
    {
	*eol = '\0'; // Null terminate. LF
    }
    /*
     * This may or may not be present depending on how this goes. 
     */
    p = strchr( bol, kLF);  // End of line
    if (p != NULL)
    {
	eol = p;
	*p = '\0'; // Null terminate.
    }
    /* If we got nothing, e.g. the string length is now zero, return. */
    if (strlen(result) == 0)
    {
	SET_DEBUG_STACK;
	return 0.0;
    }

    /* Parse the result. */

    /* 
     * See that the first two characters are the same, otherwise it is an 
     * error. 
     */
    memcpy(test, bol, 2);
    if (strncmp(test, prefix, 2) != 0)
    {
	/* Error parsing */
	SET_DEBUG_STACK;
	return 0.0;
    }
    bol += 2; // Advance beyond the prefix. 

    /* If there is a suffix, find it. */
    if (suffix != NULL)
    {
	/* The command expected a suffix. */
	p = strstr( bol, suffix);
	if (p != NULL)
	{
	    memcpy(test, suffix, 2);
	    /* p now points at the end of the string. */
	}
	else
	{
	    p = eol;
	    /*
	     * There are some commands that expect a suffix and
	     * it has not been found. Experimentally these are:
	     * DT, VH, VL, VM. 
	     * This isn't necessarily an error. 
	     */
	}
    }
    else 
    {
	p = eol;
    }

    /*
     * Now we have pointers to where the value starts and ends in the string.
     */
    strncpy( sVal, bol, p-bol);
    val   = atof(sVal);
    SET_DEBUG_STACK;
    return val;
}

/**
 ******************************************************************
 *
 * Function Name : Query
 *
 * Description : query value on the function generator. The available 
 *  Queries are found in the enumeration enum QUERYS in the associated 
 *  header. The query information can be found in the HP3314A operators
 *  manual on page: 97.
 *
 *  Page 96 has typical resonses. 
 *
 * Inputs : Query to perform. 
 *
 * Returns : current value (double) reltated to the query. 
 *
 * Error Conditions :
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double HP3314A::Query(QUERYS q) const
{
    char   result[32], Query[8];
    char   *prefix, *suffix; // for each query we have an expected result. 

    SET_DEBUG_STACK;
    
    switch (q)
    {
    case kAMPLITUDE:
	prefix = (char *) "AP";
	suffix = (char *) "VO"; // VOLTS
	break;
    case kAMPLITUDE_MODULATION:
	prefix = (char *) "AM";
	suffix = NULL;
	break;
    case kARB_MODE:
	prefix = (char *) "AR";
	suffix = NULL;
	break;
    case kARB_WAVE_NUMBER:
	prefix = (char *) "RW";
	suffix = NULL;
	break;
    case kARB_DT:
	prefix = (char *) "DT";
	suffix = (char *) "SN";  // SECONDS
	break;
    case kERROR_CODE:
	prefix = (char *) "ER";
	suffix = NULL;
	break;
    case kFREQUENCY:
	prefix = (char *) "FR";
	/* 
	 * Set Frequency, example: FR 0002550000.HZ
	 * This is typical of frequency results. 
	 */
	suffix = (char *) "HZ";
	break;
    case kFREQUENCY_MODULATION:
	prefix = (char *) "FM";
	suffix = NULL;
	break;
    case kFUNCTION_INVERT:
	prefix = (char *) "FI";
	suffix = NULL;
	break;
    case kFUNCTION_SELECT:
	prefix = (char *) "FU";
	suffix = NULL;
	break;
    case kMANUAL_SWEEP:
	prefix = (char *) "MA";
	suffix = NULL;
	break;
    case kMARKER_FREQUENCY:
	prefix = (char *) "MK";
	suffix = (char *) "HZ";
	break;
    case kQMODE:
	prefix = (char *) "MO";
	suffix = NULL;
	break;
    case kQN:
	prefix = (char *) "NM";
	suffix = (char *) "EN";  // ENTER
	break;
    case kOFFSET:
	prefix = (char *) "OF";
	/* 
	 * Return message is special. 
	 * example return value: OF 0000000.500V0
	 */
	suffix = (char *) "VO";  // VOLTS
	break;
    case kPHASE:
	prefix = (char *) "PH";
	suffix = (char *) "DG";  // Degrees
	break;
    case kSTART_FREQUENCY:       // Sweep Start F
	prefix = (char *) "ST";
	suffix = (char *) "HZ";
	break;
    case kSTOP_FREQUENCY:       // Sweep Stop F
	prefix = (char *) "SP";
	suffix = (char *) "HZ";
	break;
    case kQSWEEP:              // Sweep
	prefix = (char *) "SW";
	suffix = NULL;
	break;
    case kSWEEP_MASK:
	prefix = (char *) "SM";
	suffix = NULL;
	break;
    case kSYMMETRY:
	prefix = (char *) "SY";
	suffix = (char *) "PC";  // Percent
	break;
    case kQTRIGGER_INTERVAL:
	prefix = (char *) "TI";
	suffix = (char *) "SN";  // Seconds
	break;
    case kQTRIGGER_THRESHOLD:
	prefix = (char *) "LV";
	suffix = NULL;
	break;
    case kQTRIGGER_SLOPE:
	prefix = (char *) "SL";
	suffix = NULL;
	break;
    case kQTRIGGER_SOURCE:
	prefix = (char *) "SR";
	suffix = NULL;
	break;
    case kQVCO:
	prefix = (char *) "VC";
	suffix = NULL;
	break;
    case kVECTOR_HEIGHT:
	prefix = (char *) "VH"; // Odd return
	suffix = (char *) "EN";
	break;
    case kVECTOR_LENGTH:
	prefix = (char *) "VL";  // Odd return
	suffix = (char *) "EN";
	break;
    case kVECTOR_MARKER:
	prefix = (char *) "VM"; // Odd return
	suffix = (char *) "EN";
	break;
    }

    /*
     * Issue a command with a result. 
     */
    sprintf(Query, "Q%s", prefix);
    Command(Query, result, sizeof(result));

    return ParseValue( result, prefix, suffix);
}
/**
 ******************************************************************
 *
 * Function Name : ARB_Wave
 *
 * Description : Recall stored ARB
 *
 * Inputs : Waveform number to retrive. 
 *
 * Returns : current value
 *
 * Error Conditions : When the waveform requested is > 5
 * 
 * Unit Tested on: 23-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
bool HP3314A::ARB_Wave(unsigned char Number)
{
    char cstring[16];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (Number>5)
    {
	return false;
    }
    sprintf(cstring, "RW%1d", Number);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Range
 *
 * Description : Turn range hold on 
 *
 * Inputs : integer value from 0 - OFF, through 8. See page 93. 
 *
 * Returns : true on success. 
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
bool HP3314A::Range(int val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char cstring[8];

    if ((val<0) || (val>8))
    {
	SetError(kERR_RANGE,__LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    if (val>0)
    {
	fRangeHold = true;
    }
    else
    {
	fRangeHold = false;
    }
    sprintf( cstring, "RH%1d", val);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : LoadWaveForm
 *
 * Description : Helper function to load up a pre-defined waveform. 
 *
 * Inputs : 
 *   Height - an array of length Nsamples of heights to load {-1999:1999}
 *   Length - an array of length Nsamples of time intervals {1:127}
 *   NSamples - number of samples to load. 
 *   Append   - Reset marker (false) to the first position if true
 *              otherwise this will append (true). 
 *
 * Returns : number of time samples used for the waveform. returns 0 on
 *           error. 
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 23-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned int HP3314A::LoadWaveForm(const int Height[],  
				   const unsigned int Length[], 
				   unsigned int Nsamples,
				   bool Append)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    CLogger *pLog = CLogger::GetThis();
    unsigned int total_length = 0;
    int err;

    if (Debug(1))
    {
	pLog->Log("#ARB Waveform load. (index, Height, Length)\n");
    }

    if (!Append)
    {
	VectorMarker(1);
	err = (int) Query(HP3314A::kERROR_CODE);
	if(err!=0)
	{
	    pLog->Log("# ERROR loading waveform, VM. %d\n", err);
	}
    }

    for (unsigned int i=0;i<Nsamples;i++)
    {
// 	cout << "(" << i << "," << Height[i] << "," << Length[i] 
// 	     << ")" << endl;
	if(Debug(1))
	{
	    pLog->Log("(%d,%d,%d)\n", i, Height[i], Length[i]);
	}
	total_length += Length[i];
	VectorHeight(Height[i]);
	err = (int) Query(HP3314A::kERROR_CODE);
	if(err!=0)
	{
	    pLog->Log("# ERROR loading waveform, Height. %d %d\n", i, err);
	}
	VectorLength(Length[i]);
	err = (int) Query(HP3314A::kERROR_CODE);
	if(err!=0)
	{
	    pLog->Log("# ERROR loading waveform, Length. %d %d\n", i, err);
	}
        // This gets us ready for the next insert, it does not 
	// finalize the current entry. 
	if (i<(Nsamples-1))
	{
	    InsertVector(); 
	    err = (int) Query(HP3314A::kERROR_CODE);
	    if(err!=0)
	    {
		pLog->Log("# ERROR loading waveform, Insert. %d %d\n", i, err);
	    }
	}
    }

    SET_DEBUG_STACK;
    return total_length;
}

/**
 ******************************************************************
 *
 * Function Name : GetWaveform
 *
 * Description : Helper functon that fills provided arrays with
 *               the waveform information for diagnostic puroses. 
 *
 * Inputs : 
 *   Height - an array of length Nsamples of heights to load {-1999:1999}
 *   Length - an array of length Nsamples of time intervals {1:127}
 *   NSamples - size of provided arrays. 
 *
 * Returns :  The entries in the arrays. 0 on failure.
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 23-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
size_t HP3314A::GetWaveForm(int *Height, unsigned int *Length, size_t N)
{
    SET_DEBUG_STACK;
    unsigned int L;
    int H;
    ClearError(__LINE__);
    CLogger *pLog = CLogger::GetThis();
    unsigned int total_length = 0;
    unsigned int err;

    // Loop to max, but kick out on error. 
    unsigned int i = 0;
    bool run = true;

    // Before we start, zero the buffers. 
    memset(Height, 0, N*sizeof(int));
    memset(Length, 0, N*sizeof(unsigned int));

    //cout << "GetWaveForm, VectorMarker:" << VectorMarker() << endl;
    /*
     * I'm not trusting that the original Vector Marker we got
     * is the number of vectors in the buffer. Therefore, I 
     * retrieve until I get an error for setting the VectorMarker
     * position. 
     */
    do
    {
	VectorMarker(i+1);
	err = (int) Query(HP3314A::kERROR_CODE);
	if(err==0)
	{
	    // Got a vector position. 
	    L = VectorLength();
	    err = (int) Query(HP3314A::kERROR_CODE);
	    if(err!=0)
	    {
		pLog->Log("# ERROR reading waveform, Length. %d %d\n", i, err);
	    }
	    H = VectorHeight();
	    if(err!=0)
	    {
		pLog->Log("# ERROR reading waveform, Height. %d %d\n", i, err);
	    }
	    total_length += L;
	    Height[i] = H;
	    Length[i] = L;
	    i++;
	    run = i<N;
	}
	else 
	{
	    run = false;
	    // At this point i is one too big. 
	    i--;
	}
    } while (run);

    if (i>=N) i = N;

    SET_DEBUG_STACK;
    return i;
}

/**
 ******************************************************************
 *
 * Function Name : DumpWaveform
 *
 * Description : Get the total waveform in the memory and
 *               dump it to the screen. 
 *
 * Inputs : NONE
 *
 * Returns :  NONE
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 23-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
void HP3314A::DumpWaveForm(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    CLogger *pLog = CLogger::GetThis();
    unsigned int total_length = 0;
    double dt = ARB_DT();
    const size_t N = 25;
    int          Height[N];
    unsigned int Length[N];
    size_t rc;

    cout << "ARB Waveform dump. (index, Height, Length)" << endl;

    if (Debug(1))
    {
	pLog->Log("#ARB Waveform dump. (index, Height, Length)\n");
    }

    rc = GetWaveForm(Height, Length, N);
    for (size_t i=0;i<rc;i++)
    {
	total_length += Length[i];
	cout << "( " << i << "," << Height[i] << "," 
		 << Length[i] << " )" << endl;
	if(Debug(1))
	{
	    pLog->Log("(%d,%d,%d)\n", i, Height[i], Length[i]);
	}
    }

    cout << "Overall total length: " << total_length
         << " Total waveform time: " << dt*total_length
         << endl;  

    if(Debug(1))
    {
	pLog->Log("Overall total length: %d Total waveform time: %f\n",
		  total_length, dt*total_length);
    }

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : ModeStr
 *
 * Description : Return string from mode value
 *
 * Inputs : mode to get string value for. 
 *
 * Returns : string associated with mode m
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
const char *HP3314A::ModeStr(MODE m) const
{
    const char *rv = NULL;
    SET_DEBUG_STACK;

    switch (m)
    {
    case kFREERUN:
	rv = "Free Run";
	break;
    case kGATE: 
	rv = "Gate";
	break;
    case kN_CYCLE: 
	rv = "N Cycle";
	break;
    case kHALF_CYCLE:
	rv = "Half Cycle";
	break;
    case kFINXN:
	rv = "Fin X N";
	break;
    case kFINOVERN:
	rv = "Fin 1/N";
	break;
    case kARB:
	rv = "Arbitrary";
	break;
    default:
	rv = "NOT_VALID";
	break;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : RecallARB
 *
 * Description : Recall stored ARB
 *
 * Inputs : Waveform to get string on. 
 *
 * Returns : current value
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
const char *HP3314A::FunctionStr(WAVEFORM f)
{
    const char *rv = NULL;
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    switch (f)
    {
    case kDCONLY:
	rv = "DC Only";
	break;
    case kSINE:
	rv = "SINE";
	break;
    case kSQUARE: 
	rv = "Square";
	break;
    case kTRIANGLE:
	rv = "Triangle";
	break;
    default:
	rv = "NOT_VALID";
	break;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : ErrorCode
 *
 * Description : Return string from code value
 *
 * Inputs : code to get string value for. 
 *
 * Returns : string associated with code
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
const char *HP3314A::ErrorCode(unsigned char val)
{
    const char *rv = NULL;
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    switch (val)
    {
    case 0:
	rv = "No Error";
	break;
    case 1:
	rv = "Frequency/Symmetry conflict";
	break;
    case 2: 
	rv = "Bus address entry error";
	break;
    case 3:
	rv = "Front panel failure invalid keycode";
	break;
    case 4:
	rv = "Calibration measurement not performed";
	break;
    case 5:
	rv = "Allowed in sweep only";
	break;
    case 6: 
	rv = "Not Allowed in sweep";
	break;
    case 7: 
	rv = "Not Allowed in log sweep";
	break;
    case 8:
	rv = "Store 0 not allowed";
	break;
    case 9: 
	rv = "Non-volatile memory lost, battery down";
	break;
    case 10:
	rv = "Vector insert not allowed";
	break;
    case 11:
	rv = "Vector delete not allowed";
	break;
    case 18:
	rv = "Allowed in ARB, only";
	break;
    case 19: 
	rv = "Not allowd in ARB";
	break;
    case 20:
	rv = "Unstable input frequency";
	break;
    case 21:
	rv = "Input frequency outside of acquisition range";
	break;
    case 22:
	rv = "3314A output frequency would be out of range";
	break;
    case 23: 
	rv = "SW/TR INTVL>20ms";
	break;
    case 24:
	rv = "Internal phase locked loop, unlocked";
	break;
    case 30: 
	rv = "No frequency detected";
	break;
    case 31:
	rv = "Frequency error exceeds correction capablity";
	break;
    case 32:
	rv = "Frequency unstable during calibration";
	break;
    case 34:
	rv = "Signal amplitude outside measurement range";
	break;
    case 35: 
	rv = "Signal amplitude gain too high";
	break;
    case 36:
	rv = "Signal amplitude gain too low";
	break;
    case 37:
	rv = "Signal amplitude gain exceeds correction capability";
	break;
    case 38:
	rv = "Signal amplitude gain offset exceeds correction capability";
	break;
    case 41: 
	rv = "Mneumonic invalid";
	break;
    case 42:
	rv = "Definition number invalid";
	break;
    case 43: 
	rv = "Data invalid";
	break;
    case 44:
	rv = "Units invalid";
	break;
    case 45:
	rv = "Range Hold not allowed";
	break;
    case 46:
	rv = "ARB/SWEEP parameter conflict";
	break;
    case 47: 
	rv = "Not allowed in MAN Sweep";
	break;
    case 50:
	rv = "AM or FM/VCO input voltage exceeds normal operating limits";
	break;
    case 51:
	rv = "Output voltage exceeds safe operating limits";
	break;
    default:
	rv = "NOT_VALID";
	break;
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : operator << 
 *
 * Description : output the current status of the system to 
 *               the stream indicated
 *
 * Inputs : output - desired output stream
 *          p      - HP3314 class
 *
 * Returns : string associated with code
 *
 * Error Conditions :  NONE
 * 
 * Unit Tested on: 18-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */    
std::ostream& operator<<(std::ostream& output, const HP3314A &p)
{
    SET_DEBUG_STACK;

    output << "HP3314A, current values are:" << endl
	   << "    Mode:      " << p.ModeStr(p.Mode()) << endl
	   << "    Waveform:  " << p.Waveform() << endl
	   << "    Frequency: " << p.Frequency() << " Hz." << endl
	   << "    Amplitude: " << p.Amplitude() << " Volts."<< endl
//	   << "    Range:     " << p.Range() << endl
	   << "    Symmetry:  " << p.Symmetry() << " %"<< endl
	   << "    Phase:     " << p.Phase() << " degrees." << endl
	   << "    Invert:    " << p.FunctionInvert() << endl
	   << "    DC Offset: " << p.DCOffset() << " Volts." << endl
	   << "    AM:        " << p.AM() << endl
	   << "    FM:        " << p.FM() << endl
	   << "    VCO:       " << p.VCO() << endl
	   << "    Sweep:     " << p.Sweep() << endl;
    if (p.Sweep() != HP3314A::kSWEEP_OFF)
    {
        output << "        Manual:" << p.ManualSweep() << endl
	       << "        Start: " << p.StartFrequency() << endl
	       << "        Stop:  " << p.StopFrequency() << endl
	       << "        Mask:  " << p.SweepMask() << endl
	       << "        Trigger Interval: " << p.SweepTriggerInterval() << endl;
    }

    output << "    Marker Frequency: " << p.MarkerFrequency() << endl
	   << "    Trigger" << endl
	   << "        Source:    " << p.TriggerSource() << endl
	   << "        Threshold: " << p.TriggerThreshold() << endl
	   << "        Slope:     " << p.TriggerSlope() << endl
	   << "    ARB Mode: "  << p.ARBMode() << endl
	   << endl;

    if (p.ARBMode() == HP3314A::kARB_ON)
    {
	output << "ARB parameters: " << endl
	       << " DT: " << p.ARB_DT() << "seconds" << endl
	       << " Selected waveform: " << p.ARB_Wave() << endl
	       << " Current vector position: " << p.VectorMarker()
	       << " Height: " << p.VectorHeight()
	       << " Length: " << p.VectorLength()
	       << endl;
    }

    SET_DEBUG_STACK;
    return output;
}
const char* HP3314A::Version(void) const
{
    SET_DEBUG_STACK;
    static char version_string[64];

    sprintf( version_string, "%d.%d %s %s", MAJOR_VERSION,
	     MINOR_VERSION, __DATE__, __TIME__);

    SET_DEBUG_STACK;
    return version_string;
}
