/**
 ******************************************************************
 *
 * Module Name : Version.hh 
 *
 * Author/Date : C.B. Lirakis / 23-Jan-21
 *
 * Description : Software versioning information
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * 24-Jan-21   CBL    Version 1.3 Works, doing clean up on a subsystem
 *                    by subsystem basis. Adding in StatusAndEvent
 *                    to determine hardware inventory and details 
 *                    about the system. 
 * 
 *                    V1.4 - Previously I handled the GPIB handle in an
 *                    unusual manner. I am going to collapse all the GPIB
 *                    extention files. 
 *
 *                    - I merged the module files into StatusAndEvent, but
 *                    this is incomplete.
 *                    - Adding in Measurement Module
 *
 *                    Version 1.5 Complete Measurement module, 
 *                    Merged ChannelGPIB and Channel into a single 
 *                    class AND added in all the other commands 
 *                    associarted with CH .
 *
 *                    Version 1.7 
 *                    The hierarchy is something like Mainframe - Module
 *                    channel. Need to remove or collapse ModuleChannel. 
 *
 *                    Version 1.8
 *                    Completed the implementation of Channel, need to
 *                    do more testing.  
 *                    Removed the remaining GPIB files and incorprated
 *                    them into their parent classes. (WFMPRE, DSAFFT)
 *
 *                    Version 1.9
 *                    Waveform is decomposed into WFMPRE and Curve. 
 *
 *                    Version 1.91
 *                    WFMPRE working, 
 *                    Working on timebase
 *
 *                    Version 1.92
 *                    TB command starting on page 272 implemented flesh
 *                    out more detail in the class TimeBase. 
 *
 *                    Version 1.93
 *                    Finish out Trace Class.  Move query and set
 *                    from Trace to AdjTrace class. 
 *
 *                    Version 1.94
 *                    Trace class done but isn't testing well. 
 *
 *   18-Dec-22        Version 2.0 about to branch into library. 
 *                    
 *
 * Classification : Unclassified
 *
 * References : DSA602 Programmers reference manual. 
 *
 *
 *******************************************************************
 */
#ifndef __Version_hh_
#define __Version_hh_

#define XXXX_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define MAJOR_VERSION 2
#define MINOR_VERSION 0

#endif
