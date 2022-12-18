/********************************************************************
 *
 * Module Name : System.cpp
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
#include <cstring>
#include <ctime>

// Local Includes.
#include "debug.h"
#include "DSA602.hh"
#include "DSA602_Utility.hh"
#include "System.hh"

/**
 ******************************************************************
 *
 * Function Name : System constructor
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
System::System () : CObject()
{
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : System destructor
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
System::~System ()
{
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : CalibratorAmplitude
 *
 * Description : Query the current output amplitude on the calibrator
 *
 * Inputs : NONE
 *
 * Returns : Amplitude in Volts. 
 *
 * Error Conditions : 
 *     if GPIB query fails
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double System::CalibratorAmplitude (void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char  rv[64];
    double f = -1.0;

    memset(rv, 0, sizeof(rv));
    if (DSA602::GetThis()->Command("CALI? AMPL", rv, sizeof(rv)))
    {
	cout << " Amplitude Query: " << rv << endl;
	f = Parse( "CALIBRATOR AMPLITUDE", rv);
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }

    SET_DEBUG_STACK;
    return f;
}
/**
 ******************************************************************
 *
 * Function Name : CalibratorAmplitude
 *
 * Description : Set the current output amplitude on the calibrator
 *
 * Inputs : depends on frequency 
 *      DC   {-10.0:10.0}
 *      1KHz fixed 5.0 - will return false
 *  1.024MHz fixed 0.5 - will return false
 *
 * Returns : true on success
 *
 * Error Conditions : 
 *     if GPIB query fails or value out of bounds. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool System::CalibratorAmplitude (double value)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char   pstr[64];

    // Query the what is the frequency output
    int freq = CalibratorFrequency();
    if (freq>=0)
    {
	switch(freq)
	{
	case kCAL_FREQ_ZERO:
	    // Bounds check. 
	    if((value<-10.0) || (value>9.9951))
	    {
		SetError(-2,__LINE__);
		SET_DEBUG_STACK;
		return false;
	    }
	    sprintf( pstr, "CALI AMPL:%f", value);
	    break;
	case kCAL_FREQ_1KHZ:  // Fixed
	    SET_DEBUG_STACK;
	    return false;
	    break;
	case kCAL_FREQ_1024MHZ: // Fixed
	    SET_DEBUG_STACK;
	    return false;
	    break;
	default:
	    SetError(-1, __LINE__);
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
    return DSA602::GetThis()->Command(pstr, NULL, 0);
}


/**
 ******************************************************************
 *
 * Function Name : CalibratorFrequency
 *
 * Description : Set the calibrator output frequency. 
 *
 * Inputs :
 *    one of 3 values specified in the enum CALIBRATOR_FREQ. 
 *              0 - DC level
 *           1000 - 1Khz square
 *          1.024 - 1.024MHz square
 *
 * Returns : true on success
 *
 * Error Conditions :
 *    requested value out of range or Command issued on GPIB failed. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool System::CalibratorFrequency(CALIBRATOR_FREQ val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    const char *pstr;

    switch(val)
    {
    case kCAL_FREQ_ZERO:
	pstr = "CALI FRE:0.0";
	break;
    case kCAL_FREQ_1KHZ:
	pstr = "CALI FRE:1.0e3";
	break;
    case kCAL_FREQ_1024MHZ:
	pstr = "CALI FRE:1.024e6";
	break;
    default:
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    
    SET_DEBUG_STACK;
    return DSA602::GetThis()->Command(pstr, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : CalibratorFrequency
 *
 * Description : Query the calibrator output frequency. 
 *
 * Inputs : NONE
 *
 * Returns:
 *    one of 3 values specified in the enum CALIBRATOR_FREQ. 
 *              0 - DC level
 *           1000 - 1Khz square
 *          1.024 - 1.024MHz square
 *
 *
 * Error Conditions :
 *    Command issued on GPIB failed. 
 * 
 * Unit Tested on: 24-Jan-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
int System::CalibratorFrequency(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char  rv[64];
    int   rc = -1;
    double f = -1.0;

    memset(rv, 0, sizeof(rv));
    if (DSA602::GetThis()->Command("CALI? FRE", rv, sizeof(rv)))
    {
	//cout << " Query: " << rv << endl;
	f = Parse( "CALIBRATOR FREQ", rv);

	if (f>1000.0)
	{
	    rc = kCAL_FREQ_1024MHZ;
	}
	else if (f>0.0)
	{
            rc = kCAL_FREQ_1KHZ;
	}
	else
	{
	    rc = kCAL_FREQ_ZERO;
	}
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : CalibratorImpedence
 *
 * Description : Query the calibrator output impedence
 *
 *
 * Inputs : NONE
 *
 * Returns:
 *    one of 2 values specified in the enum CALIBRATOR_FREQ. 
 *           450 ohms - DC level
 *           450 ohms - 1Khz square
 *            50 ohms - 1.024MHz square
 *
 *
 * Error Conditions :
 *    Command issued on GPIB failed. 
 * 
 * Unit Tested on: 24-Jan-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double System::CalibratorImpedence(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char  rv[64];
    double f = -1.0;

    memset(rv, 0, sizeof(rv));
    if (DSA602::GetThis()->Command("CALI? IMP", rv, sizeof(rv)))
    {
	f = Parse( "CALIBRATOR IMPEDANCE", rv);
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return f;
}
/**
 ******************************************************************
 *
 * Function Name : Date
 *
 * Description : Query the current system date. 
 *
 * Inputs : NONE
 *
 * Returns : string of the date on the mainframe. 
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* System::Date(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char  rv[64];
    static char rc[16];

    memset(rv, 0, sizeof(rv));
    memset(rc, 0, sizeof(rc));
    if (DSA602::GetThis()->Command("DATE?", rv, sizeof(rv)))
    {
	strcpy(rc, SParse( "DATE", rv));
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return NULL;
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetDate
 *
 * Description : Set the mainframe date to the current computer
 * date values. 
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool System::SetDate(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char sTime[32];
    time_t Seconds = time(NULL);

    struct tm *tm_Time = localtime(&Seconds);

    strftime ( sTime, sizeof(sTime), "DATE \'%d-%b-%y\'", tm_Time);
    
    SET_DEBUG_STACK;
    return DSA602::GetThis()->Command(sTime, NULL, 0);
}

/**
 ******************************************************************
 *
 * Function Name : Time
 *
 * Description : Query the current system time. 
 *
 * Inputs : NONE
 *
 * Returns : string of the time on the mainframe. 
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* System::Time(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char  rv[64];
    static char rc[16];

    memset(rv, 0, sizeof(rv));
    memset(rc, 0, sizeof(rc));
    if (DSA602::GetThis()->Command("TIME?", rv, sizeof(rv)))
    {
	strcpy(rc, SParse( "TIME", rv));
    }
    else
    {
	SetError(-1, __LINE__);
	SET_DEBUG_STACK;
	return NULL;
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetTime
 *
 * Description : Set the mainframe time to the current computer
 * date values. 
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool System::SetTime(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char sTime[32];
    time_t Seconds = time(NULL);

    struct tm *tm_Time = localtime(&Seconds);

    strftime ( sTime, sizeof(sTime), "TIM \'%H:%M:%S\'", tm_Time);
    cout << "Set Time " << sTime << endl;
    
    SET_DEBUG_STACK;
    bool ok = DSA602::GetThis()->Command(sTime, NULL, 0);
    return ok;
}



/**
 ******************************************************************
 *
 * Function Name : UpTime
 *
 * Description : How long as the system been up?
 *
 * Inputs : none
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
double System::UpTime(void)
{
    SET_DEBUG_STACK;
    double rv = 0.0;
    char   data[128];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    if(pDSA602->Command("UPTIME?", data, sizeof(data)))
    {
	rv = Parse( "UPTIME", data);
    }
    else
    {
	SetError(-1,__LINE__);
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : UserID
 *
 * Description : What is the user specified user id
 *
 * Inputs : none
 *
 * Returns :
 *    character string that identifies the mainframe. 
 *
 * Error Conditions :
 *     GPIB read error
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* System::UserID(void)
{
    SET_DEBUG_STACK;
    char data[256];
    static char rv[256];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    memset(data, 0, sizeof(data));
    memset(rv, 0, sizeof(rv));
    if(pDSA602->Command("USERI?", data, sizeof(data)))
    {
	strcpy( rv, SParse( "USERID", data));
    }
    else
    {
	SetError(-1,__LINE__);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : UserID
 *
 * Description : Set the mainframe UserID to the provide string value
 *
 * Inputs : user string to set up to 128 characters
 *
 * Returns : true on success
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: DOES NOT WORK
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool System::UserID(const char *str)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char Command[256];
    
    if (str==NULL)
    {
	SET_DEBUG_STACK;
	return false;
    }
    if (strlen(str) > 128) 
    {
	SET_DEBUG_STACK;
	return false;
    }
    
    sprintf(Command,"USERI \'%s\'", str); 
    
    bool ok = DSA602::GetThis()->Command(Command, NULL, 0);
    SET_DEBUG_STACK;
    return ok;
}
/**
 ******************************************************************
 *
 * Function Name : PowerOn
 *
 * Description : 
 *     Query the number of times the mainframe has been powered on. 
 *
 * Inputs : 
 *     NONE
 *
 * Returns : 
 *     integer number
 *
 * Error Conditions : if the GPIB commmand fails. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned System::PowerOn(void)
{
    SET_DEBUG_STACK;
    char data[32];
    char *p;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    memset(data, 0, sizeof(data));

    if(pDSA602->Command("POW?", data, sizeof(data)))
    {
	p = strchr(data, ' ');
	p += 1;
	return atoi(p);
    }
    else
    {
	SetError(-1,__LINE__);
    }
    SET_DEBUG_STACK;
    return 0;
}
