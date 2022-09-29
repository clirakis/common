 /*
  ******************************************************************
  *
  * Module Name : BC635.cpp
  *
  * Author/Date : C.B. Lirakis / 22-Jul-10
  *
  * Description : Display status of BC635 card
  *
  * Restrictions/Limitations :
  *
  * Change Descriptions :
  *
  * Classification : Unclassified
  *
  * References :
  *
  *******************************************************************
  */

// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <csignal>
#include <stdio.h>
#include <time.h>
#include <string.h>

/// Local Includes.
#include "debug.h"
#include "BC635.hh"
struct BC_PCI_STRUCT* BC635::fBC_PCI;
BC635* BC635::fBC635;
int    BC635::fVerbose = 0;

const unsigned int YEAR_AREA     = 0x00;
const unsigned int GPS_AREA      = 0x02;
const unsigned int OUTPUT_AREA   = 0x82;
const unsigned int INPUT_AREA    = 0x102;
const unsigned int PCI_REGISTERS = 0x50;
const unsigned int PCI_DPMEM     = 0x800;


/**
 ******************************************************************
 *
 * Function Name : BC635 Constructor
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
BC635::BC635()
{
    fError = NO_ERROR;
    fBC635 = this;

    if (fBC_PCI)
    {
        cerr <<"Driver already open." << endl;
    }
    else 
    {
        fBC_PCI = bcStartPci();
        if (!fBC_PCI)
        {
            fError = OPEN_FAIL;
            cout << "Error Opening Device Driver" << endl;
            fBC_PCI = NULL;
            return;
        }
        else
        {
	    SetDriverVerbose( fBC_PCI, fVerbose);
	    bcSetTimeFormat( fBC_PCI, UNIX_TIME);
        }
    }
    if (fVerbose>0)
    {
        cout << "Connected to BC635, time format = UNIX_TIME" << endl; 
    }
}
/**
 ******************************************************************
 *
 * Function Name : BC635 Destructor
 *
 * Description : detach from bc635 driver. 
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 26-Feb-06
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
BC635::~BC635()
{
    if (fBC_PCI)
    {
        bcStopPci(fBC_PCI);
    }
}
/**
 ******************************************************************
 *
 * Function Name : RequestTime
 *
 * Description : get a decent time format back that we can deal with.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned char BC635::RequestTime(struct timespec *req, double *delta)
{
    static struct timespec host_now;
    struct tm*      tme;
    struct tm       now;
    double          t1, t2;
    //double          Offset;
    uint32_t        maj, micro;
    uint16_t        year;
    uint16_t        nano = 0;
    int8_t          stat = 0;
    int8_t          format;

    fError = NO_ERROR;

    if (req == NULL)
    {
        fError = REQUEST_FAIL;
        return stat;
    }
    if (delta != NULL)
    {
	*delta = 0.0;
    }

    if (!fBC_PCI)
    {
        fError = REQUEST_FAIL;
        return stat;
    }
    if (bcReqTimeFormat( fBC_PCI, &format))
    {
	fError = REQUEST_FAIL;
        return 0;
    }

    if (format == BCD_TIME)
    {  	
        memset (&now, 0, sizeof(struct tm));
        bcReqYear( fBC_PCI, &year);
        if (year>=2000)
        {
            year -= 2000;
            year += 100;
        }
        // This call is somewhat faulty in that it does not fully
        // fill out the struct tm fields. Missing are the year 
        // month and day.
        if ( bcReadDecTimeEx (fBC_PCI, &now, &micro, &nano, &stat))
        {
            cerr << "Error getting decimal time. " << endl;
            fError = REQUEST_FAIL;
            return stat;
        }
        now.tm_year = year;
        //cout << "NOW: " << now.tm_mon << " " << now.tm_mday << endl;
        maj  = mktime(&now);
    } 
    else 
    {
        if ( bcReadBinTimeEx (fBC_PCI, &maj, &micro, &nano, &stat))
        {
            cerr << "Error getting binary time. " << endl;
            fError = REQUEST_FAIL;
            return stat;
        }
	//cout << "Read Time EX, Maj: " << maj << endl;
    }

    memset (&host_now, 0, sizeof(struct timespec));
    clock_gettime(CLOCK_REALTIME, &host_now);   // Host time
    tme = localtime(&host_now.tv_sec);
    host_now.tv_sec -= tme->tm_gmtoff;
    //Offset = 0.0;
    //if (tme->tm_isdst > 0)
    //{
    //    Offset = 3600.0;
    //}

    req->tv_sec  = maj;
    req->tv_nsec = micro*1000 + nano;

    t1 = ((double)host_now.tv_nsec)*1.0e-9 + host_now.tv_sec;
    t2 = ((double)req->tv_nsec)*1.0e-9     + req->tv_sec;
    *delta = t1-t2;

    if (fVerbose>2)
    {
        char msg[64];
        strftime( msg, sizeof(msg)," %H:%M:%S %F", gmtime((time_t *)&maj));
        cout << "Status:0x" << hex << (int) stat;
	cout << " Seconds:" << dec << maj;
        cout << " Micro:" << micro
             << " Nano: " << nano
             << "  BC635 Time: " 
             << msg
             << endl;
    }
    return stat;
}

/**
 ******************************************************************
 *
 * Function Name : SyncRTC
 *
 * Description : Synchornize the RTC to the current TFP time.
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions : 
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SyncRTC()
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if ( !bcSyncRtc (fBC_PCI))
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : Connect Battery
 *
 * Description : Either connect battery - yes or disconnect it - no.
 *
 * Inputs : true/false
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
bool BC635::DisconnectBattery()
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        rc = bcRtcBatt (fBC_PCI, 0);
    }
    if (rc)
    {
        fError = NO_ERROR;
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : SetLeapSeconds
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
bool BC635::SetLeapSeconds(int s, unsigned char flag)
{
    bool rc = false;

    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        rc = (bcSetLeapEvent (fBC_PCI, flag, (uint32_t)s)==0);
    }
    if (rc)
    {
        fError = NO_ERROR;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetYear
 *
 * Description : Set year on BC635
 *
 * Inputs : year - 4 digit format. 
 *
 * Returns : true on success
 *
 * Error Conditions : Card not open or call failed. 
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetYear(int y)
{
    bool rc = false;

    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        rc = (bcSetYear (fBC_PCI,(uint32_t)y) == 0);
    }
    if (rc)
    {
        fError = NO_ERROR;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetLocalOffset
 *
 * Description : This is the 1D call
 * This command adds/subtracts local time offset to the TFP time. 
 * This command affects the TFP timeonly; the generator time is not affected. 
 * (See Command 0x1C.)
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetLocalOffset(int o, bool h)
{
    bool          rc   = false;
    unsigned char half = 0;
    fError       = REQUEST_FAIL;

    if (fBC_PCI)
    {
        if (h) half = 1;
        rc = (bcSetLocOff (fBC_PCI,o,half) == 0);
    }
    if (rc)
    {
        fError = NO_ERROR;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetTimeFormat
 *
 * Description : Tell the BC635 which format (decimal or binary) to
 * exchange data in the DPRAM
 *
 * Inputs : 0 - FORMAT_DECIMAL
 *          1 - FORMAT_BINARY
 *
 * Returns : true on success
 *
 * Error Conditions : Failed to do set or card not open. 
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetTimeFormat(int f)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcSetTimeFormat(fBC_PCI, f) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetPropagationDelay
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetPropagationDelay(long d)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcSetPropDelay(fBC_PCI,d) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : RevisionID
 *
 * Description : Get revision number
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned short BC635::RevisionID()
{
    int8_t rc = 0;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcReqRevisionID(fBC_PCI,&rc) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : SetInputTimeFormat
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 26-Jul-10 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetInputTimeFormat(int v)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcSetTcIn(fBC_PCI,v) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetInputModulation
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetInputModulation(int v)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcSetTcInMod(fBC_PCI,v) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetOutputFormat
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 26-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetOutputFormat(int v)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if (bcSetGenCode(fBC_PCI,v) == 0)
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetGeneratorOffset
 *
 * Description : Set the values for the generator time code output offset. 
 *
 * Inputs : v - an integer number (-12:12)
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetGeneratorOffset(int v, bool half)
{
    bool rc  = false;
    char set = 0;

    fError  = REQUEST_FAIL;
    if (half)
    {
	set = 1;
    }
    if (fBC_PCI)
    {
        if ( bcSetGenOff (fBC_PCI, v, set) == 0 )
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetHeartBeat
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetHeartBeat(int mode, int counter1, int counter2)
{
    bool rc = false;
    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        if ( bcSetHbt (fBC_PCI, mode, counter1, counter2) == 0 )
        {
            fError = NO_ERROR;
            rc = true;
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : OutputFrequency
 *
 * Description : Set the output frequency using an index 0:2
 *
 * Inputs : Ouput frequency index
 *
 * Returns : true on success
 *
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 25-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::OutputFrequency(int v)
{
    fError = REQUEST_FAIL;
    if (v>2) v = 2;
    if (v<0) v = 0;
    if(!SetOutputFrequency(fBC_PCI, v))
    {
	fError = NO_ERROR;
	return true;
    }
    return false;
}

/**
 ******************************************************************
 *
 * Function Name : OutputFrequency
 *
 * Description : return the output frequency in 0-2, this is recognizeable
 * by the ComboBox
 *
 * Inputs :  none
 *
 * Returns : Output Frequency index
 *
 * Error Conditions : card not open or call failed
 * 
 * Unit Tested on: 25-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
int BC635::OutputFrequency(void)
{
    uint8_t val;
    fError = REQUEST_FAIL;
    if(!GetOutputFrequency( fBC_PCI, &val))
    {
	return val;
    }
    return -1;
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
 * Error Conditions : Card not open or call failed.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void BC635::EventTime(int i, struct timespec *rc)
{
    bool     rv = false;;
    uint32_t sec, micro;
    uint16_t nano;
    int8_t   stat;

    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
        switch(i)
        {
        case 0:
            rv = bcReadEventTimeEx (fBC_PCI, &sec, &micro, &nano, &stat);
            break;
        case 1:
            rv = bcReadEvent2TimeEx (fBC_PCI, &sec, &micro, &nano, &stat);
            break;
        case 2:
            rv = bcReadEvent3TimeEx (fBC_PCI, &sec, &micro, &nano, &stat);
            break;
        }
        //cout << i << " " << sec << " " << micro << " " << nano << endl;
        if (rv == 0 )
        {
            rc->tv_sec  = sec;
            rc->tv_nsec = micro*1000 + nano;
            fError = NO_ERROR;
        }
    }
}
/**
 ******************************************************************
 *
 * Function Name : SetTime
 *
 * Description : Given seconds from UNIX Epoch, set the TFP 
 * data register
 *
 * Inputs : time_t seconds from Epoch
 *
 * Returns : true on success
 *
 * Error Conditions : Either the driver is disconnected or the call
 * failed
 * 
 * Unit Tested on: 25-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetTime(time_t sec)
{
    struct tm stm;
    struct tm *dec;
    bool      rc = false;
    int8_t    format;

    fError = REQUEST_FAIL;
    if (fBC_PCI)
    {
	if (bcReqTimeFormat( fBC_PCI, &format))
	{
	    fError = REQUEST_FAIL;
	    return false;
	}

        if (format == BCD_TIME)
        {  	
            dec = gmtime( (time_t *)&sec );
            stm.tm_yday = dec->tm_yday + 1;			
            if ( bcSetDecTime (fBC_PCI, stm) == 0 )
            {
                fError = NO_ERROR;
                rc     = true;
            }
        }
        else
        {
            if ( bcSetBinTime (fBC_PCI, sec) == 0 )
            {
                fError = NO_ERROR;
                rc     = true;
            }
        }
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : DaylightSavings
 *
 * Description : Enable daylight savings offset
 *
 * Inputs : set - true, unset - false
 *
 * Returns : true on success
 *
 * Error Conditions : Either the driver is disconnected or the call
 * failed
 * 
 * Unit Tested on: 25-Jul-10
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::DaylightSavings(bool set)
{
    uint8_t val;
    fError = REQUEST_FAIL;
    // Get the current value.
    if (!GetIEEE1344Flag( fBC_PCI, &val))
    {
	if (set)
	{
	    val |= 0x08;
	}
	else
	{
	    val &= 0xF7;
	}

	if (!SetIEEE1344Flag( fBC_PCI, val))
	{
	    return true;
	}
    }
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetMode
 *
 * Description : how do I keep sync?
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
bool BC635::SetMode(int Mode)
{
    int rc;
    switch(Mode){
    case 0:
	rc = bcSetMode (fBC_PCI, TIMEMODE_CODE);
	break;
    case 1:
	rc = bcSetMode (fBC_PCI, TIMEMODE_FREERUN);
	break;
    case 2:
	rc = bcSetMode (fBC_PCI, TIMEMODE_PPS);
	break;		
    case 3:
	rc = bcSetMode (fBC_PCI, TIMEMODE_RTC);
	break;
    case 6:
	rc = bcSetMode (fBC_PCI, TIMEMODE_GPS);
	break;
    default:
	return false;
	break;
    }
    if (rc < 0)
    {
	return false;
    }
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : EnableJamSync
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
bool BC635::EnableJamSync(bool tf)
{
    if (tf)
    {
        if (bcSetJam(fBC_PCI,JAM_SYNC_ENA) == 0)
        {
            return false;
        }
    }
    else
    {
        if (bcSetJam(fBC_PCI,JAM_SYNC_DIS) == 0)
        {
            return false;
        }
    }
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : ForceJamSync
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
bool BC635::ForceJamSync()
{
    if ( bcForceJam (fBC_PCI) == 0)
    {
        return false;
    }
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetDAC
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
bool BC635::SetDAC(unsigned short val)
{
    if ( bcSetDAC (fBC_PCI, val)==0)
    {
        return false;
    }
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : SetGain
 *
 * Description : Set discipline gain of feedback loop
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
bool BC635::SetGain( short val)
{
    return  (bcSetGain (fBC_PCI, val) == 0 );
}
/**
 ******************************************************************
 *
 * Function Name : SetClock
 *
 * Description : This command advances or retards the TFP internal 
 * clock upto 100 milliseconds/second. Each count is 10us
 *
 * Inputs : value to set to. 
 *
 * Returns : true on success.
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetClock( long val)
{
    return (bcAdjustClock (fBC_PCI, val) == 0 );
}
/**
 ******************************************************************
 *
 * Function Name : SetClockSource
 *
 * Description : true for internal, false for external
 *
 * Inputs : value to set to. 
 *
 * Returns : true on success.
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool BC635::SetClockSource( bool internal)
{
    if (internal)
    {
        if ( bcSetClkSrc (fBC_PCI, INTERNAL) <0 )
        {
            return false;
        }
    }
    else
    {
        if ( bcSetClkSrc (fBC_PCI, EXTERNAL) <0)
        {
            return false;
        }
    }
    return true;
}
int BC635::DriverVerbose(uint8_t i)
{
    return SetDriverVerbose(  fBC_PCI, i);
}
