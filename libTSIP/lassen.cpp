/**
 ******************************************************************
 *
 * Module Name : lassen.cpp
 *
 * Author/Date : C.B. Lirakis / 02-Jun-02
 *
 * Description : Interface to Trimble Lassen SK8 GPS Receiver. 
 *
 * TSIP packet structure is the same for both commands and reports. 
 * The packet format is:
 * <DLE> <id> <data string bytes> <DLE> <ETX>
 * Where:
 *     <DLE> is the byte 0x10
 *     <ETX> is the byte 0x03
 *     <id> is a packet identifier byte, which can have any value 
 *     excepting <ETX> and <DLE>.
 * The bytes in the data string can have any value. To prevent confusion 
 * with the frame sequences <DLE> <id> and <DLE> <ETX>, every <DLE> byte 
 * in the data string is preceded by an extra <DLE> byte ('stuffing'). 
 * These extra <DLE> bytes must be added ('stuffed')
 * before sending a packet and removed after receiving the packet. 
 * Notice that a simple <DLE> <ETX> sequence does not necessarily signify 
 * the end of the packet, as these can be bytes in the middle of a data 
 * string. The end of a packet is <ETX> preceded by an odd
 * number of <DLE> bytes.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 03-Mar-24 CBL Changed buffered and removed hex dump in favor of << 
 *               operator overload
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
#include <fstream>
#include <sys/time.h>
#include <time.h>
#ifdef __APPLE__
#  include <sys/time.h>
#endif

/// Local Includes.
#include "lassen.hh"
#include "debug.h"
#include "CLogger.hh"
#include "TSIPUtility.hh"

/**
 ******************************************************************
 *
 * Function Name : Lassen Constructor
 *
 * Description : Note  this  constructor is  setup for  use on
 * linux. Note also that we have set  the third argument in the
 * SerialIO constructor to LassenBufferSize. We  have also asked 
 * for non-canonical mode. What this will  do is read
 * either LassenBufferSize if timeout  does  not occur
 * or something less if timeout does  occur. Timeout is set
 * static in SerialIO constructor.
 *
 * Inputs : none
 *
 * Returns : fully constructed lassen class.
 *
 * Error Conditions :
 *
 *******************************************************************
 */
Lassen::Lassen (void)
{
    SET_DEBUG_STACK;
    fVerbosity = 0;

    fNavigationProcessor  = new Revision_Info(NULL);
    fSignalProcessor      = new Revision_Info(NULL);
    fDataOutOfBounds      = false;
    fMode = fRCTM_Version = fReferenceStationID = 0; 
    fPRNCount = fPRNCountB = 0;

    SET_DEBUG_STACK;
    fSStatus      = new SolutionStatus();
    fLLPosition   = new TSIPosition();
    fENUVelocity  = new TSIPVelocity();
    fSLevel       = new SignalLevel[MAXPRNCOUNT];
    fpRawTracking = new RawTracking[MAXPRNCOUNT];
    fGPStime      = new GPSTime();

    ClearRawTracking();

    SET_DEBUG_STACK;
    fError = NO_DECODE_ERROR;
}

/**
 ******************************************************************
 *
 * Function Name : Lassen destructor
 *
 * Description : Clean up all allocated data structures. 
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Lassen::~Lassen(void)
{
    delete fSStatus;
    delete fLLPosition;
    delete[] fSLevel;
    delete[] fpRawTracking;
    delete fNavigationProcessor;
    delete fSignalProcessor;
}

/**
 ******************************************************************
 *
 * Function Name : DecodeMessage
 *
 * Description : Given a line of data retrived from the serial port
 * decide which message is currently at the head of this data stream
 * and decode it properly. 
 *
 * Inputs : Line - buffer of data collected from the serial port.
 *          NBytes - Number of bytes in the line. 
 *
 * Returns : number of bytes left over after decoding. 
 *
 * Error Conditions : A text message is displayed to standard out if
 * it is not handled in the case statement. 
 *
 *******************************************************************
 */
int Lassen::DecodeMessage (Buffered *buf)
{
    SET_DEBUG_STACK;
    char*          MessageDump  = (char *) "UNKNOWN.";
    unsigned char  p;
    int            delta;
    CLogger*       pLogger = CLogger::GetThis();

    /*
     * Number of expected bytes by message type. 
     * This number is based on 0x10 preamble
     * Mesasge ID and 0x10 0x03 ETX.
     */
    int           ExpectedBytes; 
    unsigned char MessageID;

    fError = NO_DECODE_ERROR;
    fBuffer = buf;
    buf->SetBusy();

    /* Remove any prefix of DLE */
    do {
	p = fBuffer->GetChar();
    } while ((p == DLE) && (fBuffer->Remaining()>2));

    MessageID     = p;
    // Perhaps the buffer is queer.Usually at startup.
    if ((fBuffer->Remaining() < 2) || (MessageID == 0))
    {
	fError = BUFFER_TOO_SMALL;
	fBuffer->Reset();
	SET_DEBUG_STACK;
	return 0;
    }

    ExpectedBytes = 0;

    //printf ("Message %2X\n", (int) MessageID);
    switch (MessageID)
    {
    case ECEF_XYZ_REPLY:
        // Not currently decoded. 8-Aug-03
	MessageDump = (char *)"ECEF_XYZ_REPLY not decoded";
	ExpectedBytes = 16;
        break;
    case ECEF_XYZ_DOUBLE_REPLY:
        // Not currently decoded. 8-Aug-03
	MessageDump = (char *)"ECEF_XYZ_DOUBLE_REPLY, not decoded";
	ExpectedBytes = 36;
        break;
    case ECEF_VELOCITY_REPLY:
        // Not currently decoded. 8-Aug-03
	MessageDump = (char *)"ECEF_VELOCITY_REPLY, not decoded";
	ExpectedBytes = 20;
        break;
    case ALMANAC_FOR_SINGLE:
	MessageDump = (char *)"Almanac for single satellite 0x40, not decoded";
	ExpectedBytes = 36;
        break;
    case GPS_TIME_OF_WEEK_REPLY:
	// Get PC time when this is detected. 
        // This is a power up packet, if time is available. 
	MessageDump = (char *) "PUP: GPS time  of week.  0X41";
        ExpectedBytes = Decode_GPS_Time ();
        break;
    case SOFTWARE_REVISION_REPLY:
	MessageDump = (char *) "Software revision. 0x45";
        ExpectedBytes = Decode_SoftwareRevision();
        break;
    case HEALTH_REPLY:
	MessageDump = (char *) "Health. 0x4B";
        ExpectedBytes = Decode_RxHealth ();
        break;
    case MODE_REPLY:
	MessageDump = (char *) "Mode. 0x82";
        ExpectedBytes = Decode_RxMode ();
        break;
    case SATELLITE_TRACKING_REPLY:
	MessageDump = (char *) "Satellite Tracking data. 0x46";
        ExpectedBytes = DecodeSatelliteTracking();
        break;
    case FIX_DATA_REPLY:
	/* ********************************************************
	 * This seems to mark the beginning of a new frame of data.
	 * *********************************************************/
	MessageDump = (char *) "Fix data. 0x6D";
        ExpectedBytes = DecodeFixData();
        break;
    case LLA_SINGLE_PRECISION_REPLY:
	MessageDump = (char *) "LLA Single precision. 0x4A";
        ExpectedBytes = Decode_PositionSingle ();
        break;
    case VELOCITY_ENU_REPLY:
	MessageDump = (char *) "Velocity ENU. 0x56";
        ExpectedBytes = Decode_VelocityENU ();
        break;
    case SIGNAL_LEVEL_REPLY:
	MessageDump = (char *) "Signal level. 0x47";
        ExpectedBytes = Decode_SignalLevels ();
        break;
    case LLA_DOUBLE_REPLY:
	MessageDump = (char *) "LLA Double precision. 0x84";
        ExpectedBytes = Decode_PositionDouble ();
        break;
    case GPS_DATA_REPLY:
	MessageDump = (char *) "GPS_DATA. 0x58";
        ExpectedBytes = Decode_SatelliteData ();
        break;
    case SATELLITE_DISABLE_REPLY:
	MessageDump  = (char *) "Satellite disable. 0x59";
        Decode_SatelliteDisable ();
        break;
    case RAW_DATA_REPLY:
	MessageDump = (char *) "RawData. 0x5A";
        ExpectedBytes = Decode_SatelliteRaw ();
        break;
    case RAW_TRACKING_REPLY:
	MessageDump = (char *) "Raw tracking. 0x5C";
        ExpectedBytes = Decode_SatelliteRawTracking ();
        break;
    case SEND_DIFFERENTIAL_CORRECTIONS_TYPE1:
        // Not yet decoded but RCTM104 data types. 
	MessageDump = (char *) "Differential Corrections. 0x60";
        break;
    case SYNCHRONIZED_MEASUREMENTS_REPLY:
	MessageDump =  (char *) "Synchronized measurement. 0x6E";
        ExpectedBytes = Decode_SynchronizedMeasurementsReply ();
        break;
    case SYNCHRONIZED_DATA_REPLY:
	MessageDump = (char *) "Synchronized data packet. 0x6F";
        ExpectedBytes = Decode_SyncronizedPacket ();
        break;
    case ONE_SATELLITE_BIAS:
	MessageDump = (char *) "One Satellite Bias and Bias rate.";
	// Not decoded.
	break;
    case 0x4C:
	if (pLogger)
	{
	    pLogger->LogTime(" 0x4C -- LastID: 0x%X\n", fLastMessageID);
//	    fBuffer->HexDump(pLogger->LogPtr()); 
	    *pLogger->LogPtr() << fBuffer;
	}
	break;
    case ETX:
        // Bad Alignment, just move ahead
	fBuffer->Reset();
	SET_DEBUG_STACK;
	if (pLogger)
	{
	    pLogger->LogTime("Extra ETX LastID: 0x%X\n", fLastMessageID);
	}
	fLastMessageID = ETX;
        return 1;
        break;
    case 0x13:
        // This is a strange case
	SET_DEBUG_STACK;
	if (pLogger)
	{
	    pLogger->LogTime("Error interpreting last request.\n");
	}
        break;
    case SUPER_PACKET8F:
	MessageDump = (char *) "Super Packet reply";
        ExpectedBytes = Decode_SuperPacket2 ();
        break;
    case IO_OPTION_REPLY:
	MessageDump = (char *) "IO Option Reply";
        ExpectedBytes = Decode_IO_Options ();
        break;
    case SET_POSITION_LLA:
	if (pLogger)
	{
	    pLogger->LogTime("Set position reply??\n");
	}
	break;
    case SATELLITE_SELECTION_REPORT:
	if (pLogger)
	{
	    pLogger->LogTime("Satellite Selection report response packet Not Supported.\n");
	}
	break;
    default:
	if (pLogger)
	{
	    pLogger->LogTime(" Unknown message: 0x%X LastID: 0x%X\n",
			     MessageID, fLastMessageID);
	    //fBuffer->HexDump(pLogger->LogPtr()); 
	    *pLogger->LogPtr() << fBuffer;
	}
	fBuffer->Reset();
	SET_DEBUG_STACK;
        fLastMessageID= MessageID;
	SET_DEBUG_STACK;
        return 0;    // Increment by 1 and see if it fixes the problem. 
    }

    if(pLogger)
    {
	if (fVerbosity > 0)
	{
	    pLogger->LogTime("%s %s %d\n", MessageDump, __FILE__, __LINE__);
	}
    }
    SET_DEBUG_STACK;
    /* Skip over the trailing ETX and DLE */
    fBuffer->Skip(2);
    delta = fBuffer->Remaining();

    SET_DEBUG_STACK;
    switch (delta)
    {
    case 0:
	break;
    case 1:
	if (delta != 0)
	{
	    p = fBuffer->GetChar();
	    if (p != 0x03)
	    {
		if (pLogger) 
		{
		    pLogger->Log("# %s %d Unexpected single overage. 0x%X\n", 
				 __FILE__, __LINE__, p);
		}
	    }
	}
	break;
    default:
	if (pLogger)
	{
	    pLogger->Log("# ***************************************\n");
	    pLogger->LogTime(
		"DIFFERENCE: %d ID: 0x%X LastID: 0x%X Expected %d Got %d\n",
		delta, MessageID, fLastMessageID, ExpectedBytes, 
		fBuffer->GetFill());
	    //fBuffer->HexDump(pLogger->LogPtr()); 
	    *pLogger->LogPtr() << fBuffer;
	}
    }

    SET_DEBUG_STACK;
    fLastMessageID = MessageID;
    buf->Reset();
    SET_DEBUG_STACK;
    return delta;
}

/**
 ******************************************************************
 *
 * Function Name : DecodeSatelliteTracking
 *
 * Description : 
 * Report Packet 0x46
 * This packet provides information about the satellite tracking status 
 * and the operational health of the Receiver. The receiver sends this 
 * packet after power-on or software-initiated resets, in response to 
 * Packet 0x26 and, during an update cycle. Packet 0x4B is always sent
 * with this packet.
 * Byte Item Type Value Meaning
 *    0 Status Code BYTE 0 h Doing position fixes
 *    01 h Don't have GPS time yet
 *    02 h need initialization
 *    03 h PDOP is too high
 *    08 h No usable satellites
 *    09 h Only 1 usable satellite
 *    0A h Only 2 usable satellites
 *    0B h Only 3 usable satellites
 *    0C h The chosen satellite is unusable
 *    1 Status codes BYTE 0-7 b See Table A-31
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::DecodeSatelliteTracking (void)
{
    SET_DEBUG_STACK;
    //const int ExpectedBytes = (2 + 4);
    // 16-Oct-15 we are getting 10 bytes, don't know why. 
    const int ExpectedBytes = (6 + 4);
    CLogger *pLogger = CLogger::GetThis();

    fError = NO_DECODE_ERROR;
    fRxStatus         = fBuffer->GetChar();
    unsigned char val = fBuffer->GetChar();

    if(fVerbosity > 0)
    {
	if (val & 0x01)
	{
	    if (pLogger)
	    {
		pLogger->LogTime("No battery backup present.\n");
	    }
	}
	if (val & 0x10)
	{
	    if (pLogger)
	    {
		pLogger->LogTime("Antenna Problem!\n");
	    }
	}
    }
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : DecodeFixData (0x6D)
 *
 * Description :   Decode the solution parameters!
 *                 Tested OK 28-Oct-04
 *
 * Inputs : pointer to current location in buffer where
 * this message has been found. The size of this message depends
 * on the number of satellites participating in the solution. 
 * This can be found in the solution status structure.
 *
 * The following manuals differ in how they store the data.
 * References : Lassen-SK8 Embedded GPS Module
 *              System Designer Reference Manual
 *              Part Number: 34149-01
 *              Firmware: 7.20-7.52
 *              Date: August 1997
 *
 *              TSIP Reference
 *              Part Number: 34462-00
 *              Revision: C
 *              Date: April 1999
 *
 * 
 * Checked: 29-Jun-08
 * 18-Oct-15 - This does not work well for multiple PRN. Better decode. 
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::DecodeFixData ()
{
    SET_DEBUG_STACK;
    const int Expected = (17+4);   // add in number of satellites in solution.
    unsigned char FixMode;
    int           rv   = Expected;
    int           NSV, PRN; 
    fError = NO_DECODE_ERROR;

    /*
     * TSIP Reference Manual, page 247. 
     * Table 3-59 
     * Byte 0, Fix Mode
     * Bit #   Meaning
     * -----   -------
     * 0-2     2D or 3D Mode:
     *             0: Unknown
     *             1: 0D
     *             2: 2D Clock Hold
     *             3: 2D
     *             4: 3D
     *             5: Overdetermined Clock
     *             6: DGPS Reference Station
     * 3        Auto or Manual Mode:
     *             0: Auto 
     *             1: Manual
     * 4-7      Number of satellites
     *
     */
    fSStatus->Stamp();  /* Put the time stamp on the Status Solution data */
    FixMode = fBuffer->GetChar(); rv++; /* Byte 0 of the message */
    fSStatus->Solution(FixMode);
    NSV    = (FixMode >> 4) & 0x0F;
    fSStatus->NSV(NSV);
    fSStatus->PDOP(fBuffer->GetSingle());
    fSStatus->HDOP(fBuffer->GetSingle());
    fSStatus->VDOP(fBuffer->GetSingle());
    fSStatus->TDOP(fBuffer->GetSingle());
    SET_DEBUG_STACK;

    // NSV should be limited between
    // 0 and MAXPRNCOUNT;
    // We only fill based on the number of satellites participating.
    for (int i = 0; i < NSV; i++)
    {
	PRN = fBuffer->GetChar(); // Always decode
	// Only store if there is space. 
	if (NSV<MAXPRNCOUNT) 
	    fSStatus->PRN(PRN,i);
	rv++;  // Bump expected based on number decoded. 
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_GPS_Time
 *
 * Description : Decodes message 0x41. 
 *
 * Inputs : address of pointer to current data. This is
 * incremented during decoding, tested OK 15-Jun-02
 * 25-Oct-15 deleted the call to remove 1010. 
 * 
 *
 * Returns : Number of actual bytes expected. 
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_GPS_Time (void)
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (10 + 4);
    CLogger* pLogger = CLogger::GetThis();
    fError = NO_DECODE_ERROR;
    fGPStime->Stamp();
    fGPStime->SetPCTime();
    fGPStime->GPSTimeOfWeek(fBuffer->GetSingle());
    fGPStime->ExtendedGPSWeek(fBuffer->GetInt());     // 0-1023
    fGPStime->UTC_Delta(fBuffer->GetSingle());
    fGPStime->SetDelta();

    /*
     * Bounds check.
     * Note, there appears to be a firmware bug. 
     * at 22-Aug-1999 the week number should have been modulo 1023
     * but apparently is not. This is affirmed by the TSIP documentation. 
     */
    if (fGPStime->CheckBounds())    
    {
	if (pLogger)
	{
	    pLogger->LogTime("Decode_GPS_Time, GPS Time of Week: %f Extended week number:%d GPS/UTC offset: %f\n", 
			     fGPStime->GPSTimeOfWeek(), 
			     fGPStime->ExtendedGPSWeek(),
			     fGPStime->UTC_Delta());
	    //fBuffer->HexDump(pLogger->LogPtr());
	    *pLogger->LogPtr() << fBuffer;
	}
    }
    SET_DEBUG_STACK;
    return ExpectedBytes;
}
/**
 ******************************************************************
 *
 * Function Name : RevisionInfo Constructor
 *
 * Description : Decode and store the various revision  ids and
 * dates for the device. 
 *
 * Inputs : address of pointer to data
 *
 * Returns : Number of actual bytes expected. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
Revision_Info::Revision_Info(Buffered *buf)
{
    SET_DEBUG_STACK;

    /**
     *    0 Major version number BYTE
     *    1 Minor version number BYTE
     *    2 Month BYTE
     *    3 Day BYTE
     *    4 Year number minus 1900 BYTE
     *
     * The first 5 bytes refer to the Navigation Processor and 
     * the second 5 bytes refer to the Signal Processor.
     */
    Major_ID = 0;
    Minor_ID = 0;
    Month    = 0;
    Day      = 0;
    Year     = 0;
    if (buf != NULL)
    {
	Fill(buf);
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : RevisionInfo FILL
 *
 * Description : Decode and store the various revision  ids and
 * dates for the device. 
 *
 * Inputs : address of pointer to data
 *
 * Returns : Number of actual bytes expected. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
void Revision_Info::Fill(Buffered *buf)
{
    SET_DEBUG_STACK;

    /**
     *    0 Major version number BYTE
     *    1 Minor version number BYTE
     *    2 Month BYTE
     *    3 Day BYTE
     *    4 Year number minus 1900 BYTE
     *
     * The first 5 bytes refer to the Navigation Processor and 
     * the second 5 bytes refer to the Signal Processor.
     */
    Major_ID = buf->GetChar();
    Minor_ID = buf->GetChar();
    Month    = buf->GetChar();
    Day      = buf->GetChar();
    Year     = buf->GetChar();
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : RevisionInfo::Print
 *
 * Description : Dump the revision information. 28-Oct-04 CBL
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
void Revision_Info::Print(const char *title) const
{
    SET_DEBUG_STACK;
    cout << title  
	 << " Software revision: "
	 << " Major ID: " << (int) Major_ID 
	 << " Minor ID: " << (int) Minor_ID 
	 << " Month "     << (int) Month
	 << " Day "       << (int) Day 
	 << " Year "      << (int) Year
	 << endl;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : RevisionInfo::FormatData
 *
 * Description : Dump the revision information. 28-Oct-04 CBL
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
void Revision_Info::FormatData(char *buffer, size_t buf_size) const
{
    if (buffer!=NULL && buf_size>80)
    {
	sprintf(buffer, "%2d.%2d Month: %2d Day: %2d Year: %2d",
		Major_ID, Minor_ID, Month, Day, Year);
    }
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SoftwareRevision (0x45)
 *
 * Description : Decode and store the various revision  ids and
 * dates for the device. 28-Oct-04 CBL
 *
 * Inputs : ptr - pointer to slot in data stream where the 
 *          Software revision response code was found. 
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_SoftwareRevision ()
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (10 + 4);
    fError = NO_DECODE_ERROR;

    /**
     *    0 Major version number BYTE
     *    1 Minor version number BYTE
     *    2 Month BYTE
     *    3 Day BYTE
     *    4 Year number minus 1900 BYTE
     *    5 Major revision number BYTE
     *    6 Minor revision number BYTE
     *    7 Month BYTE
     *    8 Day BYTE
     *    9 Year number minus 1900 BYTE
     *
     * The first 5 bytes refer to the Navigation Processor and 
     * the second 5 bytes refer to the Signal Processor.
     */
    fNavigationProcessor->Fill(fBuffer);
    fSignalProcessor->Fill(fBuffer);

    if (fVerbosity > 2)
    {
	fNavigationProcessor->Print("Navigation Processor");
	fSignalProcessor->Print("Signal Processor");
    }
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_RxHealth (0x4B);
 *
 * Description :  Decode the receiver elements. Tested OK 15-Jun-02
 * but not as yet stored in the class data set. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_RxHealth ()
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (3 + 4);
    CLogger* pLogger = CLogger::GetThis();

    fError     = NO_DECODE_ERROR;
    fMachineID = fBuffer->GetChar();
    fStatus[0] = fBuffer->GetChar();

    if (fStatus[0] & (0x08 > 0))
    {
	if (pLogger)
	{
	    pLogger->LogTime("Almanac current.\n");
	}
    }

    // Bit 0 set, then Super packets are supported.
    fStatus[1] = fBuffer->GetChar();     
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_RxMode (0x82) 
 *
 * Description :
 * Valid modes are:
 *    Mode 0 Manual GPS (Differential off) - The receiver does position 
 *           solutions without differential corrections, even if the 
 *           differential corrections are available.
 *
 *    Mode 1 Manual GPD (Differential on) - The receiver only does position
 *           solutions if  valid differential correction data are available.
 *
 *    Mode 2 Auto GPS (Differential currently off) - The receiver is not 
 *           receiving differential correction data for all satellites in
 *           constellation which meets all other masks, and is doing 
 *           non-differential position solutions.
 *
 *    Mode 3 Auto GPD (Differential currently on) - The receiver is 
 *           receiving differential correction data for all satellites in 
 *           a constellation which meets all other masks, and is doing 
 *           differential position solutions.
 *
 * Tested OK 15-Jun-02
 *
 * Inputs : Address of pointer to data, this is incremented as decoded. 
 *          
 * Returns : number of exepcted bytes. This is dependent on 
 *           the mode the system is in. Additional information is 
 *           provided when the GPS is actually receiving Differential
 *           correctors.
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_RxMode ()
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (1 + 4);

    fError = NO_DECODE_ERROR;
    fMode = (int) fBuffer->GetChar(); 

    switch(fMode)
    {
    case MANUAL_DGPS: 
    case   AUTO_DGPS:
	fRCTM_Version       = (int) fBuffer->GetChar();
	fReferenceStationID = fBuffer->GetInt();
	SET_DEBUG_STACK;
	return 8;
	break;
    default:
	// Do nothing.
	SET_DEBUG_STACK;
	break;
    }
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_PositionSingle 
 *
 * Description : Single precision position data. 0x4A
 *
 * Inputs : pointer to data stream.
 *
 * Returns : Number of expected bytes.
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_PositionSingle (void)
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (20 + 4);
    fError = NO_DECODE_ERROR;

    fLLPosition->Stamp();
    fLLPosition->Valid(true);
    fLLPosition->Latitude(fBuffer->GetSingle());
    fLLPosition->Longitude(fBuffer->GetSingle());
    fLLPosition->Altitude(fBuffer->GetSingle());
    fLLPosition->ClockBias(fBuffer->GetSingle());
    // 01-Jan-06 This could be double depending on how this is setup. 
    // Unlikely for our version of firmware.
    fLLPosition->Seconds(fBuffer->GetSingle());

    SET_DEBUG_STACK;
    return ExpectedBytes; 
}

/**
 ******************************************************************
 *
 * Function Name : Decode_PositionDouble (0x84)
 *
 * Description : When a double precision Latitude-Longitude 
 * position fix is detected, it is stored here. 
 *
 * Inputs : address of pointer to data, it is incremented .
 * 15-jun-02 Tested OK!
 *
 * Returns : Number of bytes expected
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_PositionDouble (void)
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (36 + 4);

    fError = NO_DECODE_ERROR;
    fLLPosition->Stamp();
    fLLPosition->Valid(true);
    fLLPosition->Latitude(fBuffer->GetDouble());
    fLLPosition->Longitude(fBuffer->GetDouble());
    fLLPosition->Altitude(fBuffer->GetDouble());
    fLLPosition->ClockBias(fBuffer->GetDouble()); // Clock bias
    // 01-Jan-06 This could be double depending on how this is setup. 
    // Unlikely for our version of firmware.
    fLLPosition->Seconds(fBuffer->GetSingle());  // time of fix.

    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_VelocityENU (0x56)
 *
 * Description : Decode the velocity message for East North and Up 
 * components. 
 *
 * Inputs : address of data buffer pointer. This is incremented as
 * the data is accessed.  29-Jun-08 OK!
 *
 * Returns : Number of bytes expected. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_VelocityENU ()
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (10 + 4);
    fError = NO_DECODE_ERROR;

    fENUVelocity->Stamp();
    fENUVelocity->East(fBuffer->GetSingle());
    fENUVelocity->North(fBuffer->GetSingle());
    fENUVelocity->Up(fBuffer->GetSingle());
    fENUVelocity->ClockBiasRate(fBuffer->GetSingle());
    // 01-Jan-06 This could be double depending on hos this is setup. 
    // Unlikely for our version of firmware.
    fENUVelocity->Seconds(fBuffer->GetSingle());
    
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SignalLevels (0x47)
 *
 * Description : Get the signal level for all tracked SV's
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Tested OK 15-Jun-02
 *
 * Returns : Number of expected bytes. 
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SignalLevels ()
{
    SET_DEBUG_STACK;
    int i;
    unsigned char ID;
    float         value;

    // Number of bytes used is 4 + N satellites * 5 bytes each composed
    // of PRN, single - signal level
    int rv = 4;  
    fError = NO_DECODE_ERROR;

    for (i = 0; i < MAXPRN; i++)
    {
	fSLevel[i].Clear();
    }

    // First byte is number of satellites in report. 
    int count = (int) fBuffer->GetChar(); rv++;

    for (i = 0; i <count; i++)
    {
	ID    = fBuffer->GetChar();   rv++;
	value = fBuffer->GetSingle(); rv +=4;
	if (count<MAXPRNCOUNT)
	{
	    fSLevel[i].Now();
	    fSLevel[i].Set(ID,value);
	}
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : LoadAlmanac
 *
 * Description : When called, fills almanac parameters. 
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 *         Number of bytes available
 *         PRN of satellite this data pertains to.
 *
 * Tested OK 17-Jun-02 
 *
 * Returns : Number of expected bytes
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::LoadAlmanac (unsigned char Nbytes, int PRN)
{
    SET_DEBUG_STACK;
    // Expected bytes includes 4 bytes of common
    // satellite system report data.
    //const int ExpectedBytes = (70 + 4);
    fError = NO_DECODE_ERROR;

    //    unsigned char *Start = (*data);
    fAlmanac.t_oa_raw  = fBuffer->GetChar();
    fAlmanac.SV_HEALTH = fBuffer->GetChar();
    fAlmanac.e         = fBuffer->GetSingle();
    fAlmanac.t_oa      = fBuffer->GetSingle();
    fAlmanac.i_o       = fBuffer->GetSingle();
    fAlmanac.OMEGADOT  = fBuffer->GetSingle();
    fAlmanac.sqrt_A    = fBuffer->GetSingle();
    fAlmanac.OMEGA_0   = fBuffer->GetSingle();
    fAlmanac.omega     = fBuffer->GetSingle();
    fAlmanac.M_0       = fBuffer->GetSingle();
    fAlmanac.a_f0      = fBuffer->GetSingle();
    fAlmanac.a_f1      = fBuffer->GetSingle();
    fAlmanac.Axis      = fBuffer->GetSingle();
    fAlmanac.n         = fBuffer->GetSingle();
    fAlmanac.OMEGA_n   = fBuffer->GetSingle();
    fAlmanac.ODOT_n    = fBuffer->GetSingle();
    fAlmanac.t_zc      = fBuffer->GetSingle();
    fAlmanac.weeknum   = fBuffer->GetInt();
    fAlmanac.wn_oa     = fBuffer->GetInt();

    SET_DEBUG_STACK;
    return 66;
}


/**
 ******************************************************************
 *
 * Function Name : Decode_IO_Options
 *
 * Description :  Decode the options that the user has requested.  
 * Verified: 07-Jul-02
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 *
 *******************************************************************
 */
int Lassen::Decode_IO_Options (void)
{
    SET_DEBUG_STACK;

    CLogger* pLogger = CLogger::GetThis();
    fError = NO_DECODE_ERROR;
    fIO_Options.Set = true;
    /**
     * Bit		Assignment
     *  0		XYZ ECEF
     *  1       LLA
     *  2       LLA-ALT Output HAE=0, MSL=1
     *  3       LLA-ALT Output HAE=0, MSL=1
     *  4       Precision Single=0, Double=1
     *  5       Enable SuperPackets
     *  6       Not Used
     *  7       Not Used
     */
    fIO_Options.Pos = fBuffer->GetChar();

    /**
     * Bit		Assi9gnment
     *  0		XYZ ECEF Velocity
     *	1		NEU Velocity
     *	2		Not Used
     *	3		Not Used
     *	4		Not Used
     *	5		Not Used
     *	6		Not Used
     *	7		Not Used
     */
    fIO_Options.Vel = fBuffer->GetChar();

    /**
     * Bit		Assignment
     *  0		GPS Time=0, UTC Time = 1 
     *	1		Automatic output of fix time (Response 0x37)
     *	2		Simultaneous Measurements
     *	3		Minimum Projection
     *	4		Reserved
     *	5		Not Used
     *	6		Not Used
     *	7		Not Used
     */
    fIO_Options.Timing = fBuffer->GetChar();

    /**
     * Bit		Assignment
     *  0		Measurement output (Always 0 for lassen)
     *	1		Codephase RAW PR=0, Filtered PR=1  in Response 5A.
     *	2		Automatic output of Additiona Fix Status 0x5E
     *	3		Units for Signal Level Output: AMU=0, DBhz=1
     *	4		Not Used
     *	5		Not Used
     *	6		Not Used
     *	7		Not Used
     */
    fIO_Options.Aux = fBuffer->GetChar();

    if (pLogger && (fVerbosity > 0))
    {
	pLogger->LogTime("Decode_IO_Options: ");
	OutputPositionOptions(*pLogger->LogPtr(), fIO_Options.Pos);
	OutputVelocityOptions(*pLogger->LogPtr(), fIO_Options.Vel);
	OutputTimingOptions(*pLogger->LogPtr(), fIO_Options.Timing);
	OutputAuxOptions(*pLogger->LogPtr(), fIO_Options.Aux);
	pLogger->Log("\n");
    }

    SET_DEBUG_STACK;
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : LoadAlmanacHealth
 *
 * Description : 
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Checked OK 17-Jun-02
 *
 * Returns : Number of expected bytes.
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::LoadAlmanacHealth (	unsigned char Nbytes, int PRN)
{
    SET_DEBUG_STACK;
    // Expected bytes includes 4 bytes of common
    // satellite system report data.
    //const int ExpectedBytes = (41 + 4);
    fError = NO_DECODE_ERROR;

    memset (&fAlmanacHealth, 0, sizeof (t_AlmanacHealth));
    fAlmanacHealth.WeekNumber = fBuffer->GetChar();

    for (int i = 0; i < 32; i++)
    {
        fAlmanacHealth.SV_Health[i] = fBuffer->GetChar();
    }
    fAlmanacHealth.t_oa         = fBuffer->GetChar();
    fAlmanacHealth.current_t_oa = fBuffer->GetChar();
    fAlmanacHealth.CurrentWeek  = fBuffer->GetInt();
    SET_DEBUG_STACK;
    return 37;
}

/**
 ******************************************************************
 *
 * Function Name : LoadIono
 *
 * Description : Load ionospheric corrections.
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 *
 *
 * Returns : number of expected bytes. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::LoadIono (unsigned char Nbytes, int PRN)
{
    int i;
    SET_DEBUG_STACK;
    // Expected bytes includes 4 bytes of common
    // satellite system report data.
    //const int ExpectedBytes = (44 + 4);
    fError = NO_DECODE_ERROR;

    memset (&fIonosphere, 0, sizeof (t_iono));
    // 8 unused bytes preceed this message. 
    fBuffer->Skip(8);

    for (i = 0; i < 4; i++)
    {
        fIonosphere.alpha[i] = fBuffer->GetSingle ();
    }
    for (i = 0; i < 4; i++)
    {
        fIonosphere.beta[i] = fBuffer->GetSingle ();
    }
    SET_DEBUG_STACK;
    return 40;
}

/**
 ******************************************************************
 *
 * Function Name : LoadUTC
 *
 * Description : Get UTC corrections for this PRN. 
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 *
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::LoadUTC (unsigned char Nbytes, int PRN)
{
    //    unsigned char *Start = (*data);
    SET_DEBUG_STACK;
    // Expected bytes includes 4 bytes of common
    // satellite system report data.
    //const int ExpectedBytes = (43 + 4);
    fError = NO_DECODE_ERROR;

    memset (&fUTC, 0, sizeof (t_UTCData));
    // 13 unused bytes preceed this data
    fBuffer->Skip(13);

    fUTC.A0          = fBuffer->GetDouble ();
    fUTC.A1          = fBuffer->GetSingle ();
    fUTC.delta_t_LS  = fBuffer->GetInt ();
    fUTC.t_ot        = fBuffer->GetSingle ();
    fUTC.WN_t        = fBuffer->GetInt ();
    fUTC.WN_LSF      = fBuffer->GetInt ();
    fUTC.DN          = fBuffer->GetInt ();
    fUTC.delta_t_LSF = fBuffer->GetInt ();

    SET_DEBUG_STACK;
    return 39;
}

/**
 ******************************************************************
 *
 * Function Name : LoadEphemeris
 *
 * Description : 
 *
 * Inputs : Nbytes - 
 *          PRN -  
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::LoadEphemeris (unsigned char Nbytes, int PRN)
{
    //unsigned char *Start = (*data);
    SET_DEBUG_STACK;
    // Expected bytes includes 4 bytes of common
    // satellite system report data.
    //const int ExpectedBytes = (171 + 4);
    fError = NO_DECODE_ERROR;

    memset (&fEphemeris, 0, sizeof (t_Ephemeris));

    fEphemeris.svid         = fBuffer->GetChar();
    fEphemeris.t_ephem      = fBuffer->GetSingle();
    fEphemeris.weeknum      = fBuffer->GetInt();
    fEphemeris.codeL2       = fBuffer->GetChar();
    fEphemeris.L2Pdata      = fBuffer->GetChar();
    fEphemeris.SVacc_raw    = fBuffer->GetChar();
    fEphemeris.SV_health    = fBuffer->GetChar();
    fEphemeris.IODC         = fBuffer->GetInt();
    fEphemeris.T_GD         = fBuffer->GetSingle();
    fEphemeris.t_oc         = fBuffer->GetSingle();
    fEphemeris.a_f2         = fBuffer->GetSingle();
    fEphemeris.a_f1         = fBuffer->GetSingle();
    fEphemeris.a_f0         = fBuffer->GetSingle();
    fEphemeris.SVacc        = fBuffer->GetSingle();
    fEphemeris.IODE         = fBuffer->GetChar();
    fEphemeris.fit_interval = fBuffer->GetChar();

    fEphemeris.C_rs         = fBuffer->GetSingle();
    fEphemeris.delta_n      = fBuffer->GetSingle();
    fEphemeris.M_O          = fBuffer->GetDouble();
    fEphemeris.C_uc         = fBuffer->GetSingle();
    fEphemeris.e            = fBuffer->GetDouble();
    fEphemeris.C_us         = fBuffer->GetSingle();
    fEphemeris.sqrt_A       = fBuffer->GetDouble();
    fEphemeris.t_oe         = fBuffer->GetSingle();
    fEphemeris.C_ic         = fBuffer->GetSingle();
    fEphemeris.OMEGA_O      = fBuffer->GetDouble();
    fEphemeris.C_is         = fBuffer->GetSingle();
    fEphemeris.i_O          = fBuffer->GetDouble();
    fEphemeris.C_rc         = fBuffer->GetSingle();
    fEphemeris.omega        = fBuffer->GetDouble();
    fEphemeris.OMEGADOT     = fBuffer->GetSingle();
    fEphemeris.IDOT         = fBuffer->GetSingle();
    fEphemeris.Axis         = fBuffer->GetDouble();   // Sqrt_A^2
    fEphemeris.n            = fBuffer->GetDouble();   // Derived from delta_n
    fEphemeris.r1me         = fBuffer->GetDouble();   // sqrt(1-e^2)
    fEphemeris.OMEGA_n      = fBuffer->GetDouble();   // derived from OMEGA_0 OMEGADOT
    fEphemeris.ODOT_n       = fBuffer->GetDouble();   // derived from OMEGADOT

    SET_DEBUG_STACK;
    return 167;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SatelliteData (0x58)
 *
 * Description : Get some very specific data. 
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Checked OK 15-Jun-02
 *
 * Returns : Number of bytes actually expected
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_SatelliteData ()
{
    int rv = 4; // Prefix, ID + DLE ETX
    unsigned char Operation = fBuffer->GetChar();
    SET_DEBUG_STACK;
    fError = NO_DECODE_ERROR;

    rv++;
    if (Operation == 2)
    {
        unsigned char DataType = fBuffer->GetChar();
        rv++;
        unsigned char SatPRN   = fBuffer->GetChar();
        rv++;
        unsigned char NBytes   = fBuffer->GetChar();
        rv++;

        switch (DataType)
        {
        case GPS_ALMANAC:
            rv += LoadAlmanac (NBytes, SatPRN);
            break;
        case GPS_HEALTH:
            rv += LoadAlmanacHealth (NBytes, SatPRN);
            break;
        case GPS_IONOSPHERE:
            rv += LoadIono (NBytes, SatPRN);
            break;
        case GPS_UTC:
            rv += LoadUTC (NBytes, SatPRN);
            break;
        case GPS_EPHEMERIS:
            rv += LoadEphemeris (NBytes, SatPRN);
            break;
        }
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SatelliteDisable (0x59)
 *
 * Description :  
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 *
 *
 * Returns : Number of expected bytes. 
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
int Lassen::Decode_SatelliteDisable ()
{
    SET_DEBUG_STACK;
    const int ExpectedBytes = (33 + 4);
    fError = NO_DECODE_ERROR;

    char Operation = fBuffer->GetChar();

    for (int i = 0; i < 32; i++)
    {
        fUseSatellite[i] = Operation * fBuffer->GetChar();
    }
    
    SET_DEBUG_STACK;
    return ExpectedBytes;
}
/**
 ******************************************************************
 *
 * Function Name : Fill_SatelliteRaw
 *
 * Description : Allow multiple fills. 
 *
 * Inputs : line to be decoded, there are 25 bytes of data per line
 *          There may be multiple lines in a fBuffer so this may be 
 *          called multiple times. 
 *
 * Returns : number of bytes decoded. 
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 13-Apr-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
int Lassen::Fill_SatelliteRaw ()
{
    SET_DEBUG_STACK;
    CLogger* pLogger = CLogger::GetThis();
    fError = NO_DECODE_ERROR;
    /*
     * PRN can vary from 0 to 255 by definition. 
     *
     * This message is received once per Satellite participating 
     * in the solution. 
     */
    unsigned char PRN = fBuffer->GetChar();
    // Make sure we don't overflow the storage. 
    if (fPRNCount < MAXPRNCOUNT)
    {
	fRawData[fPRNCount].PRN          = PRN; 
	// in milliseconds.
	fRawData[fPRNCount].SampleLength = fBuffer->GetSingle(); 
	// in either AMU or dBHz
	fRawData[fPRNCount].SignalLevel  = fBuffer->GetSingle();
	// in units of 1/16 of a chip.
	fRawData[fPRNCount].CodePhase    = fBuffer->GetSingle();
	// Hertz
	fRawData[fPRNCount].Doppler      = fBuffer->GetSingle(); 
	// Seconds
	fRawData[fPRNCount].TimeOfMeasurement = fBuffer->GetDouble();
	fPRNCount++;  // Must zero when packet 6D is received.
	SET_DEBUG_STACK;
    }
    else
    {
	if(pLogger)
	{
	    pLogger->Log("# %s %d PRN Count Exceeded. %d \n",
			 __FILE__, __LINE__, PRN);
	}
    }
    SET_DEBUG_STACK;
    return 25;       // 25 Base and DLE ETX as suffix.
}


/**
 ******************************************************************
 *
 * Function Name : Decode_SatelliteRaw (0x5A)
 *
 * Description :  
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Checked OK 29-June-08
 *
 * Returns : Number of expected bytes;
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SatelliteRaw ()
{
    SET_DEBUG_STACK;
    //const int Expected = (25+4);
    int rv = 0;
    int BytesUsed, nb;
    int N = 0;

    fError = NO_DECODE_ERROR;
    BytesUsed = 2;
    do {
	N++;
	BytesUsed += Fill_SatelliteRaw( );
	nb = fBuffer->Remaining();
	//(*errorLog) << "RAW: " << N << " nb: " << nb << endl;
    } while (nb > 25);

    rv = BytesUsed;

    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SatelliteRawTracking (0x5C)
 *
 * Description :  
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Checked OK 18-Jun-02
 * 16-Oct-15 changed expected bytes from 23+4 to 24+4
 *           unpack all data, then decide if we want to store it. 
 *
 * Returns : Number of expected bytes. 
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SatelliteRawTracking ()
{
    SET_DEBUG_STACK;
    CLogger* pLogger = CLogger::GetThis();
    int ptr;
    fError = NO_DECODE_ERROR;
    const int ExpectedBytes = (24 + 4);
    unsigned char PRN        = fBuffer->GetChar();
    char  ChannelCode        = fBuffer->GetChar();
    char  Acquisitionflag    = fBuffer->GetChar();
    char  EphemerisFlag      = fBuffer->GetChar();
    float SignalLevel        = fBuffer->GetSingle();
    // Seconds
    float GPS_TimeofLastMeasurement=fBuffer->GetSingle();
    // Radians
    float Elevation          = fBuffer->GetSingle(); 
    float Azimuth            = fBuffer->GetSingle();
    char  OldMeasurementFlag = fBuffer->GetChar();
    char  MSecFlag           = fBuffer->GetChar();
    char  BadDataFlag        = fBuffer->GetChar();
    char  DataCollectionFlag = fBuffer->GetChar();
    // Are we actually tracking something??
    if (Acquisitionflag>0)
    {
	// Determine if this is already in the array.
	ptr = FindRawTrackingPRN(PRN);
	if (ptr < 0)
	{
	    // it is not in the array, allocate a new space. 
	    ptr = fPRNCountB;
	    fPRNCountB++;    // Reset when message 6D is received.
	    if ((fPRNCountB > MAXPRNCOUNT) && pLogger)
	    {
		//Notify user, and still decode. 
		pLogger->LogTime("Raw Tracking, buffer overflow: %d\n", PRN);
		ptr = MAXPRNCOUNT-1;
	    }
	}
	fpRawTracking[ptr].Stamp(); 
	fpRawTracking[ptr].SetValid();
	fpRawTracking[ptr].PRN(PRN);
	// Put a local timestamp on when we got this data. 
	fpRawTracking[ptr].ChannelCode(ChannelCode);
	fpRawTracking[ptr].Acquisitionflag(Acquisitionflag);
	fpRawTracking[ptr].EphemerisFlag(EphemerisFlag);
	fpRawTracking[ptr].SignalLevel(SignalLevel);
	// Seconds
	fpRawTracking[ptr].GPS_TimeofLastMeasurement(GPS_TimeofLastMeasurement);
	// Radians
	fpRawTracking[ptr].Elevation(Elevation);
	fpRawTracking[ptr].Azimuth(Azimuth);
	fpRawTracking[ptr].OldMeasurementFlag(OldMeasurementFlag);
	fpRawTracking[ptr].MSecFlag(MSecFlag);
	fpRawTracking[ptr].BadDataFlag(BadDataFlag);
	fpRawTracking[ptr].DataCollectionFlag(DataCollectionFlag);
    }
    SET_DEBUG_STACK;
    return ExpectedBytes;
}

/**
 ******************************************************************
 *
 * Function Name :Decode_SyncronizedPacket (0x6F)
 *
 * Description :  Decode  a synchronized packet.
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Unfinished ----------------------------------------------------------
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SyncronizedPacket ()
{
    SET_DEBUG_STACK;
    CLogger* pLogger = CLogger::GetThis();
    if (pLogger)
    {
	pLogger->LogTime("Decoding sync packet!\n");
    }

    fError = NO_DECODE_ERROR;
#if 0
    unsigned char Subcode = (**ptr);
    SET_DEBUG_STACK;
    (*ptr)++;                   // Should be filled with 1
    unsigned char Preamble = (**ptr);
    (*ptr)++;                   // Should be filled with 2
    short Length = StuffInt (ptr);      // Number of bytes 
    double RxTime = fBuffer->GetDouble();  // MSec GPS week
    double ClockOffset = fBuffer->GetDouble();     // Receiver clock offset
    int NSV = (**ptr);
    (*ptr)++;                   // Number of satellites
    memset (SP, 0, 8 * sizeof (struct t_SynchronizedPacket));

    for (int i = 0; i < NSV; i++)
    {
        SP[i].SVPRN = (**ptr);
        (*ptr)++;
        SP[i].Flags1 = (**ptr);
        (*ptr)++;
        SP[i].Flags2 = (**ptr);
        (*ptr)++;
        SP[i].Elevation = (**ptr);
        (*ptr)++;               // Elevation angle in degrees
        SP[i].Azimuth = StuffInt (ptr); // Azimuth in degrees
        SP[i].SNR = (**ptr);
        (*ptr)++;               // Number of AMUs * 4
        SP[i].Pseudorange = fBuffer->GetDouble();  // Full L1 C/A Pseudorange, filtered. 
        SP[i].CarrierPhase = fBuffer->GetDouble(); // L1 band Continous Phase truncated to integer value.
        SP[i].Doppler = fBuffer->GetSingle();      // L1 band Doppler (hertz)
    }
    unsigned short Checksum = StuffInt (ptr);
    char Postamble = (**ptr);
    (*ptr)++;                   // Should be 3
#endif
    // Unfinished. 
    SET_DEBUG_STACK;
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_SynchronizedMeasurementsReply
 *
 * Description :  Decode status of synchronized messages.
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Unfinished ----------------------------------------------------------
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SynchronizedMeasurementsReply ()
{
    SET_DEBUG_STACK;
    fError = NO_DECODE_ERROR;
#if 0
    char Subcode = (**data);
    SET_DEBUG_STACK;
    (*data)++;
    char OnOff = (**data);
    (*data)++;
    char Interval = (**data);
    (*data)++;
#endif
    SET_DEBUG_STACK;
    return 0;
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
 *******************************************************************
 */
void Lassen::SetOscillatorOffset (const float offset, const bool set)
{
    SET_DEBUG_STACK;
    LoadSuffix ();
    SendChar (CLEAR_OSCILLATOR_OFFSET);

    if (!set)
    {
        SendChar ('C');
    }
    else
    {
        SendSingle (offset);
    }
    LoadSuffix ();
//    SendData ();
    SET_DEBUG_STACK;
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
 *******************************************************************
 */
void Lassen::SetECEF (const float X, const float Y, const float Z)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_POSITION_XYZ);
    SendSingle (X);
    SendSingle (Y);
    SendSingle (Z);
    LoadSuffix ();
    SET_DEBUG_STACK;
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
 *******************************************************************
 */
void Lassen::SetSystemAltitude (const float alt)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_ALTITUDE);
    if (alt == 0.0)
    {
        SendChar (0);
    }
    else
    {
        SendSingle (alt);
    }
    LoadSuffix ();
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
 *******************************************************************
 */
void Lassen::SetPosition (const float Latitude, const float Longitude,
                          const float Altitude)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_POSITION_LLA);
    SendSingle (Latitude);
    SendSingle (Longitude);
    SendSingle (Altitude);
    LoadSuffix ();

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
 *******************************************************************
 */
void Lassen::SetGPSTime (const float Seconds, const short Week)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_TIME_OF_WEEK);
    SendSingle (Seconds);
    SendInteger (Week);
    LoadSuffix ();

}

/**
 ******************************************************************
 *
 * Function Name : SetAccurateECEF
 *
 * Description : Only use this when a highly precise XYZ is known.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
void Lassen::SetAccurateECEF (const float X, const float Y, const float Z)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_POSITION_XYZ_ACCURATE);
    SendSingle (X);
    SendSingle (Y);
    SendSingle (Z);
    LoadSuffix ();
}

/**
 ******************************************************************
 *
 * Function Name : SetAccuratePosition
 *
 * Description : Only use this when a highly precise LLA is known.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
void Lassen::SetAccuratePosition (const float Latitude, const float Longitude,
                                  const float Altitude)
{
    SET_DEBUG_STACK;
    LoadPrefix (SET_POSITION_LLA_ACCURATE);
    SendSingle (Latitude);
    SendSingle (Longitude);
    SendSingle (Altitude);
    LoadSuffix ();
}

/**
 ******************************************************************
 *
 * Function Name : SetIO_Options
 *
 * Description : Setup the IO options. Send Packet (0x35)
 *
 * Inputs : Position
 *               0 (LSB) - XYZ ECEF 0-off, 1-on
 *               1       - LLA 0-off, 1-on
 *               2       - LLA ALT Output 
 *                             0-HAE(datum)
 *                             1-MSL geoid
 *               3       - ALT input 
 *                         0-HAE(datum)
 *                         1-MSL geoid
 *               4       - Precision of position output 
 *                             0 - Single precision
 *                             1 - double precision
 *               5       - Super Packets - 0-off, 1-on
 *               6       - N/A
 *               7 (MSB) - N/A
 *          Velocity
 *               0 (LSB) - XYZ ECEF Output 0-off, 1-on
 *               1       - ENU Output 0-off, 1-on
 *               2       - N/A
 *               3       - N/A
 *               4       - N/A
 *               5       - N/A
 *               6       - N/A
 *               7 (MSB) - N/A
 *
 *          Timing
 *               0 (LSB) - 0-GPS, 1-UTC
 *               1       - N/A
 *               2       - N/A
 *               3       - N/A
 *               4       - N/A
 *               5       - N/A
 *               6       - N/A
 *               7 (MSB) - N/A
 *
 *          AuxPR
 *               0 (LSB) - ?
 *               1       - 0- Raw PRs in 0x5A, 1-filterd PRs in 0x5A
 *               2       - N/A
 *               3       - 0 - ouput dBHz, 1- output AMU
 *               4       - N/A
 *               5       - N/A
 *               6       - N/A
 *               7 (MSB) - N/A
 *
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 *
 *******************************************************************
 */
void Lassen::SetIO_Options (const unsigned char Position,
                            const unsigned char Velocity, 
			    const unsigned char Timing,
                            const unsigned char AuxPR)
{
    SET_DEBUG_STACK;
    CLogger* pLogger = CLogger::GetThis();

    // Make sure order is correct. 
    // ID PREFIX Velocity Position AuxPR Timing
    LoadPrefix (SET_IO_OPTION_STATE);
    SendChar (Position);
    SendChar (Velocity);
    SendChar (Timing);
    SendChar (AuxPR);
    LoadSuffix ();
    if (pLogger && (fVerbosity > 0))
    {
	pLogger->LogTime("SetIO_Options: ");
	OutputPositionOptions(*pLogger->LogPtr(), Position);
	OutputVelocityOptions(*pLogger->LogPtr(), Velocity);
	OutputTimingOptions(*pLogger->LogPtr(), Timing);
	OutputAuxOptions(*pLogger->LogPtr(), AuxPR);
	pLogger->Log("\n");
    }
}
/**
 ******************************************************************
 *
 * Function Name : SetIO_Options
 *
 * Description : Use structure to set the options. 
 *
 * Inputs : struct t_IO_Options
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Lassen::SetIO_Options (const struct t_IO_Options *in)
{
    SET_DEBUG_STACK;
    CLogger* pLogger = CLogger::GetThis();
    LoadPrefix (SET_IO_OPTION_STATE);
    SendChar (in->Pos);
    SendChar (in->Vel);
    SendChar (in->Timing);
    SendChar (in->Aux);
    LoadSuffix ();
    
    if (pLogger && (fVerbosity > 0))
    {
	pLogger->LogTime("SetIO_Options: ");
	OutputPositionOptions(*pLogger->LogPtr(), in->Pos);
	OutputVelocityOptions(*pLogger->LogPtr(), in->Vel);
	OutputTimingOptions(*pLogger->LogPtr(), in->Timing);
	OutputAuxOptions(*pLogger->LogPtr(), in->Aux);
	pLogger->Log("\n");
    }
}

/**
 ******************************************************************
 *
 * Function Name : SetSatelliteData
 *
 * Description : USE WITH EXTREME CAUTION.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
void Lassen::SetSatelliteData (const GPS_DATA_TYPE type, const char PRN,
                               const char *data, int data_size)
{
    SET_DEBUG_STACK;
    LoadPrefix (GET_OR_SET_ALM);
    SendChar (type);
    char prn = PRN % 33;
    SendChar (prn);
    SendChar (data_size);
    for (int i = 0; i < data_size; i++)
    {
        SendChar (data[i]);
    }
    LoadSuffix ();
}

/**
 ******************************************************************
 *
 * Function Name : SetFilter
 *
 * Description : Turn various filters on and off. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
void Lassen::SetFilter (const bool PV, const bool Static, const bool Altitude)
{
    SET_DEBUG_STACK;
    LoadPrefix (REQUEST_FILTER);
    if (PV)
    {
        SendChar (1);
    }
    else
    {
        SendChar (0);
    }
    if (Static)
    {
        SendChar (1);
    }
    else
    {
        SendChar (0);
    }
    if (Altitude)
    {
        SendChar (1);
    }
    else
    {
        SendChar (0);
    }

    LoadSuffix ();
};

/** Super PACKET stuff below.  */
/**
 ******************************************************************
 *
 * Function Name : Decode_SuperPacket2
 *
 * Description : When called, parsels out super packet 2 data. 
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Unfinished ----------------------------------------------------------
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_SuperPacket2 ()
{
    SET_DEBUG_STACK;
#if 0
    char *MessageDump = "UNKNOWN SP2";
    char SubCode = **data;
    (*data)++;
    switch (SubCode)
    {
    case COMPREHENSIVE_TIME:
	MessageDump = "Comprehensive Time Reply";
	break;
    case CURRENT_DATUM:
	MessageDump = "Current Datum";
	break;
    case LAST_FIX_EXTRA:                 // Position SuperPacket. 
	MessageDump = "Last Fix Extra";
        Decode_PositionSuperPacket (data);
        break;
    case EEPROM_SEGMENTS:
	MessageDump = "EEPROM Segments";
	break;
    case MANUFACTURING_PARAMETERS:
	MessageDump = "Manufacturing parameters";
	break;
    case PRODUCTION_PARAMETERS:
	MessageDump = "Production Parameters";
	break;
    case PPS_CHARACTERISTICS:
	MessageDump = "PPS characteristics";
	break;
    case AUTOMATIC_PACKET_OUTPUT_MASK:
	MessageDump = "Automatic Packet output mask";
	break;
    case TIME_DATA:
	MessageDump = "Time Data";
	DecodeTimeData(data);
	break;
    case SUPPLEMENTAL_TIME_DATA:
	MessageDump = "Supplemental time data";
	DecodeSupplementalTimeData(data);
	break;
    case PRIMARY_UTC_TIME:
	MessageDump = "Primary UTC Time";
	break;
    }
    SET_DEBUG_STACK;
    (*data) += 2;
    SET_DEBUG_STACK;
    if (Verbosity > 1)
    {
	if(errorLog)
	{
	    (*errorLog) << MessageDump  <<  endl;
	}
    }
    if (logdata)
    {
	(*logdata) << endl << MessageDump << endl;
    }
#endif
    SET_DEBUG_STACK;
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : Decode_PositionSuperPacket
 *
 * Description : When called, gets super packet sub code 20.
 *
 * Inputs :Address of data pointer, Increments appropriately. 
 * Unfinished ----------------------------------------------------------
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
int Lassen::Decode_PositionSuperPacket ()
{
    SET_DEBUG_STACK;
#if 0  // Unfinished
    char TrimblePostProc = **data;
    (*data)++;
    // Velocities are in 0.005 or 0.02 M/S depending on Byte 24
    int VelocityEast = fBuffer->GetInt();
    int VelocityNorth = fBuffer->GetInt();
    int VelocityUP = fBuffer->GetInt();
    // GPS time of week in ms
    int GPSTime = StuffLongInt (data);
    // Lat and lon are in 2E31 semicircles.
    int Latitude = StuffLongInt (data);
    int Longitude = StuffLongInt (data);
    // In mm
    int Altitude = StuffLongInt (data);
    // When bit 0 is set to 1, Velocities have been scaled by 4.
    char Scaling = **data;
    (*data)++;
    char Reserved = **data;
    (*data)++;
    // Datum 0- unknown otherwise, index+1. See table in documentation. 
    char Datum = **data;
    (*data)++;

        /**
	 * Type of fix. This is a set of bit flags.
     * 0 (LSB) 0: Fix was available
     * 1: No fix available
     * 1 0: Fix is autonomous
     * 1: Fix was corrected with RTCM
     * 2 0: 3D fix
     * 1: 2D fix
     * 3 0: 2D fix used last-calculated
     *           altitude
     * 1: 2D fix used entered altitude
     * 4 0: unfiltered
     * 1: position or altitude filter on
     * 5-7 not used (always 0)
     */
    char FixType = **data;
    (*data)++;
    char NumSV = **data;
    (*data)++;
    char UTC_offset = **data;
    (*data)++;
    int Week = fBuffer->GetInt();
    int i;
    unsigned char tmp;
    char PRN;
    int IODC;
    char IODE;
    for (i = 0; i < MAXPRNCOUNT; i++)
    {
        tmp = **data;
        (*data)++;
        PRN = tmp & 0x3F;
        // Top 2 bits is (IODC-IODE)/256
        IODC = ((tmp >> 5) & 0x03) << 8;
        IODE = **data;
        (*data)++;
        IODC += IODE;
    }
    char iono;
    for (i = 0; i < 8; i++)
    {
        iono = **data;
        (*data)++;
    }
#endif
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 * Unfinished ----------------------------------------------------------
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
int Lassen::DecodeTimeData()
{
    SET_DEBUG_STACK;
    fTimeData.TimeOfWeek = fBuffer->GetLongInt ();      // Seconds
    fTimeData.WeekNumber = fBuffer->GetInt();
    fTimeData.UTC_Offset = fBuffer->GetInt();
    fTimeData.TimingFlag = fBuffer->GetChar();

    fDataOutOfBounds = (fTimeData.TimingFlag > 0x0F);
    fTimeData.Seconds    = fBuffer->GetChar();

    if (fTimeData.Seconds > 60)
    {
	fDataOutOfBounds = true;
    }

    fTimeData.Minutes    = fBuffer->GetChar();
    if (fTimeData.Minutes > 60)
    {
	fDataOutOfBounds = true;
    }

    fTimeData.Hours      = fBuffer->GetChar();
    if (fTimeData.Hours> 24)
    {
	fDataOutOfBounds = true;
    }

    fTimeData.DayOfMonth = fBuffer->GetChar();
    if (fTimeData.DayOfMonth > 31)
    {
	fDataOutOfBounds = true;
    }

    fTimeData.Month      = fBuffer->GetChar();
    if (fTimeData.Month > 12)
    {
	fDataOutOfBounds = true;
    }

    fTimeData.Year       = fBuffer->GetInt();
    if (fTimeData.Year> 3000)
    {
	fDataOutOfBounds = true;
    }
    return 0;
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
int Lassen::DecodeSupplementalTimeData ()
{
    SET_DEBUG_STACK;

    fSTimeData.ReceiverMode = fBuffer->GetChar(); // Reserved 
    if (fSTimeData.ReceiverMode > 7)
    {
	fDataOutOfBounds = true;
	fSTimeData.ReceiverMode = 7;
    }

    fSTimeData.SelfSurveyProgress = fBuffer->GetChar();
    if (fSTimeData.SelfSurveyProgress>100)
    {
	fDataOutOfBounds = true;
	fSTimeData.SelfSurveyProgress = 100;
    }

    fBuffer->Skip(6);                  // Reserved;

    fSTimeData.MinorAlarms = fBuffer->GetInt();
    if (fSTimeData.MinorAlarms > 4095)
    {
	fSTimeData.MinorAlarms = 4095;
    }
    fSTimeData.GPSDecodingStatus = fBuffer->GetChar();
    if (fSTimeData.GPSDecodingStatus > 0x1F)
    {
	fSTimeData.GPSDecodingStatus = 0x1F;
    }
    fBuffer->Skip(3);                  // Reserved;
    fSTimeData.Bias      = fBuffer->GetSingle();      // Nanoseconds
    if (fSTimeData.Bias > 9999999.0)
    {
	fSTimeData.Bias = 9999999.0;
    }

    fSTimeData.BiasRate  = fBuffer->GetSingle();  // ppb
    if (fSTimeData.BiasRate > 99999999.0)
    {
	fSTimeData.BiasRate = 99999999.0;
    }
    fBuffer->Skip(12);

    fSTimeData.Latitude  = fBuffer->GetDouble();  // radians
    if (fSTimeData.Latitude > 90.0)
    {
	fSTimeData.Latitude = 90.0;
    }
    else if (fSTimeData.Latitude < -90.0)
    {
	fSTimeData.Latitude = -90.0;
    }

    fSTimeData.Longitude = fBuffer->GetDouble(); // radians
    if (fSTimeData.Longitude > 180.0)
    {
	fSTimeData.Longitude = 180.0;
    }
    else if (fSTimeData.Longitude<-180.0)
    {
	fSTimeData.Longitude = -180.0;
    }
    fSTimeData.Altitude  = fBuffer->GetDouble();  // Meters
    if (fSTimeData.Altitude > 99999999.0)
    {
	fSTimeData.Altitude = 99999999.0;
    }
    else if (fSTimeData.Altitude < -99999999.0)
    {
	fSTimeData.Altitude = -99999999.0;
    }
    fSTimeData.PPSQuantizationError = fBuffer->GetSingle(); // Nanoseconds
    if (fSTimeData.PPSQuantizationError > 99999999.0)
    {
	fSTimeData.PPSQuantizationError = 99999999.0;
    }
    else if (fSTimeData.PPSQuantizationError < -99999999.0)
    {
	fSTimeData.PPSQuantizationError = -99999999.0;
    }
    fSTimeData.PPSOutputStatus = fBuffer->GetChar(); // Bit packed.
    SET_DEBUG_STACK;
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : Check1010
 *
 * Description : Since 0x10 is a frame marker, an extra 0x10 preceeds
 * every real databyte. This will strip them out of the buffer
 *
 * Inputs : NONE
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
bool Lassen::Check1010()
{
    unsigned char *found, *ptr;
    int           delta, NBytes, move;
    bool          rc = false;

    ptr    = fBuffer->GetData();
    NBytes = fBuffer->GetFill(); 
    found = (unsigned char *) memchr( ptr, 0x10, NBytes);
    if ((int)(found-ptr) == 0)
    {
	ptr++;
	// This is the beginning byte, skip it. 
	found = (unsigned char *) memchr( ptr, 0x10, NBytes);
    }

    if (found && (*(found+1)==0x10))
    {
	// Two consecutive 0x10 values are found, remove one. 
	delta = (int)(found-ptr); 
	// Do overlapping move to remove extra character. 
	move = NBytes-(delta+1);
	memmove( &ptr[delta], &ptr[delta+1], move);
	fBuffer->DecrementFillCount();
	rc = true;
    }
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
bool Lassen::Remove1010()
{
    SET_DEBUG_STACK;
    bool rv;
    do {
	rv = Check1010();
    } while (rv);
    SET_DEBUG_STACK;
    return rv;
}
 /**
 ******************************************************************
 *
 * Function Name :Clear RawTracking
 *
 * Description : Clear the raw tracking data 
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
void Lassen::ClearRawTracking()
{
    SET_DEBUG_STACK;
    int i;
    for (i=0;i<fPRNCountB;i++)
    {
	fpRawTracking[i].Clear();
    }
    fPRNCountB = 0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : FindRawTrackingPRN
 *
 * Description : return pointer to raw tracking index for PRN given
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
int Lassen::FindRawTrackingPRN(int prn) const
{
    SET_DEBUG_STACK;
    int i;
    int rc = -1;
    for (i=0;i<fPRNCountB;i++)
    {
	if(fpRawTracking[i].PRN() == prn)
	{
	    return i;
	}
    }
    return rc;
    SET_DEBUG_STACK;
}
void Lassen::ResetFrame(void)
{
    SET_DEBUG_STACK;
    ClearRawTracking();
    // Reset PRN Counters. 
    fPRNCount = 0;
    SET_DEBUG_STACK;
}
#if 0
/**
 ******************************************************************
 *
 * Function Name : Clean RawTracking
 *
 * Description : Clear out old  raw tracking data 
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
void Lassen::CleanTrackingPRN()
{
    SET_DEBUG_STACK;
    int i, rv, NCleared = 0;
    for (i=0;i<PRNCountB;i++)
    {
	if(pRawTracking[i].PRN != 0)
	{
	    // If more than 10 minutes without an update, 
	    // delete it. 
	    if(pRawTracking[i].DT() > 600.0)
	    {
		pRawTracking[i].Clear();
		NCleared++;
	    }
	}
    }
    if (NCleared>0)
    {
	PRNCountB -= NCleared;
	/* When done, compact the array */
	i = 0;
	do {
	    rv = CompactRawTracking();
	    i++;
	} while ((i<MAXPRNCOUNT) && (rv != 0));
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Compact RawTracking
 *
 * Description : Clear out old  raw tracking data 
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
int Lassen::CompactRawTracking()
{
    SET_DEBUG_STACK;
    int i, forward, backward, rc;
    RawTracking temp;
    forward  = -1;
    backward = -1;
    rc = 0;
    // look for first slot forward. 
    for (i=0;i<MAXPRNCOUNT;i++)
    {
	if ((pRawTracking[i].PRN == 0) && (forward ==-1))
	{
	    forward = i;
	    break;
	}
    }
    // Now backward
    for (i=MAXPRNCOUNT-1; i>=0 ;i--)
    {
	if (pRawTracking[i].PRN == 0)
	{
	    backward = i;
	    break;
	}
    }
    if (forward != backward)
    {
	(*errorLog) << "# " << __FILE__ << " " << __LINE__ 
		    << " Removing: " 
		    << (int) pRawTracking[forward].PRN << endl;
	pRawTracking[forward] = pRawTracking[backward];
	pRawTracking[backward].Clear();
    }
    else
    {
	rc = 1;
    }
    SET_DEBUG_STACK;
    return rc;
}
#endif
