/********************************************************************
 *
 * Module Name : Serialio.cpp
 *
 * Author/Date : C.B. Lirakis / 21-Mar-99
 *
 * Description : Interface to serial interface port.
 * Modified for  Linux
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 17-Feb-24  Adding in some comments. 
 *
 * Classification : Unclassified
 *
 * References :
 *   https://www.ing.iac.es/~docs/external/serial/serial.pdf
 *
 ********************************************************************/
// System includes.
#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

// Local Includes.
#include "debug.h"
#include "SerialIO.h"
/*
Constant  Description
TIOCM_LE  DSR (data set ready/line enable)
TIOCM_DTR DTR (data terminal ready)
TIOCM_RTS RTS (request to send)
TIOCM_ST  Secondary TXD (transmit)
TIOCM_SR  Secondary RXD (receive)
TIOCM_CTS CTS (clear to send)
TIOCM_CAR DCD (data carrier detect)
TIOCM_CD  Synonym for TIOCM_CAR
TIOCM_RNG RNG (ring)
TIOCM_RI  Synonym for TIOCM_RNG
TIOCM_DSR DSR (data set ready)
*/

/**
 ******************************************************************
 *
 * Function Name : SetDTR
 *
 * Description : Set the Data Terminal Ready Line to the value
 * indicated by the input argument. 
 *
 * Inputs : value - true  -> DTR to 1
 *                  false -> DTR to 0
 *
 * Returns :  1 on success. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int SerialIO::SetDTR(const bool value)
{
    int status;
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    ioctl(fPort, TIOCMGET, &status);
    if (value)
    {
	status |=  TIOCM_DTR;
    }
    else
    {
	status &= ~TIOCM_DTR;
    }
    ioctl(fPort, TIOCMSET, &status);
    SET_DEBUG_STACK;
    return 1;
}

/**
 ******************************************************************
 *
 * Function Name : GetErrno
 *
 * Description : return errno from the last operation
 *
 * Inputs : none
 *
 * Returns : errno
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int SerialIO::GetErrno() const
{
    SET_DEBUG_STACK;
    return errno;
}
/**
 ******************************************************************
 *
 * Function Name : SetModeRaw
 *
 * Description : setup the termios structure with appropriate values
 * for using raw data
 *
 * Inputs : termios structure to fill, 
 * minimum number of characters expected
 * Milliseconds to wait. 
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
void SerialIO::SetModeRaw(struct termios *termios_p, char nmin, char nwait)
{
    SET_DEBUG_STACK;
    cfmakeraw(termios_p); 
    fMode = false;
    /* 
     * MIN > 0 TIME > 0
     *    read will return either  after  nchar  bytes are  received
     *    or timeout occurs.
     *
     * MIN>0 TIME=0
     *     Min bytes must be  received. 
     *
     * MIN=0 TIME>0
     *     Either a single byte is receieved  OR timeout occurs.
     *
     *
     * MIN=0 TIME=0
     *     Read returns immediately with the number of available
     *     bytes, which may be zero!
     *
     *  Minimum number of bytes to return.
     */
    termios_p->c_cc[VMIN]  = nmin; 
    termios_p->c_cc[VTIME] = nwait; /* in 1/10 of a second timeout.      */

   /* 
     * Local modes. 
     *   Echo off, canonical mode on, 
     *   extended input processing on, 
     *   signal characters off.
     *
     * Kind of silly, 16-Jan-11, why did I have canonical on??
     */
    termios_p->c_lflag =  IEXTEN; //(ICANON | IEXTEN);

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : SetModeCanonical
 *
 * Description : Allows line processing, ie we can 
 * set the eof character. 
 *
 * Inputs : termios structure to fill and eof charcter
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
void SerialIO::SetModeCanonical(struct termios *termios_p,unsigned char *eof)
{
    fMode = true;
    /* 
     * Local modes. 
     *   Echo off, canonical mode on, 
     *   extended input processing on, 
     *   signal characters off.
     */
    termios_p->c_lflag =  (ICANON | IEXTEN);

    /* 
     * c_cc array stuff. 
     * Returns when eof is reached. 
     */
    termios_p->c_cc[VEOL]  = *eof;
    // leave the rest alone for the moment. 
}

/********************************************************************
 *
 * Function Name : Default constructor.
 *
 * Description : Opens a serial port with base parameters. 
 *               9600BPS 8 N 1
 * 
 *     This was coded up with excellent write up given in "Advanced
 * Programming in the UNIX Environment" By W. Richard Stevens. 
 * Specifically, Chapter 11,
 * The flag descriptions are given on page 329.
 * The non-canonical mode is described in detail in section 11.11.
 *
 * Inputs : portname to open. This would be the unix device driver.
 *          BR - Baudrate
 *          Pr       - Parity 
 *          Data1    - Mode:Raw - number of characters expected. 
 *                     Mode Canonical: termination character
 *          Data2    - Only used in Raw mode, number of 1/10 seconds
 *                     to delay.
 *
 * Current no flow control is supported at all.  
 *
 * Returns : none
 *
 * Error Conditions : Puts a non-zero value in MyErrorCode variable.
 *
 ********************************************************************/
SerialIO::SerialIO(const char *ComPortName, speed_t BR, Parity Pr,
		   SerialIO::OpenMode Mode, unsigned char Data1,
		   unsigned char Data2, bool Block) : CObject()
{
    SET_DEBUG_STACK;

    struct termios termios_p;
    int flags;
    int rc;
    SetName("SerialIO");
    SetVersion(1,0);
    errno = 0;

    /**
     * Save the name of the current port that is open. 
     */
    fPortName = strdup(ComPortName);

    ClearError(__LINE__);

    /*
     * How is this to be opened? Store mode for later use. 
     */

    /*
     * Note: Open read-write, no blocking (NOCTTY), and
     * no delay at all!
     * 25-Dec-10 Added the flag O_NDELAY for use on mac.
     * O_NDELAY means don't wait on DCD. 
     */
#ifdef __APPLE__
    if (Mode == ModeCanonical)
    {
	flags = O_RDWR | O_NDELAY;
    }
    else
    {
	//flags = O_RDWR | O_NOCTTY | O_NDELAY;
	//flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
	// Raw, use delay be default. 
	flags = O_RDWR | O_NOCTTY ;
	//flags = O_RDWR;
    }
#else
    if (Mode == ModeCanonical)
    {
	flags = O_RDWR;
    }
    else
    {
	flags = O_RDWR | O_NOCTTY;
    }
#endif
    /*
     * either O_NONBLOCK or O_NDELAY
     */
    if (!Block)
    {
	flags |= O_NONBLOCK;
    }
    fPort = open (fPortName, flags);
    if (fPort > 0)
    {
	/* First throw away input data (noise) */
	if (tcflush(fPort, TCIOFLUSH) < 0)
	{
	    SetError(SerialIO::BadOpen, __LINE__);
	    close (fPort);
	    SET_DEBUG_STACK;
            return;
	}

        // Clear the termios structure. 
	memset(&termios_p, 0, sizeof(struct termios));
	/* 
	 * See page 354 of Steven's Advanced UNIX Programming book. 
	 * Get existing termios properties. 
	 * 
	 * Alternative reference is:
	 * Posix Programmer's Guide - Donald A. Lewine
	 * c 1991 O'Oreily and Associates
	 * page 153  pertains  to Termial  I/O
	 *
	 *  Modified to use
	 */

        // Get the current terminal characteristics into termios_p
	rc = tcgetattr(fPort, &termios_p);
	if (rc <0)
	{
	    SetError(SerialIO::ErrorGetAttributes, __LINE__);
	    close (fPort);
	    SET_DEBUG_STACK;
            return;	      
	}
	/*
	 * Regardless of mode, we want the following to
	 * be set. 
	 * CLOCAL - Ignore modem status lines
	 * CREAD  - enable receiver
	 * CS8    - 8 bits per byte. 
	 * HUPCL  - Hangup modem on program termination. removed 26-dec-10
	 */
	termios_p.c_cflag = 0;
	termios_p.c_cflag |= (CLOCAL | CREAD | CS8 ); 

	/* 
	 * Note, CSTOPB is not set, this gives us one
	 * stop bit!
	 * Now, based on our parity, setup the sytem. 
	 * As we go into this switch statement, parity is
	 * not enabled, and is even. 
	 */
	switch (Pr)
	{
	case EVEN:
	    termios_p.c_cflag |= PARENB; /* Enable parity */
	    break;
	case NONE:
	    // Nothing to do here
	    break;
	case ODD:
	    termios_p.c_cflag |=  (PARENB| PARODD); 
	    break;
	}

	if (Mode == ModeRaw)
	{
	    SetModeRaw( &termios_p, Data1, Data2);
	} 
	else if (Mode == ModeCanonical)
	{
	    unsigned char eof[4];
	    memset(eof,0,4);
	    eof[0] = Data1;
	    SetModeCanonical(&termios_p,eof);
	}
	/* 
	 * Setup input flag (input modes)
	 * Clear them all and set the ones we want. 
	 * Ignore all breaks and do not translate them
	 */
#if 0 /* original */
	termios_p.c_iflag =  IGNPAR |   /* Ignore parity */
	    IGNBRK;   /* Ignore  break don't pad */
#else
	termios_p.c_iflag =  INPCK;
#endif

	/*
	 * Set input and output speed. These only operate on the termios 
	 * structure. 
	 * 25-Dec-10 on Mac the Set and check fails on comparison 
	 * of c_lflags
	 * query after set is 0x500
	 * value set to is:  0x20000500 This is very Darwin specific.
	 * fix by masking lower order 16 bits. 
	 */
	cfsetispeed(&termios_p, BR);
	cfsetospeed(&termios_p, BR);
	termios_p.c_lflag &= 0x0000FFFF;
	if(!SetAndCheck(&termios_p))
	{
  	    SetError(SerialIO::ErrorSetBaudRate, __LINE__);
	    close (fPort);
	    fPort = -1;
	    return;
	}
    }
    else
    {
        SetError(SerialIO::BadOpen, __LINE__);
    }
    SET_DEBUG_STACK;
}


/********************************************************************
 *
 * Function Name : Read
 *
 * Description : Get and return 1 byte worth of data from the Serial port.
 *
 * Inputs : None
 *
 * Returns : character result
 *
 * Error Conditions : 0xFF on error and puts error code in MyErrorCode
 *                    variable.
 *
 ********************************************************************/
unsigned char SerialIO::Read(void)
{
    SET_DEBUG_STACK;
    unsigned char data[1];      // Data value.
    size_t rc;                  // return code from read.
    ClearError(__LINE__);
    rc = read( fPort,            // Handle of file to read from.
               data,            // Address of buffer that receives data 
               sizeof(data));   // Number of bytes to read.

    // Errors only occur when the return code is zero.
    if(rc != 1) 
    {
        SetError(SerialIO::RxError, __LINE__);
    }
    SET_DEBUG_STACK;
    return data[0];
}
/********************************************************************
 *
 * Function Name : Read
 *
 * Description : Get and return 1 bytes of data from the Serial port.
 *
 * Inputs : buffer to fill with byte of data. 
 *
 * Returns : true on success
 *
 * Error Conditions : 0xFF on error and puts error code in MyErrorCode
 *                    variable.
 *
 ********************************************************************/
bool SerialIO::Read(unsigned char *buf)
{
    SET_DEBUG_STACK;
    size_t rc;                  // return code from read.
    ClearError(__LINE__);
    rc = read( fPort, // Handle of file to read from.
               buf,   // Address of buffer that receives data 
               1);    // Number of bytes to read.

    // Want 1 byte only!
    if(rc != 1) 
    {
        SetError(SerialIO::RxError, __LINE__);
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
/********************************************************************
 *
 * Function Name : Read
 *
 * Description : Fill buffer until terminator is reached or buffer full.
 *               This is really only applicable for canonical mode, not RAW
 *
 * Inputs : 
 *     buf       - buffer to fill, allocated by user
 *     buf_size  - size of the buffer to fill
 *     CheckSize - check the size against the expected. 
 *
 * Returns : number of bytes read. (upgraded this 17-Feb-24)
 *
 * Error Conditions : 0xFF on error and puts error code in MyErrorCode
 *                    variable.
 *
 ********************************************************************/
int32_t SerialIO::Read(unsigned char *buf, const size_t buf_size, 
                                  const bool CheckSize)
{
    SET_DEBUG_STACK;
    unsigned char *ptr = buf;
    size_t  bytes_left = buf_size;
    int32_t nread;                  // number of bytes read.
    int32_t ntotal = 0L;            // count until EAGIN. 
    bool    run    = true;
    ClearError(__LINE__);

    do {
	nread = read( fPort,         // Handle of file to read from.
		      ptr,           // Address of buffer that receives data 
		      bytes_left) ;  // Number of bytes to read.
    /*
     * if nread is zero or -1 then we may or may not have an error. 
     */
	if(nread <= 0)
	{
	    switch(errno)
	    {
	    case 0:
		// Nothing to be done. All is good. 
		break;
	    case EWOULDBLOCK: //same code as EAGAIN
		run = false;
		break;
	    default:
		break;
	    }
	}
	else
	{
	    /*
	     * Advance the pointer and the count 
	     */
	    ptr += nread;
	    ntotal += nread;
	    bytes_left = buf - ptr;
	    run = (bytes_left <= 0);
	}
    } while (run);

    if ( CheckSize)
    {
	// Errors only occur when the return code is not zero.
	if(nread != (int32_t)buf_size) 
	{
	    SetError(SerialIO::RxError, __LINE__);
	}
    }

    return ntotal;
}

/********************************************************************
 *
 * Function Name : Write
 *
 * Description : Puts a buffer of data out on the serial port.
 *
 * Inputs :    buf  - address of character buffer to write.
 *             size - Number of bytes in buffer to write out.
 *
 * Returns :  0xFF for errors, otherwise, the number of bytes
 *            actually written.
 *
 * Error Conditions : 0xFF and error code is placed in MyErrorCode
 *                    variable.
 *
 ********************************************************************/
int SerialIO::Write(const unsigned char *buf, const size_t size)
{
    size_t nwritten;   // Number of bytes actually written.
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    nwritten = write( fPort,        // Handle of file to read from.
                      buf,         // Address of buffer that sends data 
                      size) ;      // Number of bytes to read.

    if(nwritten != size) 
    {
        SetError(SerialIO::TxError, __LINE__);
        return 0;
    }
    // IF we reach this spot, make sure we wait  until all of our data 
    // has been transmitted  before we continue.
    tcdrain(fPort);
    SET_DEBUG_STACK;
    return nwritten;
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
bool SerialIO::Write(unsigned char val)
{
    int rc = Write( &val, 1);
    SET_DEBUG_STACK;
    if (rc <=0)
    {
        SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}


/********************************************************************
 *
 * Function Name : PutNull
 *
 * Description : Put a Null character out on the serial line. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool SerialIO::PutNull()
{
    unsigned char c[4];
    SET_DEBUG_STACK;
    memset(c,0,sizeof(c));
    return (Write(c,1) ==1);
}

/********************************************************************
 *
 * Function Name : PutSingleWithCheck
 *
 * Description : Put a single character, then read it back. 
 *
 * Inputs : buffer containing the single character to put. 
 *
 * Returns : true if character was echoed back. 
 *           false if not.
 *
 * Error Conditions : none
 *
 ********************************************************************/
int SerialIO::PutSingleWithCheck(const unsigned char *buf)
{
    int           rc  = -1;
    unsigned char val = 0;

    SET_DEBUG_STACK;
    ClearError(__LINE__);
    // Put a single byte of data. 
    if(Write(buf,1) == 1) 
    {
        // Read back data since it should have been echoed to us. 
        val = Read();
        if (!CheckError())
        {
            if (val == buf[0] ) 
            {
                // The data echoed back is the same as what we sent. 
	        ClearError(__LINE__);
	    }
            else if (val == 0)
            {
                //SerialIO_GotNULL;
  	        ClearError(__LINE__);
	    }
            else 
            {
                // We got back something different from what we sent. 
		rc = SetError(SerialIO::BadReturn, __LINE__);
           }
        }
        else
        {
	  rc = SetError(SerialIO::RxError, __LINE__);
        }
    } 
    else
    {
        // Out send did not go well. 
        rc = SerialIO::TxError;
    }
    SET_DEBUG_STACK;
    return rc;
}
/********************************************************************
 *
 * Function Name : Destructor
 *
 * Description : Closes the serial port opened for this instance.
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions : None
 *
 ********************************************************************/
SerialIO::~SerialIO()
{
    delete fPortName;
    close(fPort);
    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : Break
 *
 * Description : Send a break over the lines.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
void SerialIO::Break(void)
{
    tcsendbreak(fPort,0);
    SET_DEBUG_STACK;
}
void SerialIO::Flush(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (tcflush(fPort, TCIOFLUSH)  == -1)
    {
	SetError(SerialIO::BadOpen, __LINE__);
    }
}
/********************************************************************
 *
 * Function Name : GetDCD
 *
 * Description : Get the current status of the DCD line
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
bool SerialIO::GetDCD()
{
    int rv = 0;
    ioctl(fPort, TIOCMGET, &rv);
    SET_DEBUG_STACK;
    return (rv & TIOCM_CAR ? true:false);
}
/********************************************************************
 *
 * Function Name : GetErrorString
 *
 * Description : Dump the error to stderr
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
const char* SerialIO::GetErrorString(void)
{
    SET_DEBUG_STACK;
    static char ErrorString[256];
    char *str;

    switch (Error())
    {
    case SerialIO::NOError:
	str = (char *)"Why you calling me? There was no error!";
	break;
    case SerialIO::RxError:
	str = (char *)"Got fewer characters than expected";
	break;
    case SerialIO::TxError:
	str = (char *)"Did not transmit the full buffer.";
	break;
    case SerialIO::BadReturn:
	str = (char *)"On send with check, we did not get back what we sent.";
	break;
    case SerialIO::BadCheckSum:
	str = (char *)"Checksum was not what was expected. ";
	break;
    case SerialIO::GotNULL:
	str = (char *)"Not really an error, we got a NULL";
	break;
    case SerialIO::BadOpen:
	str = (char *)"Bad open, possibly port does not exist.";
	break;
    case SerialIO::ErrorSetAttributes:
	str = (char *)"Error setting termios.";
	break;
    case SerialIO::ErrorSetBaudRate:
	str = (char *)" Could not set baud rate.";
	break;
    case SerialIO::ErrorGetAttributes:
        str = (char *)"Could not get port attributes.";
        break;
    default:
	str = (char *)"Code not identified!";
	break;
    }
    sprintf(ErrorString, "SerialIO Error Line %d, SerialPort: %s errno %d, %s %s",
	    ErrorLine(), fPortName, errno, strerror(errno), str);
    SET_DEBUG_STACK;
    return ErrorString;
}
/**
 ********************************************************************
 *
 * Function Name : SetAndCheck
 *
 * Description : perform a tcsetattr and check results. 
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 ********************************************************************/
bool SerialIO::SetAndCheck( struct termios *termios_p)
{
    struct termios AsSet_p;
    int  code;
    bool rv = true;
    int  rc;
    SET_DEBUG_STACK;
    if(tcsetattr(fPort, TCSANOW, termios_p) > -1)
    {
	usleep(100000);
	// Clear the termios structure. 
	memset(&AsSet_p, 0, sizeof(struct termios));
	// Get the current terminal characteristics into termios_p
	rc = tcgetattr(fPort, &AsSet_p);
	if (rc<0)
	{
	    SetError(SerialIO::ErrorGetAttributes, __LINE__);
	    SET_DEBUG_STACK;
	    return false;
        }
	/* 
	 * On the mac, the upper order bits are very darwin specific
	 * I'm not goint to deal with them right now. 25-Dec-10
	 */
	AsSet_p.c_lflag &= 0x0000FFFF;
	AsSet_p.c_cflag &= 0x0000FFFF;
	code = memcmp( termios_p, &AsSet_p, sizeof(struct termios));
	if (code!=0)
	{
	    // If anything other than zero we had a problem 
	    // with the open!
	    SetError(SerialIO::ErrorSetAttributes, __LINE__);
	    rv = false;
	}
	else
	{
	    SetError(SerialIO::NOError, __LINE__);
	    rv = true;
	}
    }
    else
    {
        SetError(SerialIO::ErrorSetAttributes, __LINE__);
	rv = false;
    }
    SET_DEBUG_STACK;
    return rv;
}
bool SerialIO::IsCTSEnabled(void)
{
  int status;
  status = ioctl(fPort, TIOCMGET, &status);

  if(status&TIOCM_CTS) 
    return true;
  else 
    return false;
}
