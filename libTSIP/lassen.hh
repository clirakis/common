/**
 ******************************************************************
 *
 * Module Name : lassen.hh
 *
 * Author/Date : C.B. Lirakis / 02-Jun-02
 *
 * Description : Command codes and control information for Trimble Lassen
 * GPS receiver. Please note that the TSIP protocol is based on the 
 * TANS protocol! 
 *
 * 26-Oct-04 - CBL - After acquiring a Symmetricom (aka: Datum) 
 * Starloc II I have found that it supports TSIP protocol and that
 * the protocol is a bit more advanced than that of the Lassen SKII which
 * I orignally wrote this for. So I am modifying it slowly to the
 * TSIP Reference Volume 1, Revision C, April 1999. This will be where *
 * marked. 
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
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
 *******************************************************************
 */

#ifndef __LASSEN_h_
#define __LASSEN_h_

#include "precisetime.hh" // For time manipulations. 
#include "TimeStamp.hh"  // To mark when data is acquired. 
#include "Buffered.hh"
#include "TSIP_Constants.hh"
#include "SolutionStatus.hh"
#include "RawTracking.hh"
#include "TSIPosition.hh"
#include "TSIPVelocity.hh"
#include "GPSTime.hh"

/**
 * Command Packet 0x1D
 * This packet commands the GPS receiver to set or clear the 
 * oscillator offset in battery backed memory. This is normally 
 * used for servicing the unit. To clear the oscillator offset, 
 * one data byte is sent: the ASCII letter 'C' = 0x43.
 * To set the oscillator offset, four data bytes are sent: 
 * the oscillator offset in Hertz as a Single real value.
 */
#define CLEAR_OSCILLATOR_OFFSET  0x1D

/**
 * Command Packet 0x1E
 * This packet commands the GPS receiver to clear all battery back-up 
 * data and to perform a
 * software reset. This packet contains one data byte. 
 * 
 * Also supported by DATUM Starloc
 */
#define COMMAND_HARDWARE_RESET   0x1E

/**
 * Command Packet 0x1F
 * This packet requests information about the version of software 
 * running in the Navigation and Signal Processors. This packet 
 * contains no data. The GPS receiver returns Packet 0x45.
 * 
 * TSIP Reference Revision C table 2-6
 * Also supported by DATUM Starloc
 */
#define SOFTWARE_VERSION         0x1F

/**
 * Command Packet 0x21
 * This packet requests current GPS time. This packet contains no data. 
 * The GPS receiver returns Packet 0x41.
 * 
 * Not supported by starloc
 */
#define GET_TIME                 0x21

/**
 * Command Packet 0x23
 * This packet provides the GPS receiver with an approximate 
 * initial position in XYZ coordinates. This packet is useful 
 * if the user has moved more than a 1,000 miles after the previous fix. 
 * (Note that the GPS receiver can initialize itself without any data 
 * from the user; this packet merely reduces the time required for 
 * initialization.) This packet is ignored if the receiver is 
 * already calculating positions.
 *
 * Data  Format
 * Byte
 * 0-3  X Single Meters
 * 4-7  Y Single Meters
 * 8-11 Z Single Meters
 */
#define SET_POSITION_XYZ         0x23

/**
 * Command Packet 0x24
 * This packet requests current position fix mode of the GPS receiver. 
 * This packet contains no data. The GPS receiver returns Packet 0x6D.
 * 
 * Data returned in report packets 0x44 and 0x6D
 *
 * TSIP Reference Revision C 
 * Also supported by DATUM Starloc
 */
#define REQUEST_FIX_MODE         0x24

/**
 * Command Packet 0x25
 * This packet commands the GPS receiver to perform a software reset. 
 * This is equivalent to cycling the power. The GPS receiver performs 
 * a self-test as part of the reset operation.
 * This packet contains no data. Following completion of the reset, 
 * the receiver will output the start-up messages (see Table A-5). 
 * The GPS receiver sends Packet 0x45 only on power-up and reset 
 * (or on request); thus if Packet 0x45 appears unrequested, then either
 * the GPS receiver power was cycled or the GPS receiver was reset.
 * 
 * Also supported by DATUM Starloc
 */
#define COMMAND_SOFTWARE_RESET  0x25

/**
 * Command Packet 0x26
 * This packet requests health and status information from the 
 * GPS receiver. This packet contains no data. The GPS receiver 
 * returns packet0x 46 and 0x4B.
 */
#define GET_RX_HEALTH          0x26

/**
 * Command Packet 0x27
 * This packet requests signal levels for all satellites currently 
 * being tracked. This packet contains no data. 
 * The GPS receiver returns Packet 0x47.
 * 
 * Also supported by DATUM Starloc
 */
#define GET_RX_SIGNAL_LEVEL   0x27

/**
 * Command Packet 0x28
 * This packet requests the most recent GPS system ASCII 
 * message sent with the navigation data by each satellite. 
 * This packet contains no data. The GPS receiver returns Packet 0x48.
 *
 */
#define GET_ASCII_SYSTEM_MESSAGE   0x28

/** 
 * Doesn't appear to be supported for Lassen SKII
 * but is for DATUM Starloc
 */
#define GET_ALMANAC_HEALTH 0x29

/**
 * Command Packet 0x2A
 * ------------------------------------------------------------------
 * Note: This packet sets or requests the altitude parameters 
 * used for the Manual 2-D mode: Reference Altitude and Altitude Flag. 
 * Packet 0x4A (type 2) is returned.
 * ------------------------------------------------------------------
 * Reference Altitude is the altitude used for manual 2-D positions 
 * if the altitude flag is set. Altitude is in units of HAE WGS-84 
 * or MSL depending on the selected I/O options for the
 * position. The Altitude Flag determines whether or not the 
 * Reference Altitude will be used. If set, it will be used. 
 * If cleared, altitude hold (last 3-D altitude) is used.
 * ------------------------------------------------------------------
 * Note: With no data bytes, this packet requests the current values 
 * of these altitude parameters. In this case, the GPS receiver returns 
 * Packet 4A.
 * ------------------------------------------------------------------
 * Byte Item Type Meaning
 * 0-3 Altitude SINGLE Reference altitude for 2-D
 */
#define SET_ALTITUDE         0x2A

/**
 *  Command Packet 0x2B
 * This packet provides the GPS receiver with an approximate initial 
 * position in latitude and longitude coordinates (WGS-84). 
 * This packet is useful if the user has moved more than 1,000 miles 
 * after the previous fix. (Note that the GPS receiver can initialize 
 * itself without any data from the user; this packet merely reduces 
 * the time required for initialization.) This packet is ignored if 
 * the receiver is already calculating positions. To initialize with
 * ELEF position, use Command Packet 0x23. 
 * 
 * Byte Item Type Units
 * 0-3 Latitude Single Radians, north
 * 4-7 Longitude Single Radians, east
 * 8-11 Altitude Single Meters
 *
 */
#define SET_POSITION_LLA     0x2B

/**
 * Command Packet 0x2D
 * This packet requests the calculated offset of the GPS receiver
 * master oscillator. This packet contains no data. The GPS receiver 
 * returns Packet 0x4D. This packet is used mainly for service. 
 * The permissible oscillator offset varies with the particular GPS
 * receiver unit.
 */
#define GET_OSCILLATOR_OFFSET 0x2D

/**
 * Command Packet 0x2E
 * This packet provides the approximate GPS time of week and the 
 * week number to the GPS receiver. The GPS receiver returns Packet 0x4E. 
 * The data format is shown below. The GPS week number reference is 
 * Week # 0 starting January 6, 1980. The seconds count
 * begins at the midnight which begins each Sunday morning. 
 * This packet is usually not required when the battery back-up 
 * voltage is applied as the internal clock keeps time to
 * sufficient accuracy. This packet is ignored if the receiver 
 * has already calculated the time from tracking a GPS satellite.
 * 
 * Byte Item Type Units
 * 0-3 GPS time of week Single Seconds
 * 4-5 Extended GPS week number Integer Weeks
 */
#define SET_TIME_OF_WEEK      0x2E

/**
 * Command Packet 0x31
 * This packet is identical in content to Packet 0x23. This packet 
 * provides an initial position to the GPS receiver in XYZ coordinates. 
 * However, the GPS receiver assumes the position provided in this packet 
 * to be accurate. This packet is used for satellite acquisition aiding in
 * systems where another source of position is available and in time 
 * transfer (one-satellite mode) applications. For acquisition aiding,
 * the position provided by the user to the GPS receiver in this packet
 * should be accurate to a few kilometers. For high-accuracy time
 * transfer, position should be accurate to a few meters.
 * 
 * Also supported by DATUM Starloc
 */
#define SET_POSITION_XYZ_ACCURATE 0x31

/**
 * Command Packet 0x32
 * This packet is identical in content to Packet 0x2B. This packet 
 * provides the GPS receiver with an approximate initial position in 
 * latitude, longitude, and altitude coordinates. However, the GPS 
 * receiver assumes the position provided in this packet to be accurate.
 * This packet is used for satellite acquisition aiding in systems where 
 * another source of position is available and in time transfer 
 * (one-satellite mode) applications. For acquisition aiding, the 
 * position provided by the user to the GPS receiver in this packet should be
 * accurate to a few kilometers. For high-accuracy time transfer, 
 * position should be accurate to a few meters.
 * 
 * Also supported by DATUM Starloc
 */
#define SET_POSITION_LLA_ACCURATE 0x32

/**
 * Command Packet 0x35
 * This packet requests the current I/O option states and optionally 
 * allows the I/O option states to be set as desired. To request the 
 * option states without changing them, the user sends the packet with no data
 * bytes included. To change any option states, the user includes 4 
 * data bytes with the values. The I/O options, their default states, 
 * and the byte values for all possible states are shown below. 
 * These option states are held in battery-backed memory and can be 
 * set into nonvolatile RAM (EEPROM) with the 0x8E-26 command. The GPS 
 * receiver returns Packet 0x55. 
 *
 * These abbreviations apply to the following table: 
 *       ALT (Altitude)
 *       ECEF (Earth-centered,Earth-fixed)
 *       XYZ (Cartesian coordinates)
 *       LLA (latitude, longitude, altitude)
 *       HAE (height above ellipsoid)
 *       WGS-84 (Earth model (ellipsoid))
 *       MSL geoid (mean sea level)
 *       UTC (coordinated universal time)
 *
 * 
 * Also supported by DATUM Starloc
 * TSIP Reference Revision C table 2-17 through 2-21
 */
#define SET_IO_OPTION_STATE    0x35

/**
 * Command Packet 0x37
 * This packet requests information regarding the last position fix
 * and is only used when the receiver is not automatically 
 * outputting positions. The GPS receiver returns the position/
 * velocity auto packets specified in the 0x35 message as well as message 0x57.
 * 
 * NOT supported by DATUM Starloc
 */
#define GET_LAST_FIX           0x37

/**
 * Command Packet 0x38
 * This packet requests current satellite data (almanac, ephemeris, etc.) 
 * or permits loading initialization data from an external source 
 * (for example, by extracting initialization data from an operating
 * GPS receiver unit via a data logger or computer and then using that data
 * to initialize a second GPS receiver unit). The GPS receiver returns 
 * Packet 0x58. (Note that the GPS receiver can initialize itself without 
 * any data from the user; it merely requires more time.)
 * To request data without loading data, use only bytes 0 through 2; to 
 * load data, use all bytes. Before loading data, observe the caution 
 * notice below. The data formats are located in Report Packet 0 x 5B.
 *
 * See Table A-22 for details
 * 
 * Also supported by DATUM Starloc
 */
#define GET_OR_SET_ALM  0x38

/**
 * Command Packet 0x39
 * Normally the GPS receiver selects only healthy satellites 
 * (based on transmitted values in the ephemeris and almanac) 
 * which satisfy all mask values, for use in the position solution.
 * This packet allows you to override the internal logic and force 
 * the receiver to either unconditionally disable a particular satellite 
 * or to ignore a bad health flag. The GPS receiver returns Packet 0x59 
 * for operation modes 3 and 6 only. It should be noted that when viewing 
 * the satellite disables list, the satellites are not
 * numbered but are in numerical order. The disabled satellites are 
 * signified by a 1 and enabled satellites are signified by a 0.
 * 
 * Also supported by DATUM Starloc
 */
#define SELECT_SATELLITE  0x39

/**
 * Command Packet 0x3C
 *  This packet requests the current satellite tracking status. The 
 * GPS receiver returns Packet 0x5C if data is available.
 * 
 * TSIP Reference Manual Revision C. 
 * Also supported by DATUM Starloc
 * Supported by Lassen SK II
 */
#define GET_TRACKING_STATUS 0x3C
/**
 * Report Packet 0x40 reports the almanac data for a single satellite. 
 * The receiver sends this packet on request (Command Packet 0x20) and 
 * when data is received from a satellite.
 */
#define ALMANAC_FOR_SINGLE  0x40
/**
 * Report Packet 0x41
 * This packet provides the current GPS time of week and the week number. 
 * The GPS receiver sends this packet in response to Packet 0x21 and 
 * during an update cycle. Update cycles occur approximately every 5 
 * seconds. The data format is shown in Table A-25.
 */
#define GPS_TIME_OF_WEEK_REPLY 0x41

/**
 * Report Packet 0x42
 * This packet provides current GPS position fix in XYZ ECEF coordinates. 
 * If the following setup:
 *      I/O position option is set to XYZ ECEF AND
 *      I/O precision-of-position output is set to single-precision
 * then the GPS receiver sends this packet each time a fix is computed.
 * The time-of-fix is in GPS time or UTC as selected by the I/O timing option.
 * Packet 83 provides a double-precision version of this information.
 * Byte Item Type Units
 * 0-3 X SINGLE meters
 * 4-7 Y SINGLE meters
 * 8-11 Z SINGLE meters
 * 12-15 time-of-fix SINGLE seconds
 */
#define ECEF_XYZ_REPLY    0x42

/**
 * Report Packet 0x43
 * This packet provides current GPS velocity fix in XYZ ECEF coordinates. 
 * If 
 *     I/O velocity option is set to XYZ ECEF
 * then the GPS receiver sends this packet each time a fix is computed. 
 * The data format is shown below.
 * Byte Item Type Value
 * 0-3 X velocity SINGLE meters/second
 * 4-7 Y velocity SINGLE meters/second
 * 8-11 Z velocity SINGLE meters/second
 * 12-15 bias rate SINGLE meters/second
 * 16-19 time-of-fix SINGLE seconds
 */
#define ECEF_VELOCITY_REPLY  0x43

/**
 * Report Packet 0x44
 * 
 *  
 * Byte Item Type Value
 * __________________________________
 * 0     Mode Byte  
 * 1-4   SV Numbers 4 bytes
 * 5-8   PDOP SINGLE
 * 9-12  HDOP SINGLE
 * 13-16 VDOP SINGLE
 * 17-20 TDOP SINGLE
 *
 * TSIP Reference Revision C table 3-10
 * Not supported by Lassen SK-II
 * Not supported by Starloc
 */
#define SATELLITE_SELECTION_REPORT 0x44

/**
 * Report Packet 0x45
 * This packet provides information about the version of software in the
 * Navigation and Signal Processors. The GPS receiver sends this packet 
 * after power-on and in response to Packet 0x1F.
 * Byte Item Type
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
 * TSIP Reference Revision C table 3-11
 * Also supported by Datum Starloc 
 *
 */
#define SOFTWARE_REVISION_REPLY 0x45

/**
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
 */
#define SATELLITE_TRACKING_REPLY 0x46

/**
 * Report Packet 0x47
 * This packet provides received signal levels for all satellites 
 * currently being tracked or on which tracking is being attempted 
 * (i.e., above the elevation mask and healthy according toReport Packet 0x47
 * the almanac). The receiver sends this packet only in response to 
 * Packet 0x27. The data format is shown below. Up to 8 satellite 
 * number/signal level pairs may be sent, indicated by the count field. Signal
 * level is normally positive. If it is zero then that satellite 
 * has not yet been acquired. If it is negative then that satellite 
 * is not currently in lock. The absolute value of signal level field
 * is the last known signal level of that satellite. The signal level 
 * provided in this packet is a linear measure of the signal strength 
 * after correlation or de-spreading. Units, either AMU or dBHz, are 
 * controlled by Packet 0x35.
 *
 * Byte Item Type
 *    0 Count BYTE
 *    1 Satellite number 1 BYTE
 *    2- 5 Signal level 1 SINGLE
 *    6 Satellite number 2 BYTE
 *    7-10 Signal level 2 SINGLE
 *                       (etc.) (etc.) (etc.)
 */
#define SIGNAL_LEVEL_REPLY  0x47

/**
 * Report Packet 0x48
 * This packet provides the most recent 22-byte ASCII message broadcast 
 * in the GPS satellite navigation message. The receiver sends this packet 
 * in response to Packet 0x28. The message effectively is a bulletin board 
 * from the Air Force to GPS users. The format is free-form ASCII and is 
 * often enabled or encrypted. The message may be blank.
 */
#define ASCII_NAV_REPLY   0x48

/**
 * Report Packet 0x4A
 * This packet provides current GPS position fix in LLA 
 *    (latitude, longitude, and altitude)
 * coordinates. If: 
 *     position option is set to LLA AND 
 *     I/O precision-of-position output is set to single-precision
 * then the receiver sends this packet each time a fix is computed.
 * The LLA conversion is done according to the datum selected using 
 * Packet 0x8E-15. The default is WGS-84. Altitude is referred to the 
 * datum ellipsoid or the MSL Geoid, depending on which I/O LLA altitude
 * option is selected. The time-of-fix is in GPS time or UTC, depending 
 * on which I/O timing option is selected. This packet also is sent at 
 * start-up with a negative time-of-fix to report the current known
 * position. Packet 0x84 provides a double-precision version of 
 * this information
 * Byte Item Type Units
 *    0-3 Latitude SINGLE radians; + for north, - for south
 *    4-7 Longitude SINGLE radians; + for east, - for west
 *    8-11 Altitude SINGLE meters (HAE or MSL)
 *    2-15 Clock Bias SINGLE meters
 *    6-19 Time-of-Fix SINGLE seconds (GPS or UTZ)
 *
 * Report Packet 0x4A is also sent in response to the setting or requesting 
 * of the Reference Altitude Parameters using Command Packet 0x2A. These 
 * parameters can be used in the Manual 2-D mode.
 * Reference Altitude
 * The altitude used for manual 2-D positions if the altitude flag is set. 
 * Altitude is in units of
 * HAE WGS-84 or MSL depending on the selected I/O options for the position.
 *
 * Altitude Flag
 * A flag that determines whether or not the Reference Altitude will be used.
 * If set, it will be used. If cleared, altitude hold (last 3-D altitude) 
 * will be used. The data format is shown in the following table.
 *
 * Byte Item Type Units
 *    0-3 Reference Altitude SINGLE Meters
 *    4-7 Reserved SINGLE
 *    8 Altitude flag BYTE
 */
#define LLA_SINGLE_PRECISION_REPLY 0x4A

/**
 * Report Packet 0x4B
 * The receiver transmits this packet in response to packets 0x25 and 0x26 
 * and following a change in state. In conjunction with Packet 0x46, 
 * health of receiver, this packet identifies the receiver and may 
 * present status messages. The machine ID can be used by equipment 
 * communicating with the receiver to determine the type of receiver to 
 * which the equipment is connected. Then the interpretation and use of 
 * packets can be adjusted accordingly.
 * 
 * The status codes are encoded into individual bits within the bytes. 
 * The bit positions and their meanings are shown in Table A-36.
 * Table A-35. Report Packet 0x4B Data Formats
 *    Byte Item Type/Value Status/Meaning
 *        0 Machine ID BYTE 6-channel receiver
 *        1 Status 1 BYTE see Table A-36
 *        2 Status 2 BYTE Bit 0 = Super packets supported
 * 
 * Table A-36. Report Packet 0x4B Bit Positions and Descriptions Status 1 Bit
 *    Position Meaning if bit value = 1
 *        0 (LSB) not used
 *        1 not used
 *        2 not used
 *        3 The almanac stored in the receiver, is not complete and current
 *        4-7 not used
 */
#define HEALTH_REPLY 0x4B

/**
 * Report Packet 0x4D
 * This packet provides the current value of the receiver master oscillator 
 * offset in Hertz at carrier. This packet contains one SINGLE number. The 
 * receiver sends this packet in response to Packet 0x2D. The permissible 
 * offset varies with the receiver unit.
 */
#define MASTER_OSCILLATOR_OFFSET_REPLY 0x4D

/**
 * Report Packet 0x4E
 * Indicates whether the receiver accepted the time given in a Set GPS 
 * time packet. the receiver sends this packet in response to Packet 0x2E. 
 * This packet contains one byte.
 * Table A-37. Report Packet 0x4E Data Formats
 *    Value Meaning
 *        ASCII Y The receiver accepts the time entered via Packet 2E. 
 *                The receiver has not yet received the time from a satellite.
 *
 *        ASCII N The receiver does not accept the time entered via Packet 2E.
 *                The receiver has received the time from a satellite 
 *                and uses that time. The receiver disregards the time 
 *                in Packet 0x 2E.
 */
#define TIME_ACCEPTED_REPLY 0x4E

/**
 * Report Packet 0x55
 * This packet requests the current I/O option states and optionally allows 
 * the I/O option states to be set as desired. These abbreviations apply to 
 * the following table: 
 *     ALT (Altitude)
 *     ECEF (Earth-centered, Earth-fixed) 
 *     XYZ (Cartesian coordinates) 
 *     LLA (latitude, longitude, altitude)
 *     HAE (height above ellipsoid)
 *     WGS-84 (Earth model (ellipsoid)
 *     MSL geoid (Earth (mean sea level) mode)
 *     UTC (coordinated universal time).
 *
 * Table A-38. Command Packets 0x55 and 0x35 Data Descriptions
 * Byte Parameter  Bit       Default    Option             Associated
 *      Name       Position  Bit Value                     Packet
 * ====================================================================
 *  0   position   0 (LSB)   1          XYZ ECEF Output   0x42 or 0x83
 *                                      0: off
 *                                      1: on
 * 
 *                 1         0          LLA Output        0x4A or 0x84
 *                                      0: off
 *                                      1: on
 *
 *                 2         0          LLA ALT Output    0x4A or 0x84 
 *                                      0: HAE (datum)    0x8F-17 0x8F-18
 *                                      1: MSL geoid
 *
 *                 3         0          ALT input         0x2A
 *                                      0: HAE (datum)
 *                                      1: MSL geoid
 *
 *                 4         0         Precision-of-position  0x42/4A/8F-17
 *                                                            0x83/84/8F-18 
 *                                     0: Send single-precision packet
 *                                     1: Send double precision packet
 *                 5         0         Automatic output of Super Packet 8F-20
 *                                     (This is not supported on Lassen SK-II)
 *                                     0 - Off
 *                                     1 - On
 *                 6-7       0         not used
 * ------------------------------------------------------------------------
 * 1    velocity   0         1         XYZ ECEF Output       0x43
 *                                     0: off
 *                                     1: on
 *                 1         0         ENU Output            0x56
 *                                     0: off
 *                                     1: on
 *
 *                 2-7       0         not used
 * ------------------------------------------------------------------------
 * 2    timing     0         0         time type             0x42, 0x43,
 *                                     0: GPS time           0x4A, 0x83,
 *                                     1: UTC                0x84, 0x56,
 *                                                           0x8F-17,0x8F-18
 *                 1         0         Fix Computation Time 
 *                                     0: ASAP, 1: At integer second
 *
 *                 2         0         Automaic Output of Fix Time (0x37)
 *                                     0: When Computed, 1: On Request
 *
 *                 3         0         Simultaneous Measurements
 *                                     (Not SK-II supported) 
 *                                     0: Off, 1: On
 *
 *                 4         0         Minimum Projection 0:Off, 1:On
 *                 5-7       0         not used
 * ------------------------------------------------------------------------
 * 3    Auxiliary  0         0: off    Measurement Output, 0:Off 1:On
 *      PR meas.   1         0: raw    Codephase Measurement Type
 *                                     0: raw PR's in 5A
 *                                     1: filtered PR's in 5A
 *                 2                   Automatic output of Additional Fix 
 *                                     Status, 0: Off, 1:On
 *
 *                 3         0: off    Units dBHz instead of AMU 0x5A, 0x5C,
 *                           1: on                                0x47,
 *                                                                0x6F
 *                 4-7 reserved
 * Note: See the associated superpacket output, described later in 
 *       this appendix. Packet 8E must be used to specify which superpacket 
 *       is to be output.
 *
 * Note: Automatic output of 5 A messages is supported in the Lassen-SK8 
 *       for backwards compatibility with older TSIP applications.
 * TSIP Reference Revision C table 3-28 through 3-32
 */
#define IO_OPTION_REPLY  0x55

/**
 * Report Packet 0x56
 * If East-North-Up (ENU) coordinates have been selected for the I/O 
 * velocity option, the receiver sends this packet under the following 
 * conditions:
 *    Each time that a fix is computed
 *    In response to Packet 0x37 (last known fix)
 * The data format is shown below.
 * The time-of-fix is in GPS or UTC time as selected by the I/O timing option.
 * Table A-39. Report Packet 0x56 Data Formats
 * Byte Item Type Units
 *    0-3 East Velocity SINGLE m/s; + for east, - for west
 *    4-7 North Velocity SINGLE m/s; + for north, - for south
 *    8-11 Up Velocity SINGLE m/s; + for up, - for down
 *    12-15 Clock Bias Rate SINGLE m/s
 *    16-19 Time-of-Fix SINGLE seconds (GPS or UTC)
 */
#define VELOCITY_ENU_REPLY 0x56

/**
 * Report Packet 0x57
 * This packet provides information concerning the time and origin 
 * of the previous position fix. The receiver sends this packet, 
 * among others, in response to Packet 0x37. The data format is shown below.
 * Table A-40. Report Packet 0x57 Data Formats
 * Byte Item Type/Units Byte 0 Value/Velocity
 *    0 Source of information  BYTE/ - - - 00/none
 *                                         01/regular fix
 *    1 Mfg. diagnostic BYTE/ - - -
 *    2-5 Time of last fix SINGLE/seconds, GPS time
 *    6-7 Week of last fix INTEGER/weeks, GPS time
 */
#define TIME_OF_LAST_FIX_REPLY 0x57

/**
 * Report Packet 0x58
 * This packet provides GPS data (almanac, ephemeris, etc.). The receiver 
 * sends this packet under the following conditions:
 *     On request
 *     In response to Packet 0x38 (acknowledges the loading of data)
 * The data format is shown below.
 * The binary almanac, health page, and UTC data streams are similar to 
 * Report Packets 0x40, 0x49, and 0x4F respectively, and those reports are 
 * preferred. To get ionosphere or ephemeris, this report must be used.
 * Table A-41. Report Packet 0x58 Data Formats
 * Byte Item Type Value Meaning
 * 0 Operation BYTE      0       Data type cannot be loaded  Acknowledge
 *                       1       Data Out
 *                       2
 * ----------------------------------------------------------------------
 * 1 Type of data   BYTE 2       Almanac
 *                       3       Health page, T_oa, IONO
 *                       4       UTC
 *                       5       Ephemeris
 *                       6
 * ----------------------------------------------------------------------
 * 2 Sat PRN #      BYTE 0       Data that is not satellite ID-specific
 *                       1 to 32 Satellite PRN number
 * ----------------------------------------------------------------------
 * 3 length (n) BYTE Number of bytes of data to be loaded
 * ----------------------------------------------------------------------
 * 4 to n+3 data n BYTES
 *
 * See page A-43  for a great may more  details on the decoding of the 
 * data contained within. 
 */
#define GPS_DATA_REPLY  0x58

/**
 * Report Packet 0x59
 * Normally the GPS receiver selects only healthy satellites (based on 
 * transmitted values in the ephemeris and almanac) which satisfy all mask 
 * values, for use in the position solution. This packet allows you to 
 * override the internal logic and force the receiver to either
 * unconditionally disable a particular satellite or to ignore a bad health 
 * flag. The GPS receiver returns Packet 0x59 for operation modes 3 and 6 only.
 * The data format is shown below.
 * This information is not held in battery-backed memory. At power-on and 
 * after a reset, the default values are set for all satellites.
 * Table A-47. Report Packet 0x59 Data Formats
 * Byte Item Type Value Meaning
 * 0 Operation BYTE 3 The remaining bytes tell whether receiver
 *                    is allowed to select each satellite.
 *                  6 The remaining bytes tell whether the
 *                    receiver heeds or ignores each satellite's
 *                    health as a criterion for selection.
 * 1 to 32 Satellite # 32 BYTES   (Depends on byte 0 value.)
 *                    (1 byte per
 *                     satellite)
 *
 *                  0 Enable satellite selection or heed
 *                    satellite's health. Default value.
 *                  1 Disable satellite selection or ignore
 *                    satellite's health.
 */
#define SATELLITE_DISABLE_REPLY 0x59

/**
 *  Report Packet 0x5A
 * This packet provides raw GPS measurement data. If the I/O auxiliary 
 * option has been selected, the receiver sends this data automatically 
 * as measurements are taken. The data format is shown below.
 *
 * Note: A new Report Packet, 0x6F has full pseudo-ranges and integrated 
 *       doppler.
 * Application Note: Packet 0x5A provides the raw satellite signal measurement
 *                   information used in computing a fix.
 * Table A-48. Report Packet 0x5A Data Formats
 * Byte Item Type Units
 * 0 Satellite PRN Number BYTE -----
 * 1 Sample Length SINGLE msec
 * 5 Signal Level SINGLE AMU or dBHz
 * 9 Code phase SINGLE 1/16th chip
 * 13 Doppler SINGLE Hertz
 * 17 Time of Measurement DOUBLE sec
 * More  detail is given in A.17.40.....
 */
#define RAW_DATA_REPLY 0x5A

/**
 * Report Packet 0x5C
 * This packet provides tracking status data for a specified satellite. 
 * Some of the information is very implementation-dependent and is provided 
 * mainly for diagnostic purposes. The receiver sends this packet in 
 * response to Packet 0x3C. 
 * ----------------------------------------------------------------------
 * Table A-49. Report Packet 0x5C Data Formats
 * Byte/Item Type/Units Value/Meaning
 * ----------------------------------------------------------------------
 * Byte 0 - Satellite PRN  BYTE/number
 *          number         1-32
 * ----------------------------------------------------------------------
 * Byte 1 - Channel code BYTE Bit 4-6, channel number, 0-7
 *                            Bit position within byte 1 7(MSB) 3
 *                            (channel number beginning with 0)
 * ----------------------------------------------------------------------
 * Byte 2/Acquisition flag BYTE Byte 2 value:
 *                                   0 never acquired
 *                                   1 acquired
 *                                   2 re-opened search
 * ----------------------------------------------------------------------
 * Byte 3/Ephemeris flag BYTE Byte 3 value:
 *                                   0 flag not set
 *                                     good ephemeris for this satellite
 *                                     (<4 hours old, good health)
 * ----------------------------------------------------------------------
 * Byte 4 - 7/Signal level SINGLE same as in Packet 0x47
 * ----------------------------------------------------------------------
 * Byte 8 - 11/GPS time of     SINGLE/seconds Byte 8 - 11 value:
 *           last measurement
 *                            <0 no measurements have been taken
 * ----------------------------------------------------------------------
 * Byte 12 - 15/Elevation SINGLE/radians Approximate elevation of this 
 *                            satellite above the horizon. Updated about
 *                            every 15 seconds. Used for searching
 *                            and computing measurement correction factors.
 * ----------------------------------------------------------------------
 * Byte 16 - 19/Azimuth SINGLE/radians Approximate azimuth from true north
 *                                     to this satellite. Updated typically 
 *                                     about every 3 to 5 minutes. Used for
 *                                     computing measurement correction
 *                                     factors.
 * ----------------------------------------------------------------------
 * Byte 20/old         BYTE Byte 20 value:
 *    measurement flag
 *                               0 flag not set
 *                              >0 the last measurement is too old to
 *                                 use for a fix computation.
 * ----------------------------------------------------------------------
 * Byte 21/Integer msec  BYTE Byte 21 value:
 * flag
 *                       0 Don't have good knowledge of integer
 *                       millisecond range to this satellite
 *                       1 msec from sub-frame data collection
 *                       2 verified by a bit crossing time
 *                       3 verified by a successful position fix
 *                       4 suspected msec error
 *                       Trimble Standard Interface Protocol
 * ----------------------------------------------------------------------
 * Byte 22/bad data flag BYTE Byte 22 value:
 *                                 0 flag not set
 *                                 1 bad parity
 *                                 2 bad ephemeris health
 * ----------------------------------------------------------------------
 * Byte 23/Data collection    BYTE Byte 23 value:
 * flag
 *                            0 flag not set
 *                           >0 The receiver currently is trying to
 *                              collect data from this satellite.
 *
 * TSIP Reference Manual Revision C
 * Supported by Lassen SK II
 * Supported by Datum Starloc II
 *
 */
#define RAW_TRACKING_REPLY 0x5C
struct PackedAzEl {
    char  PRN;                           // range 0-32
    /* 
     * Azimuth ranges from 0-360 and Elevation ranges from 0-90 
     * 0.1 degree is 10^4 for radians. 16 bits is 65535
     * Pack in (Azimuth*10^4)<<16  + Elevation*10^4
     */
    unsigned int AzEl;
};
/**
 * Store the satellite PRN and associated signal level. 
 * used as an array in Raw Tracking. 
 */
struct PackedSignal {
    char PRN;
    float SignalLevel;
};


/**
 * Command Packet 0x60 -Type 1 Differential GPS Corrections
 * This packet provides the SVeeSix with differential corrections 
 * from RTCM SC-104 record types 1 and 9, in the TSIP format. There 
 * is no response to this packet. The units and scale factors are 
 * as defined by RTCM-104 version 1. If byte 3 bit 7 is set, the unit and
 * scale factors are defined by RTCM SC-104 version 2. If bit 6 is set,
 *  the corrections are as in RTCM Type 9 records. The format for this 
 * packet is shown in Table A-50
 * See  manual  for details!
 */
#define SEND_DIFFERENTIAL_CORRECTIONS_TYPE1 0x60

/**
 * Command Packet 0x61 -Set Differential GPS Corrections
 * This TSIP packet provides the delta differential corrections from 
 * RTCM-104 record type 2. There is no response to this packet. Scale 
 * factors are version 1 unless the version 2 flag is set. The format 
 * for this packet is shown in Table A-52. The units and scale factors
 * are as defined by Packet 0x60. Delta range correction rates are
 * not entered.
 * Table A-52. Command Packet 0x61 Data Formats
 * Byte Item Type Units
 * 0 - 1 Modified Z-count WORD .6 SEC
 * 2     (set MSB for version 2) Bit 7 = 1
 *                               Bit 0-6 = number of SVs      BYTE
 * The next 3 bytes are repeated as a group for each satellite:
 *     3+ (N*3) SV PRN & scale factor BYTE
 *     4+ (N*3) Delta range correction WORD RTCM-104
 */
#define SEND_DIFFERENTIAL_CORRECTIONS_TYPE2 0x61

/**
 * Command Packet 0x62
 * This packet requests the differential position fix mode of the GPS 
 * receiver. A single data byte is sent. To request Report Packet 0x82, 
 * the data byte is set to contain any value between 0x5 and 0xFF.
 */
#define REQUEST_DIFFERENTIAL_FIX 0x62

/**
 * Command Packet 0x65
 * This packet requests the status of differential corrections for a 
 * specific satellite or for all satellites for which data is available. 
 * This packet contains only one byte specifying the PRN number of the 
 * desired satellite or zero to request all available. The response is a
 * Packet 0x85 for each satellite if data is available. If the receiver 
 * has no valid data for any satellite, no reply will be sent.
 */
#define REQUEST_SPECIFIC_DIFFERENTIAL_CORR 0x65

/**
 * Report Packet 0x6D
 * This packet provides a list of satellites used for position fixes by 
 * the GPS receiver. The packet also provides the PDOP, HDOP, and VDOP 
 * of that set and provides the current mode (automatic or manual, 3-D or 2-D).
 * This packet has variable length equal to 16+nsvs where nsvs is the 
 * number of satellites used in the solution. The GPS receiver sends 
 * this packet in response to Packet 0x24 when the receiver is doing an 
 * overdetermined fix or every 5 seconds. The data format is shown in 
 * Table A-53.
 *
 * TSIP Reference Revision C table 3-58
 * Supported by Lassen SKII
 * Supported by Starloc
 */
#define FIX_DATA_REPLY 0x6D

/**
 * Command Packet 0x6E 
 * Set or Request Synchronized Measurement Parameters
 * Packet 6E sets or requests the Synchronized Measurement parameters. 
 * The synchronized measurement parameters are sent by the GPS receiver 
 * in Packet 0x6F Enable / Disable Synchronized Measurements Controls 
 * whether synchronized measurements will be output at the output interval
 */
#define REQUEST_SYNCHRONIZED_MEASUREMENTS 0x6E

/**
 * Report Packet 0x6E 
 * Synchronized Measurements
 * Report Packet 0x6E reports the setting of Synchronized Measurement 
 * parameters. The values are shown in Table A-56. See Command Packet 
 * 0x6E for more information.
 */
#define SYNCHRONIZED_MEASUREMENTS_REPLY 0x6E
#define SYNCHRONIZED_DATA_REPLY 0x6F

/**
 * Command Packet 0x70
 * Trimble OEM receivers have a number of filters. Command 0x70 provides 
 * control for these filters. It returns Report 0x70. There are three filters
 * associated with 0x70:
 *     Position-Velocity (PV) Filter
 *     Static Filter
 *     Altitude Filter
 * The Position-Velocity (PV) Filter is the main filter and is used to 
 * "soften" the effect of constellation switches on position fixes. The 
 * filter has no effect on velocity output and there is no lag due to 
 * vehicle dynamics. There may be a small increase in accuracy however.
 *
 *  0 PV Filter BYTE 0 - Off      Requests filter setting
 *                   1 - On
 *  1 Static Filter BYTE 0 - Off
 *                       1 - On
 *  2 Altitude Filter BYTE 0 - Off
 *                         1 - On
 *  3 Reserved BYTE
 * 
 * Also supported by DATUM Starloc
 */
#define REQUEST_FILTER 0x70

/**
 * Command Packet 0x7A
 * The NMEA message mask is a 32-bit vector which determines whether or 
 * not a given NMEA message will be output. If the bit for a message is 
 * set, the message will be sent every "interval" seconds.
 * Hex values are ORed together to produce the desired combined 
 * output mask. For example, a mask value of 0x00000005 would mean 
 * GGA and VTG messages are enabled for output (the default mask), 
 * and a mask value of 0x00000013F would mean all of the messages are 
 * enabled for output. The Hex values used to request the NMEA interval and
 * message mask are listed below.
 */
#define GGA 0x00000001
#define GLL 0x00000002
#define VTG 0x00000004
#define GSV 0x00000008
#define GSA 0x00000010
#define ZDA 0x00000020
#define RMC 0x00000100

/**
 * Format:
 * 0 Subcode  BYTE      0
 * 1 Interval BYTE      The time in seconds between NMEA messages 
 *                      (position fix rate if 0)
 * 2-5 Output UNSIGNED  The NMEA bit-mask for outputting messages
 *     mask   LONG INT
 */
#define SET_NMEA_OUTPUT 0x7A

/**
 * Report Packet 0x7B
 * Report Packet 0x7B has one form. See Command Packet 0x7A for more 
 * information about the data formats.
 * To set the NMEA interval and message mask, use the values shown in 
 * Table A-62. 
 * Byte Item Type Value Meaning
 *     0 Subcode  BYTE      0
 *     1 Interval BYTE      The time in seconds between NMEA messages
 *     2-5 Output UNSIGNED  The NMEA bit-mask for outputting messages  
 *         mask   LONG INT
 */
#define  NEMA_OUTPUT_REPLY 0x7B

/**
 * Report Packet 0x82
 * This packet provides the differential position fix mode of the receiver. 
 * This packet contains only one data byte to specify the mode. The packet 
 * is sent in response to Packet 0x62 and whenever a satellite selection 
 * is made and the mode is Auto GPS/GPD (modes 2 and 3). The receiver 
 * switches automatically between modes 2 and 3 based on the availability 
 * of differential corrections for a constellation which meets all other 
 * masks. If such a constellation is not available, then the receiver stays 
 * in its current automatic mode (2 or 3), and does not do position solutions.
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
 * 22-Dec-15
 * Apparently has other stuff as well. Differential Position fix mode. 
 * The TSIP documentation shows this as 4 bytes, I am getting 6 bytes 
 * of info
 */
#define MODE_REPLY 0x82

/**
 * Report Packet 0x83
 * This packet provides current GPS position fix in XYZ ECEF coordinates. 
 * If the I/O position option is set to XYZ ECEF AND the I/O double position 
 * option is selected, the receiver sends this packet each time a fix is 
 * computed. The data format is shown in Table A-63.
 * The time-of-fix is in GPS time or UTC, as selected by the I/O timing option.
 * Packet 42 provides a single-precision version of this information.
 * Byte Item Type Units
 * 0-7 X DOUBLE meters
 * 8-15 Y DOUBLE meters
 * 16-23 Z DOUBLE meters
 * 24-31 clock bias DOUBLE meters
 * 32-35 time-of-fix SINGLE seconds
 * 
 * Also supported by DATUM Starloc
 */
#define ECEF_XYZ_DOUBLE_REPLY 0x83

/**
 * Report Packet 0x84
 * This packet provides current GPS position fix in LLA coordinates. If 
 * the I/O position option is set to LLA and the double position option 
 * is selected, the receiver sends this packet each time a fix is computed. 
 * The data format is shown in Table A-64.
 * The time-of-fix is in GPS time or UTC, as selected by the I/O timing option.
 * 
 * Byte Item Type Units
 * 0-7 latitude DOUBLE radians; + for north, - for south
 * 8-15 longitude DOUBLE radians; + for east, - for west
 * 16-23 altitude DOUBLE meters
 * 24-31 clock bias DOUBLE meters
 * 32-35 time-of-fix SINGLE seconds
 * 
 * Also supported by DATUM Starloc
 */
#define LLA_DOUBLE_REPLY 0x84

/**
 * Report Packet 0x85
 * This packet provides the status of differential corrections for a 
 * specific satellite. It is sent in response to Packet 0x65. The format 
 * of this packet is shown in Table A-65. The summary status code is encoded 
 * in Table A-66.
 * 
 * Report 85    Item                     Type   Units
 * Byte 
 * 0            Satellite PRN number     BYTE
 * 1            Summary status code      BYTE
 * 2            Station health           BYTE
 * 3            Satellite health (UDRE)  BYTE
 * 4            IODE 1                   BYTE
 * 5            IODE 2                   BYTE
 * 6            Z-count as Time-of-Week  SINGLE seconds
 * 10           Range correction         SINGLE meters
 * 14           Range-rate correction    SINGLE m/sec
 * 18           Delta range correction   SINGLE meters
 *
 * --------------------------------------------------------------------
 * 
 * Table A-66. Report Packet 0x85 Summary Status Code Encoding
 * 0 good correction data
 * 1 good delta correction data
 * 2 station health bad (5 or 7)
 * 3 data too old (60 seconds)
 * 4 UDRE too high (>4)
 * 5 IODE mismatch with ephemeris
 * 6 satellite not in current Type1 message
 */
#define DIFFERENTIAL_REPLY 0x85

/**
 * Command Packet 0xBB
 * In query mode, Packet 0xBB is sent with a single data byte and returns
 * Report Packet 0xBB. TSIP Packet 0xBB is used to set GPS Processing options.
 * The table below lists the individual fields within the 0xBB Packet. 
 * See A.3 for information on saving the settings to non-volatile memory.
 * Table A-67. Command Packet 0xBB Query Mode Data Format
 * Byte
 *  # Item Type Value Meaning Default
 *  0 Subcode BYTE 0 x 03 Query mode
 *
 * -----------------------------------------------------------
 * Table A-68. Command and Report Packet 0xBB Field Descriptions
 * Byte # Item        Type     Value    Meaning          Default
 * 0      Subcode     BYTE     0x03                      0x03
 * 1      Operating   BYTE     0        Automatic(2D/3D) Automatic
 *        Dimension            3        Horizontal (2D)  
 *                             4        Full Position (3D) 
 * ----------------------------------------------------------
 * 2      DGPS        BYTE     0        DGPS off         DGPS auto
 *        Mode                 1        DPGS only
 *                             2 or 3   DGPS auto
 * ----------------------------------------------------------
 * 3      Dynamics    BYTE     4        Land             Land
 *        Code                          Sea
 *                                      Air
 *                                      Stationary
 * ----------------------------------------------------------
 * 4-14   Reserved Not used
 * ----------------------------------------------------------
 * 15-18 Elevation    SINGLE 0.0 - 1.75 Lowest satellite 0.0873
 *       Mask                           elevation for fixes (radians) 
 * 
 * ----------------------------------------------------------
 * 19-22 AMU Mask     SINGLE            Minimum signal
 *                                      level for fixes  2.0
 * ----------------------------------------------------------
 * 23-26 DOP Mask     SINGLE            Maximum DOP for fixes 12.0
 * ----------------------------------------------------------
 * 27-30 DOP Switch   SINGLE            Selects 2D/3D mode 5.0
 * ----------------------------------------------------------
 * 31-34 Reserved Not used
 * ----------------------------------------------------------
 * 35 DGPS Age Limit  BYTE              Maximum time to use a DGPS
 *                                      correction
 *                                      (seconds)          30
 * ----------------------------------------------------------
 * 36-39 Reserved BYTE 0 Not used
 * 
 * Also supported by DATUM Starloc
 */
#define SET_PROCESSING_OPTIONS 0xBB

/**
 * Command Packet 0xBC
 * TSIP Packet 0xBC is used to query the port characteristics. In query mode, 
 * Packet 0xBC is sent with a single data byte and returns Report 
 * Packet 0xBC. See A.3 for information on saving the settings to 
 * non-volatile memory. TSIP Packet 0xBC is used to set the communication 
 * parameters on Port 1 and Port 2. The table below lists the individual 
 * fields within the Packet 0xBC.
 * Byte # Item Type Value Meaning
 * Port Number 
 *             BYTE  0   Port 1
 *                   1   Port 1
 *                  FF   Current port
 * 
 * Also supported by DATUM Starloc
 */
#define REQUEST_PORT_CHARACTERISTICS 0xBC



/**
 * Command packet
 */
#define SUPER_PACKET8E 0x8E
#define CONFIGURE_SUPER_PACKET 0x0B
/**
 * SuperPacket Sub 0B
 * Request Comprehensive Time response. 
 * 
 * Also supported by DATUM Starloc but has it listed as 0x15???
 */
#define SET_OR_REQUEST_DATUM 0x14
/**
 * For internally coded Datums use:
 * 0x0B
 * INT16  Datum number see appendix for details. 
 *
 * Alternatively we can give our own Datum parameters
 * 0x14
 * DOUBLE DX
 * DOUBLE DY
 * DOUBLE DZ
 * DOUBLE A-Axis
 * DOUBLE Eccentricity^2
 * 40 bytes total
 */ 
#define GET_LAST_FIX_EXTRA 0x20
/**
 * 0x20
 * BYTE 0 - 0 do not auto report
 *          1 auto report.
 */
#define WRITE_RX_CONFIG_TO_EEPROM 0x26
/**
 * 
 * Also supported by DATUM Starloc
 */
#define MANUFACTURING_PARAMTERS 0x41

/**
 * 
 * Also supported by DATUM Starloc
 */
#define PRODUCTION_PARAMETERS 0x42

/**
 * 
 * Also supported by DATUM Starloc
 */
#define REVERT_TO_DEFAULT 0x45

/**
 * 
 * Also supported by DATUM Starloc
 */
#define SET_REQUEST_PPS_CHARACTERISTICS 0x4A
/**
 * 0x4A
 * BYTE PPS Driver Switch
 * BYTE Time Base
 * BYTE PPS Polarity
 * DOUBLE PPS Offset or Cable delay in seconds
 * SINGLE Bias Unsertainty Threshold meters
 */

#define AUTOMATIC_PACKET_OUTPUT_MASK 0x4D
/*
 * UNSIGNED LONG bitmask. 
 * Bit  Output
 *  0    40         Almanac
 *  1    58, 5B     Ephemeris
 *  2    4F         UTC data
 *  3    58         Ionospheric Data
 *  4    48         GPS Message
 *  5    49         Almanac Health
 *  6    Reserved
 *  7    Reserved
 *  8    41         Partial and Full fix
 *  9    Reserved
 * 10    Reserved
 * 11    6D, 46, 4B, 82   New Satellite Selection/Constellation
 * 12-29 Reserved
 * 30    42, 43, 4A, 54, 56, 83, 84, 8F-20 Kinetic and Timing Info
 * 31    5A      RAW Measurement Data
 */
#define SET_PPS_OUTPUT_OPTION 0x4E
#define SET_PPS_WIDTH 0x4F


#define ONE_SATELLITE_BIAS 0x54
/**
 * The bias is the offset of the receiver internal time clock from GPS time.
 * Bias is expressed as meters of apparent range from the satellites. 
 * It is used to correct the 1PPS output. Bias rate is the frequency error of 
 * the receivers internal oscillator. It is expressed as apparent range rate.
 *
 * This is the compute clock-only solutoin when the receiver is in manual
 * or automatic one-satellite mode. 
 * Byte 0:3  SINGLE Bias in meters
 * Byte 4:7  SINGLE Bias Rate meters/sec
 * Byte 8:11 SINGLE Fix Seconds
 */
struct t_OneSatelliteBias {
    /// Bias in meters
    float Bias;
    float BiasRate;
    float TimeOfFix;
};

/**
 * Section A18
 */
#define SUPER_PACKET8F 0x8F

/**
 * Super packet sub defines.
 */
/**
 * 0x8F-0B
 * Comprehensive Time
 *
 * -----------------------------------------------------------
 * Table A-84. Command and Report Packet 0x8F-0B Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0x0B                      0x0B
 * 1-2    Event Count   INT16    External event counter, 0 for PPS
 * 3-10   UTC/GPS TOW   DOUBLE   UTC/GPS Time of week (seconds)
 * 11     Date          BYTE     Date of event or PPS
 * 12     Month         BYTE     Month of event or PPS
 * 13-14  Year          INTEGER  Year of event or PPS
 * 15     RX Mode       BYTE     0) Horiziontal (2D)
 *                               1) Full Position (3D)
 *                               2) Single Satellite
 *                               3) Automatic (2D/3D)
 *                               4) DGPS Reference
 *                               5) Clock Hold
 *                               6 Overdetermined mode (default)
 * 16-17 UTC Offset     INT16    UTC offset value seconds
 * 18-25 Oscillator Bias Double  Oscillator bias (meters)
 * 26-33 Oscillator Drift Rate   DOUBLE (meters/second)
 * 34-37 Oscillator Bias Uncertainty SINGLE (meters)
 * 38-41 Oscillator Drift Uncertainty SINGLE (meters/second)
 * 42-49 Latitude     DOUBLE     Latitude in radians
 * 50-57 Longitude    DOUBLE     Longitue in radians
 * 58-65 Altitude     DOUBLE     Altitude in Meters
 * 66-73 Satellite    8 BYTES    Satellite IDS used. 
 *
 * 
 */
#define COMPREHENSIVE_TIME 0x0B


/**
 * Report Packet 8F-14
 * Current Datum Values
 *
 * -----------------------------------------------------------
 * Table A-85. Command and Report Packet 0x8F-14 Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0x14                      0x14
 * 1-2    Datum Index   INT16
 * 3-10   DX            DOUBLE  (meters)
 * 11-18  DY            DOUBLE  (meters)
 * 19-26  DZ            DOUBLE  (meters)
 * 27-34  A-Axis        DOUBLE  (meters)
 * 35-42  Eccentricity^2 DOUBLE (meters)
 *
 * 
 */
#define CURRENT_DATUM 0x14


/**
 * Report Packet 8F-20
 * Last Fix with Extra Information
 *
 * -----------------------------------------------------------
 * Table A-86. Command and Report Packet 0x8F-14 Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0x20                      0x20
 * 1      Key Byte      BYTE              Reserved for Trimble DGPS
 * 2-3    east velocity INT16    0.005 m/s or 0.020 m/s (Byte 24)
 * 4-5    north vel.    INT16    "                   "
 * 6-7    up vel.       INT16    "                   "
 * 8-11   Time of Week  ULONG GPS Time in ms
 * 12-15  Latitude      INT32    semicircle according to datum
 * 16-19  Longitude     INT32    Longitude of east meridian. 
 * 20-23  Altitude      INT32    Altitude above ellipsoid 
 * 24     Vel. Scale    BYTE     BIT 0 only  = 1 Vel *= 4
 * 25     Reserved
 * 26     Datum         BYTE     Datum Index +1
 * 27     Fix type      BYTE     Byte 0 - 0 Fix was available
 *                                        1 No Fix was available. 
 *                               Byte 1   0 Fix is autonomous
 *                                        1 Fix was corrected with RTCM
 *                               Byte 2   0 3-D Fix
 *                                        1 2-D Fix
 *                               Byte 3   0 2-D fix used last circulated alt.
 *                                        1 2-D Fix used entered alt. 
 *                               Byte 4   0 Unfiltered
 *                                        1 Position or alt. filter on
 * 28     Num SVs       BYTE     Number of satellites in fix. 
 * 29     UTC Offset    BYTE     Number of leap seconds between UTC and GPS
 * 30-31  Week          INT16    GPS time of fix (weeks)
 * 32-47  Fix SVs       
 * 48-55  Iono Param    8 Bytes
 */
#define LAST_FIX_EXTRA 0x20

#define EEPROM_SEGMENTS 0x26
//#define MANUFACTURING_PARAMETERS 0x41
#define PRODUCTION_PARAMETERS 0x42
#define PPS_CHARACTERISTICS 0x4A
#define AUTOMATIC_PACKET_OUTPUT_MASK 0x4D


/**
 * Report Packet 8F-AB
 * TIME DATA
 *
 * -----------------------------------------------------------
 * Table A-94. Command and Report Packet 0x8F-AB Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0xAB                      0xAB
 * 1-4    Time of Week  ULONG    GPS seconds of week. 
 * 5-6    Week Number   INT16    GPS week number
 * 7-8    UTC Offset    INT16    UTC offset seconds
 * 9      Timing flag   BYTE     BIT0: 0-GPS Time, 1-UTC Time
 *                               BIT1: Reserved
 *                               BIT2: 0-Time is set, 1-Time is not set
 *                               BIT3: 0-Have UTC info, 1- No UTC Info
 * 10     Seconds
 * 11     Minutes
 * 12     Hours
 * 13     Day of Month
 * 14     Month
 * 15-16  Year
 */
#define TIME_DATA 0xAB
/// GPS time data
 struct t_TimeData {
     /// GPS time in week, seconds
     float          TimeOfWeek;
     /// GPS week number
     unsigned short WeekNumber;
     /// Offset in seconds relative to UTC
     short UTC_Offset;
     // Bit packed field
     unsigned char TimingFlag;
     /// Derived information
     char Seconds;
     char Minutes;
     char Hours;
     char DayOfMonth;
     char Month;
     unsigned short Year;
 };
/**
 * Report Packet 8F-AC
 * SUPPLEMENTAL TIME DATA
 *
 * -----------------------------------------------------------
 * Table A-95. Command and Report Packet 0x8F-AB Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0xAC                      0xAC
 * 1      Receiver Mode BYTE     0 - Automatic (2D/3D)
 *                               1 - Single Satellite (Time)
 *                               2 - 
 *                               3 - Horizontal (2D)
 *                               4 - Full Position (3D)
 *                               5 - DGPS Reference 
 *                               6 - Clock Hold (2D)
 *                               7 - Overdetermined Clock (default)
 * 2      Reserved
 * 3      Self Survey Progress  % complete
 * 4-9    Reserved
 * 10-11  Minor Alarms
 * 12     GPS Decoding Status
 * 13-15  Reserved
 * 16-19  Bias         SINGLE   Estimate of UTC/GPS offset from local clock(ns)
 * 20-23  Bias Rate    SINGLE   Estimate of UTC/GPS offset (ppb) 
 * 24-35  Reserved 
 * 36-43  Latitude     DOUBLE   Radians
 * 44-51  Longitude    DOUBLE   Radians
 * 52-59  Altitude     DOUBLE   Meters
 * 60-63  PPS Quant Error SINGLE (ns)
 * 64     PPS Output Status U8
 * 65-67  Reserved
 */
#define SUPPLEMENTAL_TIME_DATA 0xAC
/// Supplemental time data for reciever. 
struct t_SupplementalTimeData {
     char ReceiverMode;
     char SelfSurveyProgress;
     unsigned short MinorAlarms;
     unsigned char GPSDecodingStatus;
     /// Receiver bias in nanoseconds
     float Bias;  
     /// Receiver bias rate change in ppb
     float BiasRate;
     /// Latitude in radians
     double Latitude;
     /// Longitude in radians
     double Longitude;
     /// Altitude in meter, not sure what this is relative to. 
     double Altitude; 
     /// Quantization error in nanonseconds
     float PPSQuantizationError;
     /// Bit packed PPS status. 
     unsigned char PPSOutputStatus;
 };

/**
 * Report Packet 8F-AD
 * PRIMARY TIME DATA
 *
 * -----------------------------------------------------------
 * Table A-94. Command and Report Packet 0x8F-AB Field Descriptions
 * Byte # Item          Type     Value    Meaning          Default
 * 0      Subpacket ID  BYTE     0xAD                      0xAD
 * 1-2    Event Count   INT16
 * 3-10   Fractional Second DOUBLE Time ellapsed in current second
 * 11     UTC Hour
 * 12     UTC Minute
 * 13     UTC Second
 * 14     Day
 * 15     Month
 * 16-17  Year
 * 18     RX Status
 * 19     UTC Flags
 * 20     Reserved
 * 21     Reserved
 */
#define PRIMARY_UTC_TIME 0xAD
/// Receiver UTC time
struct t_PrimaryUTCTime {
    unsigned short EventCount;
    double FractionalSecond;     /// Seconds
    char Hour;
    char Minute;
    char Second;
    char Day;
    char Month;
    unsigned short Year;
    unsigned char ReceiverStatus;
    unsigned char UTCFlags;
};

enum GPS_MODE { MANUAL_GPS, MANUAL_DGPS, AUTO_GPS, AUTO_DGPS};

/// IO Setup Parameters
/// Position setup, XYZ and LL are also used in velocity.
#define POS_XYZ_ECEF 0x01
#define POS_LLA      0x02
#define POS_ALT      0x04
#define ALT_IN       0x08
#define POS_PREC     0x10
#define SUPER_PACK   0x20
#define VEL_XYZ      0x01
#define VEL_NEU      0x02

/// Additional PR data.
#define AUX_ON       0x01
#define RAW_PR       0x02
#define NOISE_RPT    0x08
const unsigned char DLE = 0x10;
const unsigned char ETX = 0x03;

enum GPS_DATA_TYPE
{ GPS_ALMANAC = 2, GPS_HEALTH, GPS_IONOSPHERE, GPS_UTC, GPS_EPHEMERIS };
/// Revision data
class Revision_Info
{
 public:
    Revision_Info(Buffered *buf = NULL);
    void Fill (Buffered *buf);
    void Print(const char *title = "none") const;
    void FormatData(char *buffer, size_t buf_size) const;
 private:
    unsigned char Major_ID, Minor_ID, Month, Day, Year;
};

/// Class to contain the signal level per PRN. 
class SignalLevel : public DataTimeStamp
{
public:
    /// Constructor
    SignalLevel() : DataTimeStamp() {fID = 0; fValue = 0.0;};
    /// Destructor
    ~SignalLevel() {fID=0;};

    inline void Set(unsigned char id, float value) {fID=id;fValue=value;};
    inline void Clear(void) {fID=0;fValue=0.0;};
    inline unsigned char ID(void) const {return fID;};
    inline float      Value(void) const {return fValue;};

private:
    /// PRN or ID of satellite for which the signal level was obtained
    char  fID;
    /// Value of signal level. 
    float fValue;
};

/// Data on almanac received
struct t_Almanac
{
    /// raw ??
    unsigned char t_oa_raw;
    /// Satellite vehicle health, status bits
    unsigned char SV_HEALTH;
    /// Eccentricity Dimensionless
    float e;
    /// Almanac Reference Time, Seconds
    float t_oa;
    /// Inclination Angle at reference time, Semicircles
    float i_o; 
    /// Rate of Right Ascension, Semicircles per second
    float OMEGADOT;
    /// Squart root of semi-major axis, sqrt(meters) 
    float sqrt_A; 
    /// Longitude of Ascending Node of Orbit Plan at Weekly Epoch, Semicircles
    float OMEGA_0;
    /// Argument of Perigee, Semicircles
    float omega;
    /// Mean Anomaly at Reference time, Semicircles
    float M_0;
    /// Seconds
    float a_f0;
    /// Seconds per second
    float a_f1;
    float Axis;
    float n;
    float OMEGA_n;
    float ODOT_n;
    float t_zc;
    short weeknum;
    short wn_oa;
};
/// Roll up of all SV 
struct t_AlmanacHealth
{
    unsigned char WeekNumber;
    unsigned char SV_Health[32];
    unsigned char t_oa;
    unsigned char current_t_oa; // units = seconds/2048
    short CurrentWeek;
};
/// Ionospheric corrections
struct t_iono
{
    float alpha[4];
    float beta[4];
};
/// Full UTC data
struct t_UTCData
{
    double A0;
    float A1;
    short delta_t_LS;
    float t_ot;
    short WN_t;
    short WN_LSF;
    short DN;
    short delta_t_LSF;          // 20.3.3.5.1.8
};
/// All ephemeris data
struct t_Ephemeris
{
    /// Satellite vehicle ID
    char   svid;
    /// Time of collection.
    float  t_ephem; 
    short  weeknum;
    char   codeL2;
    char   L2Pdata;
    char   SVacc_raw;
    char   SV_health;             // 20.3.3.3 table 20-I
    short  IODC;
    float  T_GD;
    float  t_oc;
    float  a_f2;
    float  a_f1;
    float  a_f0;
    float  SVacc;
    char   IODE;
    char   fit_interval;
    float  C_rs;
    float  delta_n;
    double M_O;
    float  C_uc;
    double e;
    float  C_us;
    double sqrt_A;
    float  t_oe;
    float  C_ic;
    double OMEGA_O;
    float  C_is;
    double i_O;
    float  C_rc;
    double omega;
    float  OMEGADOT;
    float  IDOT;
    /// sqrt_A * sqrt_A
    double Axis;
    /// derivied from delta_n.
    double n;
    /// = sqrt(1.0-e^2)
    double r1me;  
    /// derived from OMEGA_0 and OMEGADOT
    double OMEGA_n; 
    /// derived from Omegadot.
    double ODOT_n; 
};

/// Data from report 5A.
struct t_RawMeasurement
{
    /// RRN, satellite identification
    char PRN;
    /// Sample length in milliseconds
    float SampleLength; 
    /// Signal level in AMU or dBHz
    float SignalLevel;
    /// Code phase in 1/16 of chip
    float CodePhase;
    /// Doppler correction in Hertz
    float Doppler; 
    /// Time of measurement relative to code phase. 
    double TimeOfMeasurement;

};
/// Synchronized data packet, not supported by lassen SK-8
struct t_SynchronizedPacket
{
    /// Satellite PRN or unique identifier.
    char SVPRN;
    /// Measurement status of satellite.
    char Flags1; 
    /// Reserved
    char Flags2;
    /// Elevation in degrees
    char Elevation;
    /// Azimuth in degrees
    short Azimuth;
    /// SNR in AMUs/4
    char SNR;  
    /// Pseudorange in meters
    double Pseudorange;
    /// Carrier phase in cycles
    double CarrierPhase;
    /// Doppler in Hertz
    float Doppler; 
};
/*!
 * 0x35 set/query, 0x55 reply
 * Options
 *          Position
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
 * Bit patterns
 * Position:
 */
const unsigned char kXYZ_ECEF = 0x01;  /*! Set to turn on XYZ ECEF */
const unsigned char kLLA      = 0x02;  /*! Set to turn on LLA */
const unsigned char kALT      = 0x04;  /*! Set to use MSL geoid as altitude */
const unsigned char kALT_IN   = 0x08;  /*! Set to use MSL geoid as altitude */
const unsigned char kPREC     = 0x10;  /*! Set for double precision */
const unsigned char kSUPER    = 0x20;  /*! Set to turn super packets on */
/*! Velocity */
/*! ECEF XYZ is same as position. */
const unsigned char kENU      = 0x02;  /*! Set to turn ENU on */
/*! Time */
const unsigned char kUTC      = 0x01;  /*! Set for UTC, reset for GPS */
/*! AUX */
const unsigned char kFilter   = 0x02;  /*! Set for filtered PRs in 0x5A */
const unsigned char kAMU      = 0x08;  /*! Set for AMU, reset for dBHz*/
 
struct t_IO_Options {
    bool Set;
    /// Position reporting options.
    unsigned char Pos;
    /// Velocity reporting options 
    unsigned char Vel;
    /// Timing reporting options.
    unsigned char Timing; 
    /// Aux Reporting options
    unsigned char Aux; 
};

/**
 * The main Lassen class that draws it all together. 
 */
class Lassen
{
  public:
    /// Main  constructor
    Lassen ();
    /// Main destructor
    ~Lassen();

    /*! reset all the various frame parameters once per full fix cycle. */
    void ResetFrame(void);

    /*! The message Steering routine.  */
    int DecodeMessage (Buffered *buf);

    /// Get the time from the last reception. 
    inline GPSTime* GTime(void) const { return fGPStime;};

    /// Return pc time at time of GPSTIME measurement. 
    struct timespec PCTime(void)  const { return fGPStime->PCTime();};

    /// Return current position.
    inline TSIPosition *GetPositionData() {return fLLPosition;};

    /// Return the current velocity data
    inline TSIPVelocity *GetVelocityData() {return fENUVelocity;};

    /// what is  the  status of the last fix?
    inline SolutionStatus* LastStatus() {return fSStatus;};

    /// Current fix mode. 
    inline int CurrentMode() const {return fMode;};
    /// Set the current verbosity.
    inline  void  SetVerbosity(const  int Level) {fVerbosity = Level;};

    inline void RequestIO_Options (void)
    {
        LoadPrefix (SET_IO_OPTION_STATE);
        LoadSuffix ();
    };
    /// Setup io options.
    void SetIO_Options (const unsigned char Position,
                        const unsigned char Velocity, 
			const unsigned char Timing,
                        const unsigned char AuxPR);
    /// Setup io options.
    void SetIO_Options (const struct t_IO_Options *in);

    /// Request the software revision. 
    inline void GetSoftwareVersion ()
    {
        LoadPrefix (SOFTWARE_VERSION);
        LoadSuffix ();
    };

    /// Request the GPS Time. 
    inline void GetTime ()
    {
        LoadPrefix (GET_TIME);
        LoadSuffix ();
    };

    /// Make a request to return the satellite health. 
    inline void GetHealth ()
    {
        LoadPrefix (GET_RX_HEALTH);
        LoadSuffix ();
    };

    /// Make a request to the receiver to send us the RX signal levels. 
    inline void GetRxSignalLevels ()
    {
        LoadPrefix (GET_RX_SIGNAL_LEVEL);
        LoadSuffix ();
    };

    /// Request the FIX mode. 
    inline void GetFixMode ()
    {
        LoadPrefix (REQUEST_FIX_MODE);
        LoadSuffix ();
    };
    /// Set the filters on or off. 
    inline void GetFilter ()
    {
        LoadPrefix (REQUEST_FILTER);
        LoadSuffix ();
    };

    /// What is the NMEA mask set to? 
    inline void RequestNMEAmask ()
    {
        LoadPrefix (SET_NMEA_OUTPUT);
        SendChar (0);
        LoadSuffix ();
    };
    /// Request current satellite data
    inline void RequestSatelliteData (const GPS_DATA_TYPE type,
                                      const char PRN)
    {
        LoadPrefix (GET_OR_SET_ALM);
	// 24-Dec-15 change order
        SendChar (type);
        SendChar (1);
        SendChar (PRN);
        LoadSuffix ();
    };

    /**
     * Request tracking on PRN number.
     * Specific PRN or if 0, all available. 
     */
    inline void RequestTrackingStatus (const char PRN=0)
    {
        LoadPrefix (GET_TRACKING_STATUS);
        SendChar (PRN);
        LoadSuffix ();
    };

    /// Enable or Disable Synchronized parameters!! 
    inline void SetSynchronizedParameters (const bool enable,
                                           const char Interval)
    {
        LoadPrefix (0x6E);
        SendChar (1);           // always send a 1. 
        if (enable)             // Send 1 to enable. 
            SendChar (1);
        else
            SendChar (0);
        // Finish by sending interval. 
        SendChar (Interval);
        LoadSuffix ();
    };
    inline void RequestSynchronizedParametersStatus ()
    {
        LoadPrefix (REQUEST_SYNCHRONIZED_MEASUREMENTS);
        SendChar (1);
        LoadSuffix ();
    };

    inline void HardReset ()
    {
        LoadPrefix (COMMAND_HARDWARE_RESET);
        SendChar (0x46);
        LoadSuffix ();
    };
    /// Request a Reset
    inline void SoftReset ()
    {
        LoadPrefix (COMMAND_SOFTWARE_RESET);
        LoadSuffix ();
    };
    /// 
    inline void RequestDifferentialFixMode ()
    {
        LoadPrefix (REQUEST_DIFFERENTIAL_FIX);
        SendChar (0x05);
        LoadSuffix ();
    };

    /// Get Differential corrections for a specific satellite. 
    inline void RequestDifferentialStatus ()
    {
        LoadPrefix (REQUEST_SPECIFIC_DIFFERENTIAL_CORR);
        LoadSuffix ();
    };
    /// Request the receiver to send oscillator offset value.
    inline void RequestOscillatorOffset ()
    {
        LoadPrefix (GET_OSCILLATOR_OFFSET);
        LoadSuffix ();
    };
    /**
     * When positon fixes are not being output regularly, Get the most 
     * recient position fix. 
     */
    inline void RequestLastPositionFix ()
    {
        LoadPrefix (GET_LAST_FIX);
        LoadSuffix ();
    };
    /// Request the receiver to send the ACSII system message. 
    inline void RequestSystemMessage ()
    {
        LoadPrefix (GET_ASCII_SYSTEM_MESSAGE);
        LoadSuffix ();
    };

    /// Set the NMEA mask.
    inline void RequestNMEAmask (const unsigned int mask,
                                 const unsigned char interval)
    {
        LoadPrefix (SET_NMEA_OUTPUT);
	// 24-Dec-15 unchecked.
        SendChar (0);
        SendChar (interval);
        SendLong (mask);
        LoadSuffix ();
    };
    /// For processing outgoing commands.
    inline const unsigned char* GetCommandBuffer() const {return fCommand;};
    inline size_t GetCommandSize() {return (size_t)(fCmdPtr-fCommand);};

    inline t_TimeData GetSPTime() const {return fTimeData;};
    inline t_SupplementalTimeData GetSupplementalTimeData() 
	const {return fSTimeData;};
    inline const Revision_Info *GetRevisionInfo(int type) const 
    {
	Revision_Info *rc = NULL;
	switch(type)
	{
	case 0:
	    return fNavigationProcessor;
	    break;
	case 1:
	    return fSignalProcessor;
	    break;
	}
	return rc;
    };
    inline struct t_IO_Options GetIO_Options(void) const {return fIO_Options;};
    inline bool         IsDataOutOfBounds(void) const 
	{return fDataOutOfBounds;};
    inline void         ResetOOB(void) {fDataOutOfBounds = false;};
    inline SignalLevel* GetSignalLevels(void) {return fSLevel;};
    inline RawTracking* GetRawTrackingData(void) {return fpRawTracking;};
    inline int          GetLastError(void);
    /**
     * Method to clear the raw tracking array.
     */
    void ClearRawTracking(void);
    inline int  GetRawCount (void) const {return fPRNCountB;};

    /// Added in 18-Oct-15, remove any double DLE's Applies to lassen only. 
    //void RemoveDoubleDLE(Buffered *b);

    /// Error enum
    enum TSIP_ERRORS {NO_DECODE_ERROR, BUFFER_TOO_SMALL};

    // _________________________________________________________________
    // _________________________________________________________________

  private:
    /* 28-Jun-08, new buffering concept */
    Buffered *fBuffer;

    /// Verbose level for dumping run status. 
    int fVerbosity;
    /// Error from last operation
    int fError;

    /*! Last ID decoded, used to debug issues with left over stuff. */
    unsigned char       fLastMessageID;
    /// Last message was time or position.
    bool                fGotTime, fGotPosition;

    /**
     * Counters for receiving PRN data in Decode Satellite Raw
     * and Raw Tracking. 
     */
    int  fPRNCount; 
    bool fDataOutOfBounds;

    /// Private data variables. 
    unsigned char   fRxStatus;
    SolutionStatus *fSStatus;

    /// Current fix mode. Packet ID 0x82
    int fMode, fRCTM_Version, fReferenceStationID;

    /// 0x4B data
    unsigned char fMachineID;
    unsigned char fStatus[2];

    GPSTime*           fGPStime; // GPS time, every 5 seconds. 

    Revision_Info      *fNavigationProcessor, *fSignalProcessor;

    TSIPosition*       fLLPosition;
    TSIPVelocity*      fENUVelocity;
    SignalLevel        *fSLevel;
    struct t_Almanac   fAlmanac;
    struct t_AlmanacHealth fAlmanacHealth;
    struct t_iono      fIonosphere;
    struct t_UTCData   fUTC;
    struct t_Ephemeris fEphemeris;
    char               fUseSatellite[32];
    struct t_RawMeasurement fRawData[MAXPRNCOUNT];
    RawTracking        *fpRawTracking;
    /// Number of PRN's in the tracking array, also current pointer
    int                fPRNCountB; 

    struct t_SynchronizedPacket fSP[MAXPRNCOUNT];
    struct t_IO_Options fIO_Options;

    /// Super packet data
    struct t_TimeData   fTimeData;
    struct t_SupplementalTimeData fSTimeData;

    /// Private methods used in setting up the receiver. 
    unsigned char fCommand[256], *fCmdPtr;
    /// Each and every command is prefixed. 
    inline void LoadPrefix (unsigned char cmd)
    {
        memset (fCommand, 0, sizeof (fCommand));
        fCmdPtr = fCommand;
	*fCmdPtr = DLE;
	fCmdPtr++;
        *fCmdPtr = cmd;
        fCmdPtr++;
    };
    ///  Each and every command has a suffix as well. 
    inline void LoadSuffix (void)
    {
	// 24-Dec-15 changed order
        *fCmdPtr = DLE;
        fCmdPtr++;
        *fCmdPtr = ETX;
        fCmdPtr++;
    };
    /// Put a single character into the output stream. 
    inline void SendChar (const unsigned char data)
    {
        if (data == DLE)
        {
            *fCmdPtr = DLE;
            fCmdPtr++;
        }
        *fCmdPtr = data;
        fCmdPtr++;
    };

    /**
     * Multiple-byte numbers (integer, float, and double) follow the 
     * ANSI/IEEE Std. 754 IEEE
     * Standard for binary Floating-Point Arithmetic. They are sent 
     * most-significant byte first.
     * This may involve switching the order of the bytes as they are 
     * normally stored in Intel
     * based machines. Specifically:
     *     INTEGER - A 16 bit unsigned number sent in two's complement format.
     *     SINGLE  - Float, or 4 byte REAL has a precision of 24 significant 
     *               bits, roughly 6.5 digits.
     *     DOUBLE  - 8 byte REAL has a precision of 52 significant bits. 
     *               It is a little better than 15 digits.
     *
     * Note that if any byte in the data stream is a DLE then it MUST be 
     * sent twice.
     *
     */
    /// Put a SINGLE into the data stream.
    inline void SendSingle (const float data)
    {
        unsigned char *p = (unsigned char *) &data;
        SendChar (p[3]);
        SendChar (p[2]);
        SendChar (p[1]);
        SendChar (p[0]);
    };

    /// Put an INTEGER into the data stream.
    inline void SendInteger (const short data)
    {
        unsigned char *p = (unsigned char *) &data;
        SendChar (p[1]);
        SendChar (p[0]);
    };

    /// Put an LONG into the data stream.
    inline void SendLong (const unsigned int data)
    {
        unsigned char *p = (unsigned char *) &data;
        SendChar (p[3]);
        SendChar (p[2]);
        SendChar (p[1]);
        SendChar (p[0]);
    };

    /**
     * Commands sent to Receiver. 
     */
    /// Set or Clear the Oscillator offset value.
    void SetOscillatorOffset (const float offset, const bool set);

    /// Clear battery backed up RAM, Reload factory defaults, and reset!

    /// Set the initial position in ECEF
    void SetECEF (const float X, const float Y, const float Z);

    /// Set or clear the system altitude. input of zero resets system altitude.
    void SetSystemAltitude (const float alt);


    /**
     * Set the GPS time.
     *
     * Seconds - Number of seconds into GPS week. 
     * Week    - GPS week number reference is Week #0 starting January 6, 1980.
     */
    void SetGPSTime (const float Seconds, const short Week);

    /**
     * Set an approximate receiver position in Latitude and Longitude. 
     * Inputs are: 
     *    Latitude  North in radians. 
     *    Longitude East in radians. 
     *    Altitude  in Meters. 
     */
    void SetPosition (const float Latitude, const float Longitude,
                      const float Altitude);

    /// Set the initial position in ECEF, Only use this with precise data!!!
    void SetAccurateECEF (const float X, const float Y, const float Z);

    /**
     * Set an approximate receiver position in Latitude and Longitude. 
     * Inputs are: 
     *    Latitude  North in radians. 
     *    Longitude East in radians. 
     *    Altitude  in Meters. 
     *
     * Input data assumed to be accurate/precise. 
     */
    void SetAccuratePosition (const float Latitude, const float Longitude,
                              const float Altitude);

    /// Send Almanac data
    void SetSatelliteData (const GPS_DATA_TYPE type, const char PRN,
                           const char *data, int data_size);

    /// Set satellite usage
    inline void SetSatelliteUsage (const char command, const char PRN)
    {
        LoadPrefix (SELECT_SATELLITE);
        SendChar (command);
        SendChar (PRN);
        LoadSuffix ();
    };


    /// Set the filters on or off. 
    void SetFilter (const bool PV, const bool Static, const bool Altitude);
    ///
    int DecodeSatelliteTracking ();
    ///
    int DecodeFixData ();
    ///
    int Decode_GPS_Time ();
    ///
    int Decode_SoftwareRevision ();
    ///
    int Decode_RxHealth ();
    ///
    int Decode_RxMode ();
    /// 
    int Decode_PositionSingle ();
    /// 
    int Decode_PositionDouble ();
    ///
    int Decode_VelocityENU ();
    /// 
    int Decode_SignalLevels ();
    /// 
    int Decode_SatelliteData ();
    ///
    int LoadAlmanac (unsigned char Nbytes, int PRN);
    ///
    int LoadAlmanacHealth (unsigned char Nbytes, int PRN);
    ///
    int LoadIono (unsigned char Nbytes, int PRN);
    ///
    int LoadUTC (unsigned char Nbytes, int PRN);
    ///
    int LoadEphemeris (unsigned char Nbytes, int PRN);
    /// 
    int Decode_SatelliteDisable ();
    /// 
    int Decode_SatelliteRaw ();
    int Fill_SatelliteRaw ();
    /// 
    int Decode_SatelliteRawTracking ();
    ///
    int Decode_SyncronizedPacket ();
    ///
    int Decode_SynchronizedMeasurementsReply ();
    /// 
    int Decode_SuperPacket2 ();
    /// Subcode 0x20
    int Decode_PositionSuperPacket ();
    ///
    int Decode_IO_Options ();

    /// Check for 0x10 0x10 occurances and strip them out.
    bool Check1010();
    /// Remove all 1010 occurances.
    bool Remove1010();

    /**
     * Super packet decode tools. 
     */
    int DecodeTimeData();
    int DecodeSupplementalTimeData();
    int DecodePrimaryTimeData();
    /**
     * Return the pointer to the existing PRN in 
     * the raw tracking array if present. 
     */
    int FindRawTrackingPRN(int i) const;
#if 0
    /**
     * Look for PRN,s that haven't been updated in awhile and clean house.
     */
    void CleanTrackingPRN();
    /**
     * Compact PRN array
     */
    int CompactRawTracking();
#endif
};
/**
 * This represents the maximum potential number  of  bytes,sans superpackets
 * that the lassen  could spit out if we had all measurements turned on.  
 */
const int LassenBufferSize=1024;


#endif
