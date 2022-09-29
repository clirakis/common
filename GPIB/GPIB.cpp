/********************************************************************
 *
 * Module Name : GPIB.cpp
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
#include <time.h>


// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "GPIB.hh"

// For opening a device
const int sad      = 0;
const int send_eoi = 1;   // Send EOI at end of command. 
const int eos_mode = 0;
const int timeout  = T1s;


/**
 ******************************************************************
 *
 * Function Name : str_Status
 *
 * Description : After a GPIB call provide a string containing
 * the overall status of the operation. 
 *
 * Inputs : none
 *
 * Returns : String containing status of call
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 28-Oct-18
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* GPIB::str_Status( void) const
{
    SET_DEBUG_STACK;
    static char myString[512];
    char tmp[64];

    memset(myString, 0, sizeof(myString));
    sprintf(myString,"0x%X ", ibsta);

    if(ibsta & ERR)   strcat(myString, "ERR ");
    if(ibsta & TIMO)  strcat(myString, "TIMO ");
    if(ibsta & END)   strcat(myString, "END ");
    if(ibsta & SRQI)  strcat(myString, "SRQI ");
    if(ibsta & RQS)   strcat(myString, "RQS ");
    if(ibsta & SPOLL) strcat(myString, "SPOLL ");
    if(ibsta & EVENT) strcat(myString, "EVENT ");
    if(ibsta & CMPL)  strcat(myString, "CMPL ");
    if(ibsta & LOK)   strcat(myString, "LOK ");
    if(ibsta & REM)   strcat(myString, "REM ");
    if(ibsta & CIC)   strcat(myString, "CIC ");
    if(ibsta & ATN)   strcat(myString, "ATN ");
    if(ibsta & TACS)  strcat(myString, "TACS ");
    if(ibsta & LACS)  strcat(myString, "LACS ");
    if(ibsta & DCAS)  strcat(myString, "DCAS ");
    if(ibsta & DTAS)  strcat(myString, "DTAS ");


    sprintf(tmp,"iberr= %d ", iberr);
    strcat(myString, tmp);
#if 0
    if( ( ibsta & ERR ) )
    {
	strcat(myString, gpib_error_string( ibsta));
    }
#endif
    sprintf(tmp, " ibcnt = %d", ibcnt);
    strcat(myString, tmp);

    SET_DEBUG_STACK;
    return myString;
}
/**
 ******************************************************************
 *
 * Function Name : GPIB constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested:
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GPIB::GPIB () : CObject()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    fHandle = 0;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GPIB constructor
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
GPIB::GPIB (int gpib_address, bool verbose): CObject()
{
    SET_DEBUG_STACK;
//    SetOutput( logFile);
    if (verbose) SetDebug(1);
    ClearError(__LINE__);
    fHandle = 0;
    SetAddress(gpib_address);
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GPIB destructor
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
GPIB::~GPIB ()
{
    SET_DEBUG_STACK;
    CLogger::GetThis()->LogData("# GPIB close\n");
    if (fHandle>0)
    {
	ibonl( fHandle, 0);
    }
}

/**
 ******************************************************************
 *
 * Function Name : SimpleRead
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
void GPIB::SimpleRead(void)
{
    SET_DEBUG_STACK;
    CLogger *log = CLogger::GetThis();

    char data[128];

    // iberr is a global holding error code
    // ibsta holds current status
    // ibrd is a read, ibsta is returned. 
    memset( data, 0, sizeof(data));
    ibrd( fHandle, data, sizeof(data));
    if(IsError())
    {
	log->Log("# %s\n",str_Error(__FUNCTION__, __LINE__));
	log->Log("# Read done. Status: 0x%X, Number bytes %d\n", ibsta, ibcnt);
	log->Log("# %s \n", str_Status());
    }
    else if(fDebug>0)
    {
	log->Log("# Read done. Status: 0x%X, NBytes: %d, Data: %s\n",
		 ibsta, ibcnt, data);
    }
    // After this call ibcnt and ibcntl are the number of bytes 
    // actually read.
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : SetAddress
 *
 * Description : 
 *
 * Inputs : GPIB end address
 *
 * Returns : true on success
 *
 * Error Conditions : ibdev fails. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool GPIB::SetAddress(int gpib_address)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    /* Is the board already online? */
    if (fHandle>0)
    {
	ibonl( fHandle, 0); /* take it offline.    */
	fHandle = 0;        /* Set handle to zero. */
	fAddress = -1;      /* Set address to -1   */
    }
    /*
     * Now bring the board online proper.
     */
    fHandle = ibdev( 0, gpib_address, sad, timeout, send_eoi, eos_mode);
    if(fHandle < 0)
    {
	SetError( -1, __LINE__);
	CLogger::GetThis()->Log("# Error %s %s Online, ibdev failed.\n", 
				__FILE__,__FUNCTION__);
	SET_DEBUG_STACK;
	return false;
    }

    fAddress = gpib_address;
    CLogger::GetThis()->Log("# %s, Board opened, Handle: %d, Address: %d\n", 
			    __FILE__,
			    fHandle, fAddress);

    /* ibeot -- assert EOI with last data byte  */
    ibeot( fHandle, 1);
    if (ibsta & ERR)
    {
	CLogger::GetThis()->Log("# %s %s error: %d, LINE: %d\n", 
				__FILE__, __FUNCTION__, iberr, __LINE__);
    }
    /*
     * ibeos -- set end-of-string mode (board or device)
     * REOS 0x400 Enable termination of reads when eos character is received.
     * XEOS 0x800 Assert the EOI line whenever the eos character is 
     *            sent during writes.
     * BIN  0x1000 Match eos character using all 8 bits (instead of only 
     *             looking at the 7 least significant bits).
     */
    //ibeos( fHandle, REOS|XEOS|BIN);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Command
 *
 * Description : Send a command along to the GPIB controller. 
 *
 * Inputs :
 *     Command - character string with desired command. 
 *     Response - an array allocated by the user for any command response. 
 *                NULL if we don't expect anything. 
 *     n       - size of response string. 
 *
 * Returns :
 *    true on success
 *
 * Error Conditions :
 *    Bad status on command write
 *    Bad status on command read
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool GPIB::Command(const char *Command, char *Response, size_t n) const
{
    SET_DEBUG_STACK;
    CLogger *log = CLogger::GetThis();

    const struct timespec sleeptime = {0L,200000000L};

    if (Command)
    {
        ibwrt( fHandle, Command, strlen(Command));
	if (fDebug>0)
	    log->Log("# Command %s, write done. Status: %s\n", Command, 
		 str_Status());
	// The write also returns the global variable value ibsta
	if( IsError())
	{
	    log->Log("# %s\n",str_Error(__FUNCTION__, __LINE__));
	   return false;
	}
    }
    if (n>0)
    {
	nanosleep( &sleeptime, NULL);
	memset( Response, 0, n);
	return Read( Response, n);
#if 0
	// iberr is a global holding error code
	// ibsta holds current status
	// ibrd is a read, ibsta is returned. 
	if(ibrd( fHandle, Response, n) & ERR)
	    cout << "ERROR!" << endl;
	    cout << "Read done. Status: 0x"
	     << hex << ibsta
	     << " Number bytes: " << dec << ibcnt
	     << endl;
	    //OutputStatus (ibsta, &cout);
#endif
    }
    // After this call ibcnt and ibcntl are the number of bytes 
    // actually read.
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Read
 *
 * Description : Read bytes from GPIB object into buffer provided
 *
 * Inputs : buffer - byte buffer for data
 *          n - size of buffer allocated in calling function
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
bool GPIB::Read(void *buffer, size_t n) const
{
    SET_DEBUG_STACK;
    CLogger *log = CLogger::GetThis();
    // iberr is a global holding error code
    // ibsta holds current status
    // ibrd is a read, ibsta is returned. 

    ibrd( fHandle, buffer, n);
    if(fDebug>0)  log->Log("# Read done. Status: %s\n", str_Status());
    if(IsError())
    {
	log->Log("# %s\n",str_Error(__FUNCTION__, __LINE__));
	return false;
    }
    // After this call ibcnt and ibcntl are the number of bytes 
    // actually read.
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : GPIB function
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
bool GPIB::gDeviceClear(void)
{
    SET_DEBUG_STACK;
    DevClear( fHandle, fAddress);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : GPIB function
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
bool GPIB::SetTimeout(int Value)
{
    SET_DEBUG_STACK;
    /**
     * TNONE   0 Never timeout.
     * T10us   1   10 microseconds
     * T30us   2   30 microseconds
     * T100us  3  100 microseconds
     * T300us  4  300 microseconds
     * T1ms    5    1 millisecond
     * T3ms    6    3 milliseconds
     * T10ms   7   10 milliseconds
     * T30ms   8   30 milliseconds
     * T100ms  9  100 milliseconds
     * T300ms 10  300 milliseconds
     * T1s    11    1 second
     * T3s    12    3 seconds
     * T10s   13   10 seconds
     * T30s   14   30 seconds
     * T100s  15  100 seconds
     * T300s  16  300 seconds
     * T1000s 17 1000 seconds
     */
    if ((Value>-1) && (Value<18))
    {
	ibtmo( fHandle, Value);
    }
    SET_DEBUG_STACK;
    return true; 
}
/**
 ******************************************************************
 *
 * Function Name : str_error
 *
 * Description : Provide a string result for iberr
 *
 * Inputs : none
 *
 * Returns : String associated with iberr. 
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
const char* GPIB::str_Error(const char* Function, int LineNumber) const
{
    SET_DEBUG_STACK;
    static char myString[512];
    const char *p = "NONE";

    switch (iberr)
    {
    case EDVR:
	return "A system call has failed.";
	// ibcnt/ibcntl will be set to the value of errno.";
	break;
    case ECIC:
	p = "Your interface board needs to be controller-in-charge, but is not.";
	break;
    case ENOL:
	p = "You have attempted to write data or command bytes, but there are no listeners currently addressed.";
	break;
    case EADR:
	p = "The interface board has failed to address itself properly before starting an io operation.";
	break;
    case EARG:
	p = "One or more arguments to the function call were invalid.";
	break;
    case ESAC:
	p = "The interface board needs to be system controller, but is not.";
	break;
    case EABO:
	p = "A read or write of data bytes has been aborted, possibly due to a timeout or reception of a device clear command.";
	break;
    case ENEB:
	p = "The GPIB interface board does not exist, its driver is not loaded, or it is not configured properly.";
	break;
    case EDMA:
	p = "Not used (DMA error), included for compatibility purposes.";
	break;
    case EOIP:
	p = "Function call can not proceed due to an asynchronous IO operation (ibrda(), ibwrta(), or ibcmda()) in progress.";
	break;
    case ECAP:
	p = "incapable of executing function call, due the GPIB board lacking the capability, or the capability being disabled in software.";
	break;
    case EFSO:
	p = "File system error. ibcnt/ibcntl will be set to the value of errno.";
	break;
    case EBUS:
	p = "An attempt to write command bytes to the bus has timed out.";
	break;
    case ESTB:
	p = "One or more serial poll status bytes have been lost. This can occur due to too many status bytes accumulating (through automatic serial polling) without being read.";
	break;
    case ESRQ:
	p = "The serial poll request service line is stuck on. This can occur if a physical device on the bus requests service, but its GPIB address has not been opened (via ibdev() for example) by any process. Thus the automatic serial polling routines are unaware of the device's existence and will never serial poll it.";
	break;
    case ETAB:
	p = "This error can be returned by ibevent(), FindLstn(), or FindRQS(). See their descriptions for more information.";
	break;
    default:
	p = "Error code not found.";
    }
    sprintf (myString, "Function: %s, Line: %d, Error: %s",
	     Function, LineNumber, p);
    SET_DEBUG_STACK;
    return myString;
}

