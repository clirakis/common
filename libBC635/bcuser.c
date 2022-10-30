/**
 ******************************************************************
 * @file bcuser.c
 * @author C.B. Lirakis 
 * @date 17-Feb-13
 * @version 1.0
 * @brief  User space entry points for bc635 driver, BIG ENDIAN hardware! 
 * @see bc625 PCI_PCIe User Guide RevA.pdf. Intel is little endian
 *
 * 29-Oct-22 CBL removed stropts.h
 *
 *******************************************************************
 */
/* System includes. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <inttypes.h>
//#include <stropts.h>
#include <arpa/inet.h>

/** Local Includes. */
#include "bcuser.h"

/** 
 * global variable for setting verbosity.
 */
static int gVerbose = 0;

/**
 * Entry function to set verbosity. 
 * 0 - none
 * 1 - mild
 * 2 - pretty detailed
 * 3 - out of control - you'd better dump this to a file. 
 */
void BCUser_SetVerbose(int val)
{
    gVerbose = val;
}
/**
 * Get the current verbosity setting. 
 */
int BCUser_GetVerbose(void)
{
    return gVerbose;
}
/**
 * Entry function to set verbosity in the actual driver. The output
 * is found in syslog or can be gotten using dmesg.  
 * 0 - none
 * 1 - mild
 * 2 - pretty detailed
 * 3 - out of control - you'd better dump this to a file. 
 */
uint8_t SetDriverVerbose(BC_PCI_STRUCT* hBC_PCI, uint8_t val)
{
    struct  btfp_inarea cmd;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    cmd.cmd    = VERBOSE_SET;
    cmd.subcmd = val;
    return ioctl( hBC_PCI->fd, TFP_DRVRCTL, &cmd);
}

/**
 ******************************************************************
 *
 * Function Name : bcStartPci
 *
 * Description : Open a handle to the device and allocate any necessary
 * resources
 *
 * Inputs : none
 *
 * Returns : Populated BC_PCI_STRUCT on success
 *
 * Error Conditions : NULL pointer on failure to open
 * 
 * Unit Tested on: 20-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
BC_PCI_STRUCT* bcStartPci(void)
{
    BC_PCI_STRUCT *p;
    p = (BC_PCI_STRUCT *) calloc(1, sizeof(BC_PCI_HANDLE));

    if ((p->fd = open( "/dev/BC635", O_RDWR)) <0)
    {
	fprintf(stderr,"bcStartPCI error opening handle. %s\n",
		strerror(errno));
	free(p);
	p = NULL;
    }
    return p;
}

/**
 ******************************************************************
 *
 * Function Name : bcStopPci
 *
 * Description : close the connection to the device and free any 
 * available resources.
 *
 * Inputs : handle to BCPCI structure
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 18-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void  bcStopPci(BC_PCI_STRUCT* hBC_PCI)
{
    close(hBC_PCI->fd);
    free(hBC_PCI);
}

/**
 ******************************************************************
 *
 * Function Name : bcReadReg
 *
 * Description : read device registers
 *
 * Inputs :  hBC_PCI  - Handle to bc635
 *           dwOffset - offset to register you want to read
 *           ulpData  - pointer to return data.  
 *
 * Returns : rc == 0 success, rc <0 failure look at errno for reason.
 *
 * Error Conditions : the handle is null - no card open
 *                    the offset to the register is out of bounds
 *                    the pointer to the return data is null
 * 
 * Unit Tested on: 27-Nov-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReadReg(BC_PCI_HANDLE hBC_PCI, uint8_t dwOffset, uint32_t* ulpData)
{
    struct btfpctl ctl;
    uint8_t rc;

    if ((hBC_PCI == 0) || (ulpData==0))
    {
	return -1;
    }
    ctl.id  = dwOffset;     /* This is the specific register offset*/
    
    rc = ioctl( hBC_PCI->fd, BTFP_READREG, &ctl);
    if (rc)
    {
	*ulpData = 0;
	return -1;
    }

    *ulpData = ctl.u32; //ntohl(ctl.u32);
    if (gVerbose>0)
    {
	fprintf(stdout,"Register %X Value %X\n" , dwOffset, *ulpData);
    }

    return 0;
}
/**
 ******************************************************************
 *
 * Function Name : bcWriteReg
 *
 * Description : write device registers
 *
 * Inputs : hBC_PCI  - handle to BC635 card
 *          dwOffset - register to write to
 *          data     - data value to write to the register
 *
 * Returns : 0 on success, -1 on failure
 *
 * Error Conditions : if the handle is zero (no card) or the Offset, 
 *                    to the register is out of bounds. 
 * 
 * Unit Tested on: 27-Nov-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcWriteReg(BC_PCI_HANDLE hBC_PCI, uint8_t dwOffset, uint32_t data)
{
    struct btfpctl ctl;

    ctl.id  = dwOffset;     /* This is the specific register offset*/
    ctl.u32 = data;         /* The data we want to stuff. */

    if (hBC_PCI == 0)
    {
	return -1;
    }

    return ioctl( hBC_PCI->fd, BTFP_WRITEREG, &ctl);
}

/**
 ******************************************************************
 *
 * Function Name : bcReadDPReg
 *
 * Description : Given the register offset in dwOffset, read the contents
 *               into the data area pointed to by bpData. 
 *
 * Inputs : dwOffset - see defines for register address offset. 
 *          bpData   - pointer to data storage, typically a 32 bit word
 *
 * Returns : -1 or 0 depending on failure mode.
 *
 * Error Conditions : none
 * 
 * Unit Tested on:  Not yet implemented
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadDPReg (BC_PCI_HANDLE hBC_PCI,uint8_t dwOffset,uint8_t* bpData)
{
    printf("%s Not implemented.\n", __FUNCTION__);
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcWriteDPReg
 *
 * Description : Write the contents of a DP register. 
 *
 * Inputs : dwOffset - see defines to see which offset applies
 *
 * Returns :
 *
 * Error Conditions : -1 or 0 depending on failure mode.
 * 
 * Unit Tested on: Not yet implemented
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcWriteDPReg(BC_PCI_HANDLE hBC_PCI, uint8_t dwOffset, uint8_t data)
{
    printf("%s Not implemented.\n", __FUNCTION__);
    return -1;
}

/**
 ******************************************************************
 *
 * Function Name : bcReadBinTime
 *
 * Description : Read time in binary format. Implemented as a wrapper to
 *               calling ReadBinTimeEx and throwing away some data. 
 *
 * Inputs : hBC_PCI - pointer to card handle
 *
 * Returns : ulpMaj - major time, seconds since unix epoch
 *           ulpMin - microseconds time
 *           bpstat - status register
 *               Bit 1: Tracking (T)
 *                   0: Locked To Selected Reference
 *                   1: Flywheeling (Not Locked)
 *     
 *               Bit 2 Phase (P)
 *                   0: < X Microseconds
 *                   1: > X Microseconds
 *                   X = 5 (Mode 0) 
 *                   X = 2 (All Other Modes)
 *              
 *               Bit 3 Frequency (F)
 *                   0: < 5 x 10-8
 *                   1: > 5 x 10-8
 *             
 *           0 on success -1 on failure
 *
 * Error Conditions : Any of the return values or handle is NULL
 * 
 * Unit Tested on: 20-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcReadBinTime(BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
		       uint32_t* ulpMin, int8_t* bpstat)
{
    uint16_t ulpNano;
    return bcReadBinTimeEx (hBC_PCI, ulpMaj, ulpMin, &ulpNano, bpstat);
}
/**
 ******************************************************************
 *
 * Function Name : bcReadDecTime
 *
 * Description : Latches and returns time captured from the time registers.
 *               data is first formatted in struct tm like format 
 *               before reading. This is a wrapper, the heavy lifting
 *               is done in bcReadDecTimeEx
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : ptm - struct tm filled with appropriate values
 *           ulpMin - unsigned long to store microseconds
 *           bpstat - pointer to unsigned char to store status bits.
 *
 * Error Conditions : bad handle or bad read. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadDecTime(BC_PCI_HANDLE hBC_PCI, struct tm *ptm, 
		       uint32_t* ulpMin, int8_t* bpstat)
{
    uint16_t ulpNano; /* Throw this away for this call. */
    return bcReadDecTimeEx (hBC_PCI, ptm, ulpMin, &ulpNano, bpstat);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetBinTime
 *
 * Description : Set the major time buffer.
 *
 * Inputs : hBC_PCI - handle to card
 *          newtime = unsigned long time value to set (Unix format).
 *
 * Returns : none
 *
 * Error Conditions : bad handle or bad write
 * 
 * Unit Tested on: 06-Apr-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetBinTime (BC_PCI_HANDLE hBC_PCI, uint32_t newtime)
{
    uint32_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    val = htonl(newtime);
    if (!bcSetTimeFormat (hBC_PCI, UNIX_TIME))
    {
	return ioctl( hBC_PCI->fd, BTFP_MAJOR_TIME, &val);
    }
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDecTime
 *
 * Description : Set the time using BCD or decimal format. The individual 
 * data items are passed in throught the struct tm 
 *
 * Inputs : input time struct tm
 *
 * Returns : -1 on failure, 0 on success. 
 *
 * Error Conditions : bad handle or bad set
 * 
 * Unit Tested on: 06-Apr-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDecTime (BC_PCI_HANDLE hBC_PCI, struct tm time)
{
    struct  stfp_dec_tm val;
    uint16_t temp;
    if (hBC_PCI == 0)
    {
	return -1;
    }

    /* Struct tm composed of
     *          int tm_sec    seconds
     *          int tm_min    minutes
     *          int tm_hour   hours
     *          int tm_mday   day of the month
     *          int tm_mon    month
     *          int tm_year   year based on 1900
     *          int tm_wday   day of the week
     *          int tm_yday   day in the year
     *          int tm_isdst  daylight saving time
     * 
     * Time format passed into system. 
     * int16 year  full year
     * int16 days  julian day
     * int8  hours
     * int8  minutes
     * int8  seconds
     */

    temp         = time.tm_year + 1900;
    val.dec_year = htons(temp);
    val.dec_day  = htons(time.tm_yday);
    val.dec_hour = time.tm_hour;
    val.dec_min  = time.tm_min;
    val.dec_sec  = time.tm_sec;

    if (!bcSetTimeFormat (hBC_PCI, BCD_TIME))
    {
	return ioctl( hBC_PCI->fd, BTFP_MAJOR_TIME, &val);
    }
    return -1;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqYear
 *
 * Description : Request the current year 
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : 4 digit year
 *
 * Error Conditions : Usually a null pointer
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcReqYear (BC_PCI_HANDLE hBC_PCI, uint16_t* year)
{
    uint8_t rc = 0;
    uint8_t val[4];
    uint16_t *p;

    struct btfp_inarea *pcmd;
    if ((hBC_PCI == 0)||(year==0))
    {
	return -1;
    }
    memset(val, 0, sizeof(val));

    pcmd = (struct btfp_inarea *) val;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_YEAR;

    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pcmd);
    *year = 0;
    if (!rc)
    {
	if (val[0] == TFP_YEAR)
	{
	    p = (uint16_t *)&val[1];
	    *year = ntohs(*p);
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetYear
 *
 * Description : Set the current year on the timecard
 *
 * Inputs : hBC_PCI - handle to card
 *          year - year to send to card
 *
 * Returns : none
 *
 * Error Conditions : bad handle or bad set
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetYear (BC_PCI_HANDLE hBC_PCI, uint16_t year)
{
    uint16_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if ((year<1990) || (year>2036))
    {
	return -1;
    }
    val = htons(year);
    return ioctl( hBC_PCI->fd, BTFP_YEAR, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcReadEventTime
 *
 * Description : Function to read event time
 *
 * Inputs : hBC_PCI - handle to BC635
 *
 * Returns : ulpMaj - major time
 *           ulpMin - min time
 *           bpstat - status register
 *
 * Error Conditions : returns if any of the input pointers are null
 * 
 * Unit Tested on: 18-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReadEventTime (BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			  uint32_t* ulpMin, int8_t* bpstat)
{
    uint16_t ulpNano;
    return bcReadEventTimeEx (hBC_PCI, ulpMaj, ulpMin, &ulpNano, bpstat);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetStrobeTime
 *
 * Description : Set the strobe coincidence time.
 *
 * Inputs : hBC_PCI - handle to card
 *          dMaj = unsigned long value for strobe major time.
 *          dMin = unsigned long value for strobe minor time.
 *
 * Returns : none
 *
 * Error Conditions : Bad handle, bad write.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetStrobeTime (BC_PCI_HANDLE hBC_PCI, int16_t dMaj, int16_t dMin)
{
    struct timereg val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    val.time0 = htonl(dMin);
    val.time1 = htonl(dMaj);
    return ioctl( hBC_PCI->fd, BTFP_READ_STROBE, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcReqTimeFormat
 *
 * Description : Get the register time format
 *
 * Inputs : hBC_PCI - handle 
 *          timeformat - return value for format
 *
 * Returns : 
 *
 * Error Conditions : -1 on null pointers
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqTimeFormat (BC_PCI_HANDLE hBC_PCI, int8_t* timeformat)
{
    uint8_t            rc;
    struct btfp_inarea pin;

    if ((hBC_PCI == 0)||(timeformat==0))
    {
	return -1;
    }
    pin.cmd    = TFP_REQUEST_DATA;
    pin.subcmd = TFP_TIMEREG_FMT;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    *timeformat = pin.subcmd;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetTimeFormat
 *
 * Description : Set the regester time format either 
 *               UNIX - 32 bit epoch at January 1, 1970
 *               BCD  - consisting of a struct tm 
 *
 * Inputs :  tmfmt = BCD_TIME, UNIX_TIME
 *
 * Returns : 0 on success -1 on failure
 *
 * Error Conditions : time format not recognized. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetTimeFormat (BC_PCI_HANDLE hBC_PCI, int8_t tmfmt)
{
    if ((hBC_PCI == 0)||(tmfmt>1))
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_TIMEREG_FMT, &tmfmt);
}
/**
 ******************************************************************
 *
 * Function Name : bcReqInputFormat
 *
 * Description : Request input code format 
 *
 * Inputs : handle
 *
 * Returns : fmt - a packed word. 
 *               First byte:    ASCII value   Second Byte:
 *                                            0 or
 *               IRIG_A         A             Y - with year
 *               IRIG_B         B             Y - with year, T - truetime
 *               IRIG_E1K       E             Y - with year
 *               IRIG_E100      e             Y
 *               IRIG_G         G             Y
 *               IEEE_1344      I             0
 *               NASA36         N             0
 *               XR3            X             0
 *               2137           2             0
 *
 * Error Conditions : -1 on failure, usually bad request. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqInputFormat (BC_PCI_HANDLE hBC_PCI, int8_t *TcIn, 
			  int8_t *SubType)
{
    uint8_t            rc, val[4];
    struct btfp_inarea *pin;

    if ((hBC_PCI == 0)||(TcIn==0) || (SubType==0))
    {
	return -1;
    }
    pin = (struct btfp_inarea *)val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_INPUT_CODE_FMT;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (val[0] == TFP_INPUT_CODE_FMT)
	{
	    *TcIn    = val[1];
	    *SubType = val[2];
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqInputModulation
 *
 * Description : Request input code Modulation - version 2 
 * handles this better through extended call.
 *
 * Inputs : 
 *
 * Returns : mod - the modulation format used.
 *                 'M' amplitude modulated sine wave
 *                 'D' DC level shift pulse code mod	
 *
 * Error Conditions : -1 on bad request. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqInputModulation (BC_PCI_HANDLE hBC_PCI, int8_t* mod)
{
    uint8_t            rc;
    struct btfp_inarea in;

    if ((hBC_PCI == 0)||(mod==0))
    {
	return -1;
    }
    in.cmd    = TFP_REQUEST_DATA;
    in.subcmd = TFP_CODE_MODULATION;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &in);
    if (!rc)
    {
	if (in.cmd==TFP_CODE_MODULATION)
	{
	    *mod = in.subcmd;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqOutputFormat
 *
 * Description : Request output code format - version 2 
 *               handles this better through extended call.
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : TcOut   - timecode out
 *           SubType - does it contain year or any extended data? 
 *
 * Error Conditions : bad handle, null pointer, bad read
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqOutputFormat (BC_PCI_HANDLE hBC_PCI,int8_t *TcOut,
			   int8_t *SubType)
{
    uint8_t            rc, val[4];
    struct btfp_inarea *pin;

    if ((hBC_PCI == 0) || (TcOut==0) || (SubType==0))
    {
	return -1;
    }
    pin = (struct btfp_inarea*) val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_OUTPUT_CODE_FMT;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (val[0] == TFP_OUTPUT_CODE_FMT)
	{
	    *TcOut   = val[1];
	    *SubType = val[2];
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqClockSource
 *
 * Description : Request clock source - version 2 
 *               handles this better through extended call.
 *
 * Inputs : hBC_PCI - the card handle
 *
 * Returns : fmt - 0 internal, 1 external
 *
 * Error Conditions : bad handle or bad read.
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqClockSource (BC_PCI_HANDLE hBC_PCI, uint8_t* fmt)
{
    uint8_t            rc;
    struct btfp_inarea pin;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin.cmd    = TFP_REQUEST_DATA;
    pin.subcmd = TFP_CLOCK_SOURCE;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    if (!rc)
    {
	if (pin.cmd == TFP_CLOCK_SOURCE)
	{
	    *fmt = pin.subcmd;
	}
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqJamSync
 *
 * Description : Get the setting of the Jamsync
 *
 * Inputs : hBC_PCI - handle of card
 *
 * Returns : fmt: 0 - jamsync disabled, 1 - jamsync enabled
 *
 * Error Conditions : bad handle, null pointer, bad read
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqJamSync (BC_PCI_HANDLE hBC_PCI, uint8_t* fmt)
{
    uint8_t            rc;
    struct btfp_inarea pin;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin.cmd    = TFP_REQUEST_DATA;
    pin.subcmd = TFP_JAMSYNC_CTRL;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    if (!rc)
    {
	if (pin.cmd == TFP_JAMSYNC_CTRL)
	{
	    *fmt = pin.subcmd;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqBatteryStatus
 *
 * Description : Get the battery status
 *
 * Inputs : hBC_PCI - handle of card
 *
 * Returns : fmt: 0 - Battery connected, 1 - Battery disconnected
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqBatteryStatus (BC_PCI_HANDLE hBC_PCI, uint8_t* fmt)
{
    uint8_t            rc, val[4];
    uint16_t           rv;
    struct btfp_inarea *pin;
    rv = 0;
    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin = (struct btfp_inarea *) val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_BATTERY_STATUS;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    if (!rc)
    {
	if (val[0] == TFP_BATTERY_STATUS)
	{
	    memcpy( &rv, &val[1], sizeof(uint16_t));
	    *fmt = rv;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetLocalFlag
 *
 * Description : This command programs the local time observed flag. 
 *               If the local time flag is enabled, the TFPadjusts its 
 *               time by the local time offset. Note that the Generator 
 *               Time is also affected by this setting.See Command 0x1d 
 *               for programming local time offset.
 *
 * Inputs : hBC_PCI - handle to card
 *          flagoff - 0 -do not observe, 1, observe
 * 
 * Returns : none
 *
 * Error Conditions : bad handle or bad read
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetLocalFlag (BC_PCI_HANDLE hBC_PCI, int8_t flagoff)
{
    int8_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (flagoff)
	val = 1;
    else
	val = 0;
    return ioctl( hBC_PCI->fd, TFP_LOCALTZ_OBS, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcReqLocalTZObs
 *
 * Description : Return if local TZ is observed
 *
 * Inputs : hBC_PCI handle of card
 *
 * Returns : fmt: 0 - no it is not observed, 1 it is. 
 *
 * Error Conditions : Bad handle, null pointer, 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqLocalTZObs (BC_PCI_HANDLE hBC_PCI, uint8_t* fmt)
{
    uint8_t            rc;
    struct btfp_inarea pin;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin.cmd    = TFP_REQUEST_DATA;
    pin.subcmd = TFP_LOCALTZ_OBS;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    if (!rc)
    {
	if (pin.cmd == TFP_LOCALTZ_OBS)
	{
	    *fmt = pin.subcmd;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqLeap
 *
 * Description : Gets the leap flag setting. The documentation is a bit
 * unclear this may return just the flag or the flag and event time. 
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : Leap Second Flag:
 *           1 = Insertionl 
 *          -1 = Deletion (0xff)
 * 	     0 = Disable
 *
 * Error Conditions : bad handle, null pointer, bad read
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqLeap (BC_PCI_HANDLE hBC_PCI, int8_t* fmt)
{
    uint8_t            rc;
    struct btfp_inarea pin;
    //struct leapseconds leapevt;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin.cmd    = TFP_REQUEST_DATA;
    pin.subcmd = TFP_LEAP_SETTING;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &pin);
    if (!rc)
    {
	if (pin.cmd == TFP_LEAP_SETTING)
	{
	    *fmt = pin.subcmd;
	}
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqDAC
 *
 * Description : The TFP 10MHz oscillator frequency is voltage controlled
 *               using the output from a 16-bit DAC. Thiscommand allows 
 *               the user to directly load a 16-bit value to the DAC. 
 *               This feature allows the user tofine tune the TFP time 
 *               base when operating in the Free Running Mode. This voltage 
 *               is also routed outof the TFP via the J1 connector (pin 9) 
 *               allowing external oscillators to be disciplined. The DAC 
 *               outputvoltage ranges nominally from 0 V (value = 0x0000) 
 *               to 5 V (value = 0xFFFF).
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : fmt - the value of the DAC. 
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqDAC (BC_PCI_HANDLE hBC_PCI, uint16_t* dacval)
{
    uint8_t            rc, val[6];
    struct btfp_inarea *pin;
    uint16_t           *p;

    if ((hBC_PCI == 0)||(dacval==0))
    {
	return -1;
    }
    memset( val, 0, sizeof(val));
    pin = (struct btfp_inarea *) val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_LOAD_DAC;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (val[0] == TFP_LOAD_DAC)
	{
	    p = (uint16_t *) &val[1];
	    *dacval = ntohs(*p);
	}
	if (gVerbose>1)
	{
	    printf("%s dac: %X %X %X\n", __FUNCTION__, 
		   val[1], val[2], *dacval);
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDAC
 *
 * Description : The TFP~s 10MHz oscillator frequency is voltage controlled 
 *               using the output from a 16-bit DAC. Thiscommand allows 
 *               the user to directly load a 16-bit value to the DAC. 
 *               This feature allows the user tofine tune the TFP time 
 *               base when operating in the Free Running Mode. This voltage 
 *               is also routed outof the TFP via the J1 connector (pin 9) 
 *               allowing external oscillators to be disciplined. The DAC 
 *               outputvoltage ranges nominally from 0 V (value = 0x0000) 
 *               to 5 V (value = 0xFFFF).
 *
 * Inputs : hBC_PCI - handle to card
 *          dac - the value of the DAC. 
 *
 * Returns : none
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDAC( BC_PCI_HANDLE hBC_PCI, uint16_t dac)
{
    uint16_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    val = htons(dac);
    if (gVerbose>0)
    {
	printf("%s Input value: %X format value: %X\n", 
	       __FUNCTION__,dac, val);
    }
    return ioctl( hBC_PCI->fd, BTFP_LOAD_DAC, &val);
}
/**
 ******************************************************************
 *
 * Function Name : bcReqPeriodics
 *
 * Description : Return Periodics values
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : fmt - The periodics structure containing
 *           sync1pps if the edge is synchronized to the 1PPS iput
 *           n1 - divisor 1
 *           n2 - divisor 2
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqPeriodics (BC_PCI_HANDLE hBC_PCI, struct periodic* fmt)
{
    uint8_t            rc, val[6];
    uint16_t           *p;
    struct btfp_inarea *pin;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    memset( val, 0, sizeof(val));
    pin = (struct btfp_inarea *) val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_PPO;

    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (val[0] == TFP_PPO)
	{
	    fmt->sync1pps = val[1];
	    p = (uint16_t*) &val[2];
	    fmt->n1 = ntohs(*p);
	    p = (uint16_t*)&val[4];
	    fmt->n2 = ntohs(*p);
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetHbt
 *
 * Description : Set Heartbeat or setup periodics
 *
 * Inputs : hBC_PCI - handle to card
 *          mode = 0 async or 1 sync to 1pps. 
 *          n1 - n1 divisor
 *          n2 - n2 divisor
 *
 * Returns : 0 on success -1 on failure
 *
 * Error Conditions : Bad write
 * 
 * Unit Tested on: 17-Mar-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetHbt (BC_PCI_HANDLE hBC_PCI, int8_t mode, uint16_t n1, 
		   uint16_t n2)
{
    uint8_t  val[6];
    uint16_t *p;

    if (hBC_PCI == 0)
    {
	return -1;
    }
    val[0] = mode;
    p      = (uint16_t *)&val[1];
    *p     = htons(n1);
    p      = (uint16_t *)&val[3];
    *p     = htons(n2);

    return ioctl( hBC_PCI->fd, BTFP_PPO, val);
}

/**
 ******************************************************************
 *
 * Function Name : bcReqPropDelay
 *
 * Description : It is sometimes desired to program an offset into the 
 *               basic TFP time keeping functions relative to the 
 *               reference input. For example, if the reference input is 
 *               an IRIG B time code, there may be significantcable delay 
 *               between the IRIG B generator and the TFP location. 
 *               This command allows this time differenceto be removed 
 *               by inserting the known amount of offset between the 
 *               IRIG B reference andTFP location. The offset is 
 *               programmable in units of 100 nanoseconds, and may 
 *               be positive or negative.
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : fmt - number of 100's of nanoseconds of delay
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcReqPropDelay (BC_PCI_HANDLE hBC_PCI, int32_t* fmt)
{
    uint8_t             rc;
    struct propdelay    prop;
    struct btfp_inarea* pin;

    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }

    pin = (struct btfp_inarea*) &prop;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_PROP_DELAY;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (prop.id == TFP_PROP_DELAY)
	{
	    *fmt = prop.propdelay;
	}
	else
	{
	    *fmt = 0;
	    return -1;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqGeneratorOffset
 *
 * Description : This command is used to add/subtract an offset to the 
 *               time code generator output. This commandaffects the 
 *               generator output only.
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : generator offset structure containing hour value and half 
 *           hour flag
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t bcReqGeneratorOffset (BC_PCI_HANDLE hBC_PCI, 
			      struct generatorOffset_t* fmt)
{
    uint8_t rc = 0;
    struct btfp_inarea* pin;
    uint8_t  val[4];
    uint16_t *p;
    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin = (struct btfp_inarea*) val; //fmt;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_GEN_OFFSET;

    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    fmt->id = val[0];
    p = (uint16_t*) &val[1];
    fmt->LocalOffset = ntohs(*p);
    fmt->HalfHour    = val[3];
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqLocalOffset 
 *
 * Description : Get the local offset value
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : generator offset structure containing hour value and half 
 *           hour flag
 *
 * Error Conditions : bad handle, null pointer, bad read. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t bcReqLocalOffset (BC_PCI_HANDLE hBC_PCI, 
			      struct generatorOffset_t* fmt)
{
    uint8_t             val[4];
    uint16_t            *p, rc;
    struct btfp_inarea* pin;

    rc = 0;
    if ((hBC_PCI == 0)||(fmt==0))
    {
	return -1;
    }
    pin = (struct btfp_inarea*) val; //fmt;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_LOCAL_TZ;

    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (val[0] == TFP_LOCAL_TZ)
    {
	fmt->id = val[0];
	p = (uint16_t*) &val[1];
	fmt->LocalOffset = ntohs(*p);
	fmt->HalfHour    = val[3];
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcSetMode
 *
 * Description : Set the TFP mode
 *
 * Inputs : hBC_PCI - handle to cardhandle to bc pci structure
 *          mode
 *              TIMEMODE_CODE     0x00 Irig A/B, IEEE 1344/NASA36
 *              TIMEMODE_FREERUN  0x01 10Mhz reference, int | ext
 *              TIMEMODE_PPS      0x02 External 1 PPS signal
 *              TIMEMODE_RTC      0x03 tfp onboard RTC IC. 
 *              TIMEMODE_GPS      0x06 GPS. bc637 only.	
 * 
 * Returns : none
 *
 * Error Conditions : -1 on error, usually a null pointer
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t bcSetMode (BC_PCI_HANDLE hBC_PCI, int8_t mode)
{
    struct btfp_inarea cmd;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    cmd.cmd    = TFP_TIMEMODE;
    cmd.subcmd = mode; 
    return ioctl(hBC_PCI->fd, TFP_TIMEMODE, &cmd);
}
/**
 ******************************************************************
 *
 * Function Name : bcReqMode
 *
 * Description : 
 *
 * Inputs : hBC_PCI - handle to cardhandle to bc pci structure
 *
 * Returns : mode - 
 *              TIMEMODE_CODE     0x00 Irig A/B, IEEE 1344/NASA36
 *              TIMEMODE_FREERUN  0x01 10Mhz reference, int | ext
 *              TIMEMODE_PPS      0x02 External 1 PPS signal
 *              TIMEMODE_RTC      0x03 tfp onboard RTC IC. 
 *              TIMEMODE_GPS      0x06 GPS. bc637 only.	
 * 
 *
 * Error Conditions : -1 on error, usually a null pointer
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t bcReqMode (BC_PCI_HANDLE hBC_PCI, uint8_t *mode)
{
    uint8_t rc;
    struct btfp_inarea cmd;

    if ((hBC_PCI == 0) || (mode == 0))
    {
	return -1;
    }
    cmd.cmd       = TFP_REQUEST_DATA;
    cmd.subcmd    = TFP_TIMEMODE;

    rc = ioctl(hBC_PCI->fd, TFP_REQUEST_DATA, &cmd);
    if (!rc)
    {
	if (cmd.cmd == TFP_TIMEMODE)
	{
	    *mode = cmd.subcmd;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetLocOff
 *
 * Description : This is the 1D call
 * This command adds/subtracts local time offset to the TFP time. 
 * This command affects the TFP timeonly; the generator time is not affected. 
 * (See Command 0x1C.)
 *
 * Inputs : hBC_PCI - handle to card
 *          offset - hours from UTC
 *          half = 0, no half hour offset
 *                 1, add in a half hour offset. 
 *
 * Returns : none
 *
 * Error Conditions : bad handle, or bad set
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetLocOff (BC_PCI_HANDLE hBC_PCI, int16_t offset, int8_t half)
{
    if ((hBC_PCI == 0)|| (abs(offset)>16))
    {
	return -1;
    }
    printf("BC set local offset: %d %d\n", offset, half);
#if 0
    struct loctmoffset  val;
    val.locoff = htons(offset);
    if(!half)
    {
	val.locflg = 0;
    }
    else
    {
	val.locflg = 1;
    }
#else
    uint8_t val[4], *p;

    if ((hBC_PCI == 0)|| (abs(offset)>16))
    {
	return -1;
    }
    p = (uint8_t*) &offset;

    val[0] = p[1];
    val[1] = p[0];
    if(!half)
    {
	val[2] = 0;
    }
    else
    {
	val[2] = 1;
    }
    val[3] = 0;
#endif
    return ioctl(hBC_PCI->fd, BTFP_LOCAL_TZ, &val);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetGenOff
 *
 * Description : Programs the board time code generator to operate at an
 *               offset from UTC. This is the 0x1C call
 * Byte:
 *   0 - ID: 0x1C
 *   1 -  INT16 -16 to 16
 *   2 - 
 *   3 - Half hour: 0 or 1. 
 * 
 * Inputs : hBC_PCI - handle to card
 *          offset  - hours from UTC
 *          half    - flag to include half hour
 *
 * Returns : none
 *
 * Error Conditions : bad handle or offset is greater than 16
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetGenOff (BC_PCI_HANDLE hBC_PCI, int16_t offset, int8_t half)
{
    uint8_t val[4], *p;
    //uint16_t *q;

    if ((hBC_PCI == 0)|| (abs(offset)>16))
    {
	return -1;
    }
    p = (uint8_t*) &offset;

    val[0] = p[1];
    val[1] = p[0];
    if(!half)
    {
	val[2] = 0;
    }
    else
    {
	val[2] = 1;
    }
    val[3] = 0;
    //q = (uint16_t *) p;
    //printf("bcuser.c: %d 0x%X\n", offset, *q);
    return ioctl(hBC_PCI->fd, BTFP_GEN_OFFSET, val); /* 0x1C */
}
/**
 ******************************************************************
 *
 * Function Name : bcSetPropDelay
 *
 * Description : Programs a propagation delay into the timing engine to 
 *               account for delays introduced bylong cable runs.
 *
 * Inputs : hBC_PCI - handle to card
 *          delay - propagation delay (-9999999 to +9999999 100ns steps)
 *
 * Returns : none
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetPropDelay (BC_PCI_HANDLE hBC_PCI, int32_t delay)
{
    uint32_t value;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    value = htonl(delay);
    return ioctl(hBC_PCI->fd, BTFP_PROP_DELAY, &value);
}


/**
 ******************************************************************
 *
 * Function Name : bcSetTcInEx
 *
 * Description : Set input time code format
 *
 * Inputs : Time code format
 *          Subtype
 *   TCODE_2137, TCODE_IRIG_SUBTYPE_NONE    (2137)
 *   TCODE_XR3, TCODE_IRIG_SUBTYPE_NONE     (XR3)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_Y     (AY - IRIG A with year)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_Y     (BY - IRIG B with year)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_T     (BT - IRIG B Legacy TrueTime)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_NONE  (E  - IRIG E 1000Hz no year)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_Y     (EY - IRIG E 1000Hz with year)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_NONE  (e  - IRIG E 100Hz no year)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_Y     (eY - IRIG E 100Hz with year)
 *   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_NONE  (G  - IRIG G no year)
 *   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_Y     (GY - IRIG G with year)
 *
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetTcInEx (BC_PCI_HANDLE hBC_PCI, int8_t TcIn, int8_t SubType)
{
    uint8_t val[2];

    if (hBC_PCI == 0)
    {
	return -1;
    }
    switch( TcIn)
    {
    case IRIG_A:
    case IRIG_B:
    case IRIG_E1K:
    case IRIG_E100:
    case IRIG_G:
	val[0] = TcIn;
	val[1] = SubType;
	break;
    case IEEE_1344:
    case NASA36:
    case XR3:
	val[0] = TcIn;
	val[1] = 0;
	break;
    default:
	return -1;
	break;
    }
    return ioctl(hBC_PCI->fd, BTFP_INPUT_CODE_FMT, val);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetTcIn
 *
 * Description : Set input time code format
 *
 * Inputs : Time code format
 *                   IRIG_A, IRIG_B, IEEE_1344,NASA36
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetTcIn (BC_PCI_HANDLE hBC_PCI, uint8_t TcIn)
{
    return bcSetTcInEx( hBC_PCI, TcIn, 0);
}


/**
 ******************************************************************
 *
 * Function Name : bcSetTcInMod 
 *
 * Description : Set the input modulation to value indicated
 *
 * Inputs : mod, 'M' modulated sine, D - DC level shift
 *
 * Returns : 0 on success -1 on failure.
 *
 * Error Conditions :
 * 
 * Unit Tested on: 24-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetTcInMod (BC_PCI_HANDLE hBC_PCI, int8_t mod)
{
    if (hBC_PCI == 0)
    {
	return -1;
    }
    switch (mod)
    {
    case AM_SINE:
    case DCLS_PCM:
	return ioctl( hBC_PCI->fd, BTFP_CODE_MODULATION, &mod);
	break;
    }
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetGenCodeEx
 *
 * Description : 
 *
 * Inputs : 
 *
 * However, bcSetGenCodeEx supports the following new codes.
 *
 *   TCODE_2137, TCODE_IRIG_SUBTYPE_NONE    (2137)
 *   TCODE_XR3, TCODE_IRIG_SUBTYPE_NONE     (XR3)
 *   TCODE_NASA, TCODE_IRIG_SUBTYPE_NONE    (NASA)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_0     (A0 - IRIG A BCD,CF,SBS)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_1     (A1 - IRIG A BCD,CF)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_2     (A2 - IRIG A BCD)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_3     (A3 - IRIG A BCD,SBS)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_4     (A4 - IRIG A BCD,YEAR,CF,SBS)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_5     (A5 - IRIG A BCD,YEAR,CF)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_6     (A6 - IRIG A BCD,YEAR)
 *   TCODE_IRIG_A, TCODE_IRIG_SUBTYPE_7     (A7 - IRIG A BCD,YEAR,SBS)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_0     (B0 - IRIG B BCD,CF,SBS)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_1     (B1 - IRIG B BCD,CF)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_2     (B2 - IRIG B BCD)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_3     (B3 - IRIG B BCD,SBS)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_4     (B4 - IRIG B BCD,YEAR,CF,SBS)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_5     (B5 - IRIG B BCD,YEAR,CF)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_6     (B6 - IRIG B BCD,YEAR)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_7     (B7 - IRIG B BCD,YEAR,SBS)
 *   TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_T     (BT - IRIG B BCD,CF,SBS - Legacy TrueTime)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_1     (E1 - IRIG E 1000Hz BCD,CF)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_2     (E2 - IRIG E 1000Hz BCD)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_5     (E5 - IRIG E 1000Hz BCD,YEAR,CF)
 *   TCODE_IRIG_E, TCODE_IRIG_SUBTYPE_6     (E6 - IRIG E 1000Hz BCD,YEAR)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_1     (e1 - IRIG E 100Hz BCD,CF)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_2     (e2 - IRIG E 100Hz BCD)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_5     (e5 - IRIG E 100Hz BCD,YEAR,CF)
 *   TCODE_IRIG_e, TCODE_IRIG_SUBTYPE_6     (e6 - IRIG E 100Hz BCD,YEAR)
 *   TCODE_IRIG_G, TCODE_IRIG_SUBTYPE_5     (G5 - IRIG G BCD,YEAR,CF)
 *

 * Returns : none
 *
 * Error Conditions : bad handle or bad read. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetGenCodeEx (BC_PCI_HANDLE hBC_PCI, int8_t GenTc, int8_t SubType)
{
    uint8_t val[2];
    if (hBC_PCI == 0)
    {
	return -1;
    }
    val[0] = GenTc;
    val[1] = SubType;
    return ioctl( hBC_PCI->fd, BTFP_OUTPUT_CODE_FMT, val);
}
uint8_t bcSetGenCode (BC_PCI_HANDLE hBC_PCI, int8_t GenTc)
{
    return bcSetGenCodeEx( hBC_PCI, GenTc, 0);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetLeapEvent
 *
 * Description : set the flag and the value. Command 0x1E
 *
 * Inputs : hBC_PCI - handle to card
 *          flag = -1 = Deletion of 1 second 
 *                  0 = Disable
 *                  1 = Insertion of 1 second
 *          levent = Unix time since January, 1st 1970 (This may or may not 
 *                                                      work, documentation 
 *                                                      is unclear
 *
 * Returns : none
 *
 * Error Conditions : bad handle or read
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetLeapEvent (BC_PCI_HANDLE hBC_PCI, int8_t flag, uint32_t levent)
{
    struct leapseconds leapevt;

    if (hBC_PCI == 0)
    {
	return -1;
    }
    leapevt.ls_flag = flag;
    leapevt.leap_tm = htonl(levent);
    return ioctl( hBC_PCI->fd, BTFP_LEAP_SETTING, &leapevt);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetClkSrc
 *
 * Description : Set Clock source, external/interal
 *
 * Inputs : hBC_PCI - handle to card
 *          clk - 0 internal, 1 external
 *
 * Returns : none
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetClkSrc (BC_PCI_HANDLE hBC_PCI, int8_t clk)
{
    if (hBC_PCI == 0)
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_CLOCK_SOURCE, &clk);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetGain
 *
 * Description : This command allows the gain and sense of the disciplining 
 *               process to be set by the user. A positive gain indicates 
 *               that the voltage-controlled oscillator source frequency 
 *               increases with increasing controlvoltage. This feature 
 *               is valuable to anyone using the TFP to discipline an 
 *               external oscillator.
 *
 * Inputs : hBC_PCI - handle to card
 *          gain - a value from (-100:100)
 *
 * Returns : none
 *
 * Error Conditions : bad handle, gain out of bounds
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcSetGain (BC_PCI_HANDLE hBC_PCI, int16_t gain)
{
    uint16_t val;
    if ((hBC_PCI == 0) || (abs(gain)>100))
    {
	return -1;
    }
    val = htons(gain);
    if (gVerbose>0)
    {
	printf("%s Input value: %X format value: %X\n", 
	       __FUNCTION__,gain, val);
    }
    return ioctl( hBC_PCI->fd, BTFP_DISCIPLIN_GAIN, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetDis
 *
 * Description : Enable or disable the discipline control
 *
 * Inputs : hBC_PCI - handle to card
 *          dis - 0) off, 1 on.
 *
 * Returns : none
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t bcSetDis( BC_PCI_HANDLE hBC_PCI, uint8_t dis)
{
#if 0
    uint16_t val = 0;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (dis) val = 1;
    return ioctl( hBC_PCI->fd, BTFP_OSCDISCP_CTRL, &val);
#else
    return 1; // Not supported in PCI v2 hardware.
#endif
}

/**
 ******************************************************************
 *
 * Function Name : bcReqGain
 *
 * Description : Return Gain data
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t bcReqGain (BC_PCI_HANDLE hBC_PCI, int16_t* gain)
{
    uint8_t            rc, val[4];
    uint16_t           *p;
    struct btfp_inarea *pin;

    if ((hBC_PCI == 0)||(gain==0))
    {
	return -1;
    }
    memset(val, 0, sizeof(val));
    pin = (struct btfp_inarea *) val;
    pin->cmd    = TFP_REQUEST_DATA;
    pin->subcmd = TFP_DISCIPLIN_GAIN;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pin);
    if (!rc)
    {
	if (val[0] == TFP_DISCIPLIN_GAIN)
	{
	    p = (uint16_t *) &val[1];
	    *gain = ntohs(*p);
	    if (gVerbose>1)
	    {
		printf("%s gain: %X %X %X\n", __FUNCTION__, 
		       val[1], val[2], *gain);
	    }
	}
	else
	{
	    rc = -1;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetJam
 *
 * Description : Enable or Disable Jamsync
 * Jamsync: Is the process of abruptly synchronizing with a time 
 * reference, as opposed to gradually adjusting to match up with 
 * the time reference.
 *
 * Inputs : hBC_PCI - handle to card
 *          jam - 0) disable, 1) Enable
 *
 * Returns : none
 *
 * Error Conditions : bad handle or bad write
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetJam (BC_PCI_HANDLE hBC_PCI, int8_t jam)
{
    if ((hBC_PCI == 0) || (jam<0) || (jam>1))
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_JAMSYNC_CTRL, &jam);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetGpsTmFmt
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
uint8_t  bcSetGpsTmFmt (BC_PCI_HANDLE hBC_PCI, int8_t gpsfmt)
{
    printf("bcSetGpsTmFmt not implementd\n");
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetGpsOperMode 
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
uint8_t  bcSetGpsOperMode (BC_PCI_HANDLE hBC_PCI, int8_t gpsmode)
{
    printf("bcSetGpsMode not implemented.\n");
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcSetYearAutoIncFlag 
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
uint8_t  bcSetYearAutoIncFlag (BC_PCI_HANDLE hBC_PCI, int8_t yrinc)
{
    printf("%s not implemented\n", __FUNCTION__);
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcAdjustClock
 *
 * Description : This command advances or retards the TFP internal clock. 
 * The TFP can adjust its clockup to 100 milliseconds per each second. 
 * Each count is equal to 10 microseconds. Command 0x29
 *
 * Inputs : 
 *     LONG cval = 0x80000000 to 0x7FFFFFFF
 *
 * Returns : none
 *
 * Error Conditions : Error performing set. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcAdjustClock (BC_PCI_HANDLE hBC_PCI, int32_t cval)
{
#if 0
    if (hBC_PCI == 0)
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_CLOCK_SLEW, &cval);
#else
    /* Not supported on v2 or PCI hardware! */
    return 1;
#endif
}
/**
 ******************************************************************
 *
 * Function Name : bcCommand
 *
 * Description : Only commands with no arguments permitted. These are:
 *               Software reset, and Force Jam Sync
 *
 * Inputs : command
 *
 * Returns : none
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
void  bcCommand (BC_PCI_HANDLE hBC_PCI, int8_t cmd)
{
    int8_t cval = 0;
    if (hBC_PCI == 0)
    {
	return;
    }
    switch (cmd)
    {
    case TFP_SOFTWARE_RESET:
    case TFP_FORCE_JAMSYNC:
    case TFP_BATT:
	ioctl( hBC_PCI->fd, cmd, &cval);
	break;
    }
}
/**
 ******************************************************************
 *
 * Function Name : bcForceJam
 *
 * Description : This command forces the TFP to Jam-Sync on the next 
 *               rising edge of the 1PPS output.The Jam-Sync bit must 
 *               be enabled before using this command.
 *
 * Inputs : handle to card
 *
 * Returns : none
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcForceJam (BC_PCI_HANDLE hBC_PCI)
{
    if (hBC_PCI == 0)
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_FORCE_JAMSYNC, 0);
}
/**
 ******************************************************************
 *
 * Function Name : bcSyncRtc 
 *
 * Description : This command forces the TFP to Synchronize 
 *               the RTC time to current time
 *
 * Inputs : handle to card
 *
 * Returns : none
 *
 * Error Conditions : bad handle returns -1
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSyncRtc (BC_PCI_HANDLE hBC_PCI)
{
    if (hBC_PCI == 0)
    {
	return -1;
    }
    return ioctl( hBC_PCI->fd, BTFP_SYNC_RTC, 0);
}
/**
 ******************************************************************
 *
 * Function Name : bcDisRtcBatt
 *
 * Description : This command disconnects the RTC IC from the Battery 
 *               after power is turned off. Upon power on, 
 *               the TFP automatically connects the RTC IC to the battery.
 *
 * Inputs : handle to card
 *          conn - 0 disconnect, 1 connect
 * 
 *
 * Returns : none
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcRtcBatt (BC_PCI_HANDLE hBC_PCI, uint8_t conn)
{
    uint16_t val = 0;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (conn) val = 1;

    return ioctl( hBC_PCI->fd, BTFP_BATT, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcReqOscData
 *
 * Description : Request Oscillator data of the board. 
 *
 * Inputs : hBC_PCI  - handle to card
 *
 * Returns : OscData structure filled
 *                uint8_t  disctl    Disciplining Enable/Disable
 *                uint8_t  phasectl  phase control Enable/Disable
 *                uint16_t phasestep  
 *                int16_t  DisGain   Disciplining Gain
 *                int32_t  con       Not sure what this is. 
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
// Functions to request the card's configuration
uint8_t  bcReqOscData (BC_PCI_HANDLE hBC_PCI, OscData *osc)
{
    uint8_t rc;
#if 0
    uint8_t val[12];
    uint16_t           *p16;
    uint32_t           *p32;
    struct btfp_inarea *pcmd;

    /* 
     * 0x19
     * New as of 8-Mar-13 
     * uint8_t  disctl;    Disciplining Enable/Disable
     * uint8_t  phasectl;  phase control Enable/Disable
     * uint16_t phasestep;  
     * int16_t  DisGain;   Disciplining Gain
     * int32_t  con;       Not sure what this is. 
     * OLD
     * int8_t  BattStat;   Real Time Clock Battery status
     * int8_t  ClkSrc;	   Selected Clock input
     * int8_t  DisCtl;	   Disciplining Enable/Disable
     * int16_t DisGain;	   Disciplining Gain
     * int16_t DacVal;	   DAC Value
     * int8_t  JamCtl;	   Jamsynch Enable/Disable
     * long    AdjClk;	   Advance/Retard Clock Value
     * } OscData;
     *
     */
    if ((hBC_PCI == 0) || (osc ==0))
    {
	return -1;
    }
    memset(val, 0, sizeof(val));
    pcmd = (struct btfp_inarea *) val;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_OSCDISCP_CTRL;

    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, pcmd);

    if(!rc)
    {
	if (val[0] == TFP_OSCDISCP_CTRL)
	{
	    osc->disctl   = val[1];
	    osc->phasectl = val[2];
	    p16 = (uint16_t *)&val[3];
	    osc->phasestep = *p16;
	    p32 = (uint32_t *)&val[5];
	    osc->DisGain  = *p32;
	    p16 = (uint16_t *)&val[3];
	    osc->con = *p16;
	}
    }
#else
    rc = 1; // Not supported on PCI v2 hardware
#endif
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqTimeCodeData
 *
 * Description : Request time code data of the board.
 *
 * Inputs : 
 *
 * Returns :
 *          int8_t  format	Time Code Decode Type 
 *          int8_t  modulation  Time Code Decode Modulation
 *          int8_t  gencode	Time Code Generator Type
 *          short   genoffset   Time Code Generator Offset
 *          int8_t  ghfhr       Time Code Gen Offset half Hour flag 0/1
 *
 * Error Conditions :not implemented
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqTimeCodeData (BC_PCI_HANDLE hBC_PCI, TimeCodeData *data)
{
    /*
     * int8_t  format;	   Time Code Decode Type 
     * int8_t  modulation; Time Code Decode Modulation
     * int8_t  gencode;	   Time Code Generator Type
     * short   genoffset;  Time Code Generator Offset
     * int8_t  ghfhr;	   Time Code Gen Offset half Hour flag 0/1
     */
    printf("%s not implemented\n", __FUNCTION__);
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqTimeData
 *
 * Description : Request time data of the board.
 *
 * Inputs : 
 *
 * Returns : TimeData Structure
 *           int8_t  mode     Selected Reference mode
 *           int8_t  tmformat Time format (i.e. bin or bcd)
 *           int16_t year     Current year data
 *           int8_t  gpstmfmt GPS or UTC time format (0=UTC, 1=GPS)
 *           int8_t  leapsec  Current leap second count
 *           char    leapevt  Current leap second event type (1=ins,0=no,-1=del)
 *           int16_t leapevttm Scheduled time for leap second event
 *           short   locoff    Local time offset
 *           int8_t  lhlfhr    Local time offset half hour flag
 *           long    propdelay Propgation delay
 *           int8_t  dlight    IEEE Daylight Savings Flag 
 *           int8_t  localt    Local Time Flag Enable/Disable
 *
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
uint8_t  bcReqTimeData (BC_PCI_HANDLE hBC_PCI, TimeData *data)
{
    uint8_t flag;
    struct generatorOffset_t off;

    if (data == 0)
	return -1;
    /*
     * int8_t  mode	  Selected Reference mode
     * int8_t  tmformat   Time format (i.e. bin or bcd)
     * int16_t year	  Current year data
     * int8_t  gpstmfmt	  GPS or UTC time format (0=UTC, 1=GPS)
     * int8_t  leapsec	  Current leap second count
     * char    leapevt	  Current leap second event type (1=ins,0=no,-1=del)
     * int16_t leapevttm  Scheduled time for leap second event
     * short   locoff	  Local time offset
     * int8_t  lhlfhr	  Local time offset half hour flag
     * long    propdelay  Propgation delay
     * int8_t  dlight	  IEEE Daylight Savings Flag 
     * int8_t  localt	  Local Time Flag Enable/Disable
     */
    data->mode = 0;
    if (bcReqMode (hBC_PCI, &data->mode))
	return -1;

    if(bcReqTimeFormat( hBC_PCI, &data->tmformat))
	return -2;
    if(bcReqYear( hBC_PCI, &data->year))
	return -3;
    data->gpstmfmt = 0;   // Only coded for bc635
    data->leapsec  = 0;
    if(bcReqLeap( hBC_PCI, &data->leapevt))
	return -4;
    data->leapevttm = 0;
    if (bcReqLocalOffset( hBC_PCI, &off))
	return -5;
    data->locoff = off.LocalOffset;
    data->lhlfhr = off.HalfHour;

    if (bcReqPropDelay( hBC_PCI, &data->propdelay))
	return -6;

    if (GetIEEE1344Flag( hBC_PCI, &flag))
	return -7;
    if((flag&0x08)>0)
    {
	data->dlight = 1;
    }
    else
    {
	data->dlight = 0;
    }
    if ((flag&0x10)>0)
    {
	data->localt = 1;
    }
    else
    {
	data->localt = 0;
    }
    return 0;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqOtherData
 *
 * Description : Request other data of the board.
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : not implemented
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqOtherData (BC_PCI_HANDLE hBC_PCI, OtherData *data)
{
    /*
     * int8_t hbtmode	  // Heartbeat/Periodic mode (0=async,1=sync)
     * uint16_t hbtcnt1	  // Heartbeat/Periodic n1
     * uint16_t hbtcnt2   // Heartbeat/Periodic n2
     * int8_t freq        // Current freq output (1,5,10MHz)
     * int8_t evtctl      // Enable/Disable events
     * int8_t evtsense    // Event trigger on edge (1=falling,0=rising)
     * int8_t evtlock     // Enable/Disable event capture lockout
     * int8_t evtsrc      // Event trigger source (0=evt,1=hbt)
     */

    printf("%s not implemented\n", __FUNCTION__);
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqVerData
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : not implemented
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqVerData (BC_PCI_HANDLE hBC_PCI, VerData *data)
{
    printf("%s not implemented\n", __FUNCTION__);
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqModel
 *
 * Description : Request model data of the board.
 *
 * Inputs : none
 *
 * Returns : Model data structure. 
 *   int8_t model1	Model 'B'
 *   int8_t model2;	Model 'C'
 *   int8_t model3	Model '6'
 *   int8_t model4	Model '3'
 *   int8_t model5	Model '5' or '7'
 *   int8_t model6	Model 'P'
 *   int8_t model7	Model 'C'
 *   int8_t model8	Model 'I'
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqModel (BC_PCI_HANDLE hBC_PCI, ModelData *data)
{
    uint8_t rc = 0;
    struct model m;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(data==0))
    {
	return -1;
    }
    memset( &m, 0, sizeof(struct model));
    pcmd = (struct btfp_inarea *)&m;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_MODEL;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (m.id == TFP_MODEL)
	{
	    memcpy (data, m.model, 8*sizeof(int8_t));
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqSerialNum
 *
 * Description : Request serial number of the board.
 *
 * Inputs : hBC_PCI - handle to board
 *
 * Returns : 32 bit number containing serial number
 *
 * Error Conditions : bad handle
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqSerialNum (BC_PCI_HANDLE hBC_PCI, uint32_t* serial)
{
    uint8_t rc = 0;
    struct serial      ser;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(serial==0))
    {
	return -1;
    }
    memset( &ser, 0, sizeof(struct serial));
    pcmd = (struct btfp_inarea *)&ser;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_SERIAL;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (ser.id == TFP_SERIAL)
	{
	    *serial = ntohl((uint32_t)ser.sn);
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqHardwareFab
 *
 * Description : Request hardware fab of the board.
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : fab number of card
 *
 * Error Conditions : bad handle or bad read.
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcReqHardwareFab (BC_PCI_HANDLE hBC_PCI, uint16_t* fab)
{
    uint8_t rc = 0;
    uint8_t val[4];
    uint16_t *p;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(fab==0))
    {
	return -1;
    }
    memset ( val, 0, sizeof(val));
    pcmd = (struct btfp_inarea *) val;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_HDW_FAB;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (val[0] == TFP_HDW_FAB)
	{
	    p    = (uint16_t *) &val[1];
	    *fab = ntohs(*p);
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqAssembly
 *
 * Description : Request assembly number of the board.
 *
 * Inputs : hBC_PCI - handle of board
 *
 * Returns : 16 bit number containing assembly number
 *
 * Error Conditions : bad handle or null pointer to assembly number
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqAssembly (BC_PCI_HANDLE hBC_PCI, int16_t* num)
{
    uint8_t rc = 0;
    struct assembly    asy;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(num==0))
    {
	return -1;
    }
    memset ( &asy, 0, sizeof(struct assembly));
    pcmd = (struct btfp_inarea *) &asy;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_ASSEMBLY;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (asy.id == TFP_ASSEMBLY)
	{
	    *num = ntohs(asy.assembly);
	}
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqRevisionID
 *
 * Description : This is the firmware revision id. 
 * This is the 8 bit Revision ID in the PCI configuration space
 * For version 1 PCI card, the Revision ID is 0x12
 * For version 2 PCI card, the Revision ID is 0x20
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : id
 *
 * Error Conditions :
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqRevisionID (BC_PCI_HANDLE hBC_PCI, int8_t* id)
{
    uint8_t rc = 0;
    struct firmware    rev;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(id==0))
    {
	return -1;
    }
    memset ( &rev, 0, sizeof(struct firmware));
    pcmd = (struct btfp_inarea *) &rev;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_REVISION;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (rev.id == TFP_REVISION)
	{
	    memcpy( id, rev.revision, 7*sizeof(int8_t));
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqRevisionID
 *
 * Description : This is the firmware revision id. 
 * This is the 8 bit Revision ID in the PCI configuration space
 * For version 1 PCI card, the Revision ID is 0x12
 * For version 2 PCI card, the Revision ID is 0x20
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : id
 *
 * Error Conditions :
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqFirmRevisionID (BC_PCI_HANDLE hBC_PCI, int8_t* id)
{
    uint8_t rc = 0;
    struct firmware    rev;
    struct btfp_inarea *pcmd;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(id==0))
    {
	return -1;
    }
    memset ( &rev, 0, sizeof(struct firmware));
    pcmd = (struct btfp_inarea *) &rev;
    pcmd->cmd    = TFP_REQUEST_DATA;
    pcmd->subcmd = TFP_FIRM_INFO;

    rc = ioctl( hBC_PCI->fd, TFP_REQUEST_DATA, pcmd);
    if (!rc)
    {
	if (rev.id == TFP_REVISION)
	{
	    memcpy( id, rev.revision, 6*sizeof(int8_t));
	}
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqTimeCodeDataEx
 *
 * Description : Function to request the time code data extended with 
 * V2 hardware support. The V2 hardware supports time code sub type. 
 * Calling the original function bcReqTimeCodeData (with TimeCodeData *) 
 * will not return the sub type info.
 *
 * Inputs : handle to bc635, 
 *
 * Returns : extended timecode data
 * 
 * Error Conditions : 0 on success -1 on failure
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReqTimeCodeDataEx (BC_PCI_HANDLE hBC_PCI, TimeCodeDataEx *pDat)
{
    struct generatorOffset_t offset;
    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(pDat==0))
    {
	return -1;
    }

    /*
     * Time code:
     * int8_t  inputFormat   input decode type 
     * int8_t  inputSubType  input SubType
     * int8_t  modulation    Modulation
     * int8_t  outputFormat  Generator Type
     * int8_t  outputSubType Output SubType
     * int16_t genoffset     Generator Offset
     * int8_t  ghfhr  	     Offset half Hour flag 0/1
     */
    if (bcReqInputFormat (hBC_PCI, &pDat->inputFormat, &pDat->inputSubType))
	return -1;
    if (bcReqInputModulation(hBC_PCI, &pDat->modulation))
	return -1;
    if (bcReqOutputFormat(hBC_PCI,&pDat->outputFormat, &pDat->outputSubType))
       return -1;
    if (bcReqGeneratorOffset(hBC_PCI, &offset))
    {
	return -1;
    }
    else
    {
	pDat->genoffset = offset.LocalOffset;
	pDat->ghfhr     = offset.HalfHour;
    }

    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : bcGPSReq
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : bcGPSReq Not implmented!
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
// Functions to request/send GPS packets (Bc637PCI model only)
uint8_t  bcGPSReq (BC_PCI_HANDLE hBC_PCI, GpsPkt *gps1)
{
    printf("bcGPSReq Not implmented!\n");
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcGPSSnd
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions :  Not implmented!
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcGPSSnd (BC_PCI_HANDLE hBC_PCI, GpsPkt *gps)
{
    printf("bc bcGPSSnd Not implmented!\n");
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
 * Error Conditions :  Not implmented!
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcGPSMan (BC_PCI_HANDLE hBC_PCI, GpsPkt *gps1, GpsPkt *gps2)
{
    printf("bcGPSMan Not implmented!\n");
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
 * Error Conditions :  Not implmented!
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
// Interrupts functions
uint8_t  bcStartInt(BC_PCI_HANDLE hBC_PCI, BC_PCI_INT_HANDLER pCallback)
{
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
 * Error Conditions :  Not implmented! 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void  bcStopInt (BC_PCI_HANDLE hBC_PCI)
{

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
 * Error Conditions :  Not implmented!
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetInt(BC_PCI_HANDLE hBC_PCI, int8_t IntVal)
{
    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcReqInt
 *
 * Description : Return the interrupt mask
 *
 * Inputs : hBC_PCI - handle to card
 *
 * Returns : Ints - the interrupt mask
 *
 * Error Conditions : 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  bcReqInt(BC_PCI_HANDLE hBC_PCI, int8_t* Ints)
{
    uint8_t  rc;
    uint32_t val;

    // Return if we are passed null pointers
    if ((hBC_PCI == 0)||(Ints==0))
    {
	return -1;
    }
    rc = ioctl( hBC_PCI->fd, BTFP_FETCH_REG_MASK, &val);
    if (!rc)
    {
	*Ints = val & 0x000000FF;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcIntHandlerRoutine
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions : not yet implemented
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void  bcIntHandlerRoutine(BC_PCI_HANDLE hBC_PCI, int16_t dwSource)
{
    printf("bcIntHandlerRoutine not yet implemented\n");
}

/**
 ******************************************************************
 *
 * Function Name : bcdelay
 *
 * Description : Wrapper for a delay function in microseconds. 
 *
 * Inputs : number of microseconds to wait.
 *
 * Returns : none
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
void  bcDelay(int16_t ustime)
{
    struct timespec sleeptime;
    sleeptime.tv_sec  = 0;
    sleeptime.tv_nsec = ustime*1000;
    nanosleep( &sleeptime, 0);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetPeriodicDDSSelect
 *
 * Description : Select either (0) periodic or (1) DDS output 
 *
 * Inputs : handle to card, and 0/1 value for type of selection
 *
 * Returns :
 *
 * Error Conditions : -1 failure, card not open
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetPeriodicDDSSelect (BC_PCI_HANDLE hBC_PCI, int8_t bSel)
{
    uint8_t val = 0;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (bSel)
	val = 1;
    return ioctl( hBC_PCI->fd, BTFP_SELECT_DDS, &val);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetPeriodicDDSEnable
 *
 * Description : This command controls the Periodic or DDS signal 
 *               that is output on P1 pin 15. This output may be
 *               either on or off based on the selection.
 *
 * Inputs : handle to card, and enable (1) DDS or disable (0) - meaning
 *          use periodic output
 *
 * Returns : none
 *
 * Error Conditions : -1 on failure.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetPeriodicDDSEnable (BC_PCI_HANDLE hBC_PCI, uint8_t bEnable)
{
    uint8_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (bEnable)
	val = 1;
    else
	val = 0;
    return ioctl( hBC_PCI->fd, BTFP_ENABLE_DDS, &val);

}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSDivider
 *
 * Description : The DDS frequency synthesizer's divider can be used to 
 *               divide the selected input down to generatelower or 
 *               fractional frequencies. Selectable decade divides 
 *               that range from divide by 1E0 throughdivide by 1E7 
 *               are available. The divider will also allow for 
 *               fractional frequency outputs where the divider's
 *               input source is decimal shifted by up to 7 places. 
 *               The frequency synthesizer's divider is the output
 *               signal provided on P1 pin 15.
 *
 * Inputs : handle and divide value. 
 *          0 = divide by 1,
 *          1 = divide by 10
 *          2 = divide by 100
 *          3 = divide by 1000
 *          4 = divide by 10000 
 *          5 = divide by 100000
 *          6 = divide by 1000000
 *          7 = divide by 10000000
 *          F = divide by value in Period Register (when selected, 
 *              refer to command 49)
 *
 * Returns : none
 *
 * Error Conditions : -1 on failure
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSDivider (BC_PCI_HANDLE hBC_PCI, uint8_t bDiv)
{
    if (hBC_PCI == 0)
    {
	return -1;
    }

    return ioctl( hBC_PCI->fd, BTFP_DIVIDE_DDS, &bDiv);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSDividerSource
 *
 * Description : The frequency synthesizer's divide chain has 3 
 *               possible input sources.
 *                   0 = DDS
 *		     1 = Multiplier (DDS x multiplier)
 *                   2 = 100 MHz (100 MHz PLL)
 *
 * Inputs : handle and source value as defined above
 *
 * Returns : -1 on failure
 *
 * Error Conditions : bad handle, bad read, or invalid source number
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSDividerSource (BC_PCI_HANDLE hBC_PCI, uint8_t bSrc)
{
    if ((hBC_PCI == 0) || (bSrc>2))
    {
	return -1;
    }

    return ioctl( hBC_PCI->fd, BTFP_SOURCE_DDS, &bSrc);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSSyncMode
 *
 * Description : The DDS frequency synthesizer's divider has 2 modes of 
 *               operation, Fractional and Continuous. Fractionalmode  
 *               allows for fractional frequencies to be generated that  
 *               are time synchronized only when achange is made to the  
 *               DDS frequency but never again, allowing for non-integer  
 *               rates. Continuousmode should be used for integer rates  
 *               only where the frequency synthesizer and divider are  
 *               synchronizedeach second. Note that integer frequency  
 *               rates may use the fractional mode and it willremain on  
 *               time if the unit does not adjust phase using a jamsync.  
 *               Note that this selection is restoredat power-up from  
 *               NV memory.
 *
 *               0 = Fractional (synchronizes only once -  
 *                   allows fractional rates) 
 *               1 = Continuous (synchronizes every second -  
 *                   integer frequencies)
 *
 * Inputs : handle to card and mode
 *
 * Returns : none
 *
 * Error Conditions : bad handle, invalid mode, or bad read
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSSyncMode (BC_PCI_HANDLE hBC_PCI, int8_t bMode)
{
    uint8_t val;
    if ((hBC_PCI == 0) || (bMode<0) || (bMode>1))
    {
	return -1;
    }
    if (bMode)
	val = 1;
    else
	val = 0;
    return ioctl( hBC_PCI->fd, BTFP_MODE_DDS, &val);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSMultiplier
 *
 * Description : The DDS frequency synthesizer has the ability to 
 *               multiply its output by 1, 2, 3, 4, 6, 8, 10 or 16.  
 *               Notethat the DDS frequency must be high or low enough  
 *               for the multiplier to operate correctly (see InputRange  
 *               in MHz below). Note that this selection is restored  
 *               at power-up from NV memory.
 *
 * Inputs : handle to card, and multiplier. 
 *          bmult - 
 *          Value Input    Range MHz
 *          0x1 = DDS   x1   22 - 150
 *          0x2 = DDS   x2   11 - 75
 *          0x3 = DDS   x3    8 - 56
 *          0x4 = DDS   x4    6 - 38
 *          0x6 = DDS   x6    5 - 23
 *          0x8 = DDS   x8    5 - 19
 *          0xa = DDS   x10   5 - 15
 *          0x10 = DDS  x16   5 - 10
 *
 * Returns : -1 on failure
 *
 * Error Conditions : bad hndle, invalid multipler value, or bad operation
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSMultiplier (BC_PCI_HANDLE hBC_PCI, int8_t bMult)
{
    if ((hBC_PCI == 0) || (bMult<0) || (bMult>10))
    {
	return -1;
    }

    return ioctl( hBC_PCI->fd, BTFP_MULT_DDS, &bMult);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSPeriodValue
 *
 * Description : The DDS divider has a selectable mode called 
 *               Period Mode (command 0x45 value = F). This mode
 *               may be desirable when the DDS cannot be set to the 
 *               exact frequency but a period value, based on
 *               the period of the DDS rate, will be exact. 
 *               Note that this value is restored at power-up from NV memory.
 *
 * Inputs : handle to card, and period value 0x0 - 0x00FFFFFF
 *
 * Returns : none
 *
 * Error Conditions : Bad handle, or bad read
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSPeriodValue (BC_PCI_HANDLE hBC_PCI, uint32_t dwPeriod)
{
    uint32_t val;

    if (hBC_PCI == 0)
    {
	return -1;
    }
    val = 0x00FFFFFF & dwPeriod;
    return ioctl( hBC_PCI->fd, BTFP_PERIOD_DDS, &val);
}
/**
 ******************************************************************
 *
 * Function Name : bcSetDDSTuningWord
 *
 * Description : The DDS frequency is set with this command. 
 *               The desired frequency x32 = DDS Tuning Word.
 *               Frequencies higher than 22 MHz should be attained 
 *               using Multiplier Mode (0x48, 0x46 commands).
 *               Note that this value is restored at power-up from NV memory.
 *
 * Inputs : handle to card and tuning value. 
 *
 * Returns : none
 *
 * Error Conditions : bad handle or bad write
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSTuningWord (BC_PCI_HANDLE hBC_PCI, uint32_t dwTuneWord)
{
    uint32_t val;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    val = 0x3FFFFFFF & dwTuneWord;
    return ioctl( hBC_PCI->fd, BTFP_TUNING_DDS, &val);
}

/**
 ******************************************************************
 *
 * Function Name : bcSetDDSFrequency
 *
 * Description : The bcSetDDSFrequency automatically sets the 
 *               periodic/DDS output to be DDS output and sets the DDS sync 
 *               mode to be DDS_SYNC_MODE_FRAC. If you want to come back to 
 *               periodic output, please call function bcSetPeriodicDDSSelect 
 *               to set it to periodic output. You can also change sync mode 
 *               with bcSetDDSSyncMode.
 *
 *               Frequency x 32 = DDS tuning word value.
 *
 * Inputs : handle to card
 *
 * Returns : -1 on failure. 
 *
 * Error Conditions : bad handle or one of the sets fails. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcSetDDSFrequency (BC_PCI_HANDLE hBC_PCI, double fFreq)
{
    uint32_t tuning = (uint32_t) floor( fFreq *32.0); 

    if(bcSetPeriodicDDSEnable( hBC_PCI, 1)) return -1;
    if(bcSetDDSSyncMode( hBC_PCI, 0)) return -1;
    if(bcSetDDSTuningWord( hBC_PCI, tuning)) return -1;

    return 0;
}
/**
 ******************************************************************
 *
 * Function Name : ReadBinTime
 *
 * Description : Function to read event time including the 
 * 100 nano seconds count. This is for the case when the data is stored in
 * Binary format. 
 *
 * Inputs : hBC_PCI - handle to BC PCI device
 *          reg     - register to read from, we have so many!
 *
 *
 * Returns : ulpMaj - seconds count
 *           ulpMin   microseconds
 *           ulpNano  nanoseconds
 *           bpstat   status
 *
 * Error Conditions : -1 on failure, usually a null pointer...
 * 
 * Unit Tested on:  12-Jan-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
static uint8_t ReadBinTime(BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			     uint32_t* ulpMin, uint16_t* ulpNano, 
			     int8_t* bpstat, uint32_t reg)
{
    struct timereg val;
    uint8_t        rc;
    int8_t         format;

    if ((hBC_PCI == 0)||(ulpMaj==0)||(ulpMin==0)||(bpstat==0)||(ulpNano==0))
    {
	return -1;
    }

    *ulpMaj  = *ulpMin = 0;
    *bpstat  = 0xff;
    *ulpNano = 0;
    bcReqTimeFormat( hBC_PCI, &format);
    if (format == UNIX_TIME)
    {
	rc = ioctl( hBC_PCI->fd, reg, &val);
	if (!rc)
	{
	    if (gVerbose>0)
	    {
		printf(" ReadBinTime, reg: %X LINE: %d, t0: 0x%X t1: 0x%X\n",
		       reg, 
		       __LINE__, val.time0, val.time1);
	    }
	    // Time 0 is broken up as:
	    // 27:24 - status bits
	    // 23:20 - binary 100's of nanoseconds
	    // 19:0  - binary microseconds
 	    *ulpMin  = val.time0 & 0x000FFFFF;
            // All bits represent Unix seconds in time1 register
	    *ulpMaj  = val.time1; 
	    *bpstat  = (val.time0>>24)&0x0F;
	    *ulpNano = (val.time0>>20) & 0x0F;
	    *ulpNano *= 100;
	}
    }
    else
    {
	rc = -1;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : bcReadEventTimeEx
 *
 * Description : Function to read event time including the 100 nano 
 *               seconds count
 *
 * Inputs : 
 *
 * Returns : ulpMaj - number of seconds since UNIX epoch 
 *           ulpMin - milliseconds
 *           ulpNano - 100's of nanoseconds
 *           bpstat  - status of read. 
 *               Bit 1: Tracking (T)
 *                   0: Locked To Selected Reference
 *                   1: Flywheeling (Not Locked)
 *     
 *               Bit 2 Phase (P)
 *                   0: < X Microseconds
 *                  1: > X Microseconds
 *                   X = 5 (Mode 0) 
 *                   X = 2 (All Other Modes)
 *              
 *               Bit 3 Frequency (F)
 *                   0: < 5 x 10-8
 *                   1: > 5 x 10-8
 *             
 *           0 on success -1 on failure
 *
 *
 * Error Conditions : bad read, bad pointer
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadEventTimeEx (BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			    uint32_t* ulpMin, uint16_t* ulpNano, 
			    int8_t* bpstat)
{
    uint8_t rc = ReadBinTime(hBC_PCI, ulpMaj, ulpMin, ulpNano, bpstat,
			     BTFP_READ_EVENTREG);
    
    return rc;
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
uint8_t  bcReadEvent2TimeEx (BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			     uint32_t* ulpMin, uint16_t* ulpNano, 
			     int8_t* bpstat)
{
    return ReadBinTime(hBC_PCI, ulpMaj, ulpMin, ulpNano, bpstat, 
		      BTFP_READ_EVENT2);
}
uint8_t  bcReadEvent3TimeEx (BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			     uint32_t* ulpMin, uint16_t* ulpNano,
			     int8_t* bpstat)
{
    return ReadBinTime(hBC_PCI, ulpMaj, ulpMin, ulpNano, bpstat, 
		      BTFP_READ_EVENT3);
}
uint8_t  bcReqOtherDataEx(BC_PCI_HANDLE hBC_PCI, OtherDataEx *data)
{
    /*
     * int8_t hbtmode	  Heartbeat/Periodic mode (0=async,1=sync)
     * uint16_t hbtcnt1   Heartbeat/Periodic n1
     * uint16_t hbtcnt2   Heartbeat/Periodic n2
     * int8_t freq        Current freq output (1,5,10MHz)
     * int8_t evtctl      Enable/Disable events
     * int8_t evtsense    Event trigger on edge (1=falling,0=rising)
     * int8_t evtlock	  Enable/Disable event capture lockout
     * int8_t evtsrc      Event trigger source (0=evt,1=hbt)
     * int8_t evt2ctl     Enable/Disable event2
     * int8_t evt2sense   Event2 trigger on edge (1=falling,0=rising)
     * int8_t evt2lock	  Enable/Disable event2 capture lockout
     * int8_t evt3ctl     Enable/Disable event3
     * int8_t evt3sense   Event3 trigger on edge (1=falling,0=rising)
     * int8_t evt3lock    Enable/Disable event3 capture lockout
     */

    return -1;
}
uint8_t  bcReqEventsData (BC_PCI_HANDLE hBC_PCI, EventsData *data)
{
    /*
     * int8_t evtsrc      Event trigger source (0=evt,1=hbt)
     * int8_t evtctl	  Enable/Disable events
     * int8_t evtsense    Event trigger on edge (1=falling,0=rising)
     * int8_t evtlock	  Enable/Disable event capture lockout
     * int8_t evt2ctl	  Enable/Disable event2
     * int8_t evt2sense   Event2 trigger on edge (1=falling,0=rising)
     * int8_t evt2lock    Enable/Disable event2 capture lockout
     * int8_t evt3ctl     Enable/Disable event3
     * int8_t evt3sense   Event3 trigger on edge (1=falling,0=rising)
     * int8_t evt3lock    Enable/Disable event3 capture lockout
     */

    return -1;
}
uint8_t  bcSetEventsData (BC_PCI_HANDLE hBC_PCI, EventsData *data)
{
    return -1;
}

// This function enables the interrupts specified by the input mask value.
// The function bcStartInt() uses 'IntMask' 0x7F internally. We found that
// with IntMask 0x7F, interrupts were not handled correctly by the driver
// on newer, faster computers. You must pass the mask value corresponding
// to your desired interrupts to have the driver working properly.
uint8_t  bcStartIntEx(BC_PCI_HANDLE hBC_PCI, BC_PCI_INT_HANDLER pCallback, 
		      int16_t IntMask)
{
    return -1;
}

// Returns the 100 nano seconds count field
/**
 ******************************************************************
 *
 * Function Name : bcReadBinTimeEx 
 *
 * Description : Latch and read the time registers in a standard unix format
 *
 * Inputs : handle to card
 *
 * Returns : ulpMaj - number of seconds since UNIX epoch 
 *           ulpMin - milliseconds
 *           ulpNano - 100's of nanoseconds
 *           bpstat  - status of read. 
 *               Bit 1: Tracking (T)
 *                   0: Locked To Selected Reference
 *                   1: Flywheeling (Not Locked)
 *     
 *               Bit 2 Phase (P)
 *                   0: < X Microseconds
 *                  1: > X Microseconds
 *                   X = 5 (Mode 0) 
 *                   X = 2 (All Other Modes)
 *              
 *               Bit 3 Frequency (F)
 *                   0: < 5 x 10-8
 *                   1: > 5 x 10-8
 *             
 *           0 on success -1 on failure
 *
 * Error Conditions : -1 on failure, usually null pointer or bad read. 
 * 
 * Unit Tested on: 24-Mar-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadBinTimeEx (BC_PCI_HANDLE hBC_PCI, uint32_t* ulpMaj, 
			  uint32_t* ulpMin, uint16_t* ulpNano, int8_t* bpstat)
{
    if ((hBC_PCI == 0) || (ulpMaj==0) || (ulpMin==0) || (bpstat==0) || 
	(ulpNano==0))
    {
	return -1;
    }
    if(!ioctl( hBC_PCI->fd, BTFP_LATCH_TIMEREG, 0))
    {
	if(!ReadBinTime(hBC_PCI, ulpMaj, ulpMin, ulpNano, bpstat, 
			BTFP_READ_TIME))
	{
	    return 0;
	}
    }

    return -1;
}
/**
 ******************************************************************
 *
 * Function Name : bcReadDecTimeEx
 *
 * Description : Read out time data in DECIMAL/BCD format. 
 *
 * Inputs : handle to card
 *
 * Returns : struct tm partially populated
 *           ulpMin - milliseconds
 *           ulpNano - 100's of nanoseconds
 *           pbstat  - status of call
 *
 * Error Conditions : -1 on failure, eg: null pointer or bad read. 
 * 
 * Unit Tested on: 24-Mar-13
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadDecTimeEx (BC_PCI_HANDLE hBC_PCI, struct tm *ptm, 
			  uint32_t* ulpMin, uint16_t* ulpNano, int8_t* bpstat)
{
    struct timereg val;
    uint16_t       year;

    if ((hBC_PCI == 0) || (ulpMin==0) || (bpstat==0) || (ptm==0))
    {
	return -1;
    }
    *ulpMin = 0;
    *bpstat = 0xff;
    *ulpNano = 0;
    if(bcSetTimeFormat (hBC_PCI, BCD_TIME))
	return -1;

    if(ioctl( hBC_PCI->fd, BTFP_LATCH_TIMEREG, 0))
	return -1;

    if(ioctl( hBC_PCI->fd, BTFP_READ_TIME, &val))
	return -1;

    ptm->tm_sec  = val.time1&0x1F;
    ptm->tm_min  = (val.time1>>8)&0x1F;
    ptm->tm_hour = (val.time1>>16)&0x1F;
    ptm->tm_yday = (val.time1>>24)&0xFF;
    ptm->tm_mday = 0;
    ptm->tm_mon  = 0;
    if (!bcReqYear (hBC_PCI, &year))
	ptm->tm_year = year-1900;
    else
	ptm->tm_year = 1900;

    ptm->tm_wday = 0;
    ptm->tm_yday = (val.time1>>24)&0xFF;
    ptm->tm_isdst = 0;
    *ulpMin  = val.time0&0x00000FFF;
    *bpstat  = (val.time0>>24)&0x0F;
    *ulpNano = (val.time0>>20) & 0x0F;

    return 0;

}
/**
 ******************************************************************
 *
 * Function Name : GetOutputFrequency
 *
 * Description : The output reference frequency is found at the DB15 
 * on the back of the card and is settable, 1,5 or 10 MHz. Control is 
 * done through a two bit field in the control register of the BC635. 
 *
 * Inputs : structure containing a valid BC handle and the value, 0-2 to 
 * set the frequency to. This is bits 6&7 of the CR. 
 * 6    7    Result
 * 0    0    10 MHz
 * 0    1      5 MHz
 * 1    0      1 MHz
 * 1    1      1 MHz
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 25-Jan-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  GetOutputFrequency( BC_PCI_STRUCT* hBC_PCI, uint8_t *val)
{
    uint8_t  rc;
    uint16_t data;
    if ((hBC_PCI == 0) || (val==0))
    {
	return -1;
    }

    rc = bcReadControl( hBC_PCI, &data);

    if (!rc)
    {
	*val = (data>>6)&0x03;
    }
    else
    {
	*val = 0;
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SetOutputFrequency
 *
 * Description : Set the frequency of the 1, 5 or 10MHz output.
 *
 * Inputs : index to use for set. 
 *  0 - 10 MHz
 *  1 -  5 MHz
 *  2 -  1 MHz
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 25-Jan-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  SetOutputFrequency( BC_PCI_STRUCT* hBC_PCI, uint8_t val)
{
    /*      10MHz, 5MHz, 1MHz settings */
    static uint32_t FBits[4] = {0x0000, 0x0040, 0x0080, 0x00C0};
    uint8_t  rc;
    uint32_t control;

    if (hBC_PCI == 0)
    {
	return -1;
    }
    if (val<0) val = 0;
    if (val>3) val = 3;

    rc = bcReadReg( hBC_PCI, TFP_REG_CONTROL, &control);
    if (!rc)
    {
	// clear bits 6 and 7 of the control register. 
	control &= 0xFFFFFF3F;
	control |= FBits[val];
	rc = bcWriteReg( hBC_PCI, TFP_REG_CONTROL, control);
    }
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : bcReqLocalTZObs
 *
 * Description : This command queries the daylight saving and local 
 *               time observed flag. Additionally, this commandis used 
 *               to set the IEEE 1344 Time Code daylight saving observed 
 *               flag. Use this command in conjunctionwith Command 0x19.
 *
 * Inputs : hBC_PCI handle of card
 *
 * Returns : val: 
 * bit     value    meaning
 *  0               Reserved
 *  1               Reserved
 *  2               Reserved
 *  3       0x08    Local time observe flag
 *  4       0x10    IEEE 1344 DST observe flag
 *  5               Not Used
 *
 * Error Conditions : Bad handle, null pointer, 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  GetIEEE1344Flag( BC_PCI_STRUCT* hBC_PCI, uint8_t *val)
{
    struct  btfp_inarea cmd;
    uint8_t rc;
    if (hBC_PCI == 0)
    {
	return -1;
    }
    cmd.cmd    = TFP_REQUEST_DATA;
    cmd.subcmd = TFP_IEEE1344;
    rc = ioctl( hBC_PCI->fd, BTFP_REQUEST_DATA, &cmd);
    if (!rc)
    {
	if(cmd.cmd == TFP_IEEE1344)
	{
	    *val = cmd.subcmd;
	}
    }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : This command queries the daylight saving and local 
 *               time observed flag. Additionally, this commandis used 
 *               to set the IEEE 1344 Time Code daylight saving observed 
 *               flag. Use this command in conjunctionwith Command 0x19.
 *
 * Inputs : hBC_PCI handle of card
 *          val:
 * bit     value    meaning
 *  0               Reserved
 *  1               Reserved
 *  2               Reserved
 *  3       0x08    Local time observe flag
 *  4       0x10    IEEE 1344 DST observe flag
 *  5               Not Used
 *
 * Returns : none 
 *
 * Error Conditions : Bad handle, null pointer, 
 * 
 * Unit Tested on: 23-Feb-13
 *
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
uint8_t  SetIEEE1344Flag( BC_PCI_STRUCT* hBC_PCI, uint8_t val)
{
    if (hBC_PCI == 0)
    {
	return 0xFF;
    }
    return ioctl( hBC_PCI->fd, BTFP_IEEE1344, val);
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

uint8_t  bcSoftReset(BC_PCI_HANDLE hBC_PCI)
{
    if (hBC_PCI == 0)
    {
	return 0xFF;
    }
    return ioctl( hBC_PCI->fd, BTFP_SOFTWARE_RESET, 0);
}
/**
 ******************************************************************
 *
 * Function Name : bcReadControl
 *
 * Description : 
 *    Read the low 15 bits of the control register. 
 *    TFP_FETCH_REG_CONTROL 0x84 get contents of control register
 *    Details on page 28 of user manual.
 *    
 *
 * Inputs : handle to open BC635 device and data field to hold data
 *
 * Returns : 0 on success
 *
 * Error Conditions : bad data fed to ioctl call
 * 
 * Unit Tested on: 25-Jan-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint8_t  bcReadControl(BC_PCI_HANDLE hBC_PCI, uint16_t *val)
{
    uint32_t data;
    uint8_t   rc;
    if ((hBC_PCI == 0) || (val==0))
    {
	return 0xFF;
    }
    rc = ioctl( hBC_PCI->fd, BTFP_FETCH_REG_CONTROL, &data);
    *val = data;
    return rc;
}
