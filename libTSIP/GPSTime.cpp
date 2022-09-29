/********************************************************************
 *
 * Module Name : GPSTime.cpp
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Manage the GPS time, 0x41
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
 * References : NONE
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <ctime>
#include <cstring>
#ifdef __APPLE__
#include <sys/time.h>
#endif
// Local Includes.
#include "debug.h"
#include "GPSTime.hh"
#include "TSIPUtility.hh"

/**
 ******************************************************************
 *
 * Function Name : GPSTime constructor
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
GPSTime::GPSTime (void) : DataTimeStamp()
{
    SET_DEBUG_STACK;
    Clear();
}


/**
 ******************************************************************
 *
 * Function Name : GPSTime destructor
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
GPSTime::~GPSTime (void)
{
    SET_DEBUG_STACK;
}


/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : reset all variables. 
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
void GPSTime::Clear(void)
{
    SET_DEBUG_STACK;
    fTimeOfWeek = 0.0f;
    /*! Extended GPS week number */
    fExtendedWeek = 0;
    /*! GPS/UTC offset in seconds */
    fUTC_Delta = 0.0f;
    /*! Time that this data was acquired, PC clock */
    memset(&fPCTime, 0, sizeof(struct timespec));
    /*! delta between the GPS and PC */
    fDelta = 0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SolutionStatus operator << overload
 *
 * Description :
 *
 * Inputs : output stream to append to
 *          n - Position class
 *
 * Returns : packed ostream to do with as you please. 
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
ostream& operator<<(ostream& output, const GPSTime &n)
{
    SET_DEBUG_STACK;
    struct timespec pc     = n.PCTime();
    struct timespec GPSSec = n.DateFromGPSTime();
    char gps_string[64], pc_string[64];

    time_t sec = GPSSec.tv_sec;
    strftime( gps_string, sizeof(gps_string), "%F %T",localtime(&sec));
    sec = (time_t)  n.PCTime().tv_sec;
    strftime( pc_string, sizeof(pc_string), "%F %T", localtime(&sec));

    output << " GPS Time, " 
	   << "Time of week:"   << n.GPSTimeOfWeek()
	   << " Extended Week:" << n.ExtendedGPSWeek()
	   << " UTC Delta:"     << n.UTC_Delta()
	   << " PCTime:"        << pc.tv_sec << " " << pc.tv_nsec  
	   << " Delta:"         << n.Delta()
	   << endl
	   << " GPS:" << gps_string
	   << " PC:" << pc_string;

    SET_DEBUG_STACK;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name : SetPCTime
 *
 * Description : Set the PCTime value to the current PC time. 
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
void GPSTime::SetPCTime(void)
{
    SET_DEBUG_STACK;
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fPCTime.tv_sec  = tv.tv_sec;
    fPCTime.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fPCTime);
#endif
    SET_DEBUG_STACK;

}
/**
 ******************************************************************
 *
 * Function Name : SetDelta
 *
 * Description : Compute the delta given the current values.
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
void GPSTime::SetDelta(void)
{
    SET_DEBUG_STACK;
    struct timespec GPSSec = DateFromGPSTime();
    fDelta = (double) (fPCTime.tv_sec-GPSSec.tv_sec) + 
	((double)(fPCTime.tv_nsec - GPSSec.tv_nsec))/1.0e9;
#if 0
    cout << "DELTA: " << fGPStime.delta 
	 << endl;
#endif
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : CheckBounds
 *
 * Description : Compute the delta given the current values.
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
bool GPSTime::CheckBounds(void)
{
    SET_DEBUG_STACK;
    return ( (fExtendedWeek > 2048)        ||
	     (fTimeOfWeek > (86000.0*7.0)) ||
	     (fTimeOfWeek < 0.0)           ||
	     (fUTC_Delta  < 0.0) );
}
/**
 ******************************************************************
 *
 * Function Name : DateFromGPSTime
 *
 * Description : Conversion from internal time standards to something
 * that is usable. 
 *       TimeOfWeek runs 0-1023 
 *          Week zero was January 6, 1980. This was crossed August 22, 1999.
 *       Extended GPS week number, August 22, 1999 is wek 1024. 
 *
 *          
 *
 * Inputs : TimeOfWeek    - time since our epoch. 
 *          Extended Week - see above
 *          UTC_Offset    - seconds from GPS time that UTC is 
 *
 * Returns : timespec of GPSWeek. 
 *
 * Error Conditions :
 *
 *******************************************************************
 */
struct timespec GPSTime::DateFromGPSTime(void) const
{
    const unsigned  SecPerDay  = 60*60*24;
    const unsigned  SecPerWeek = SecPerDay*7;
    // First Epoch is January 6, 1980. 
    //const time_t    gps_epoch1 = 315964800;
    // Second Epoch is August 22, 1999
    const time_t    gps_epoch2 = 935280000;
    struct timespec rv;
    double fsec = (double)(gps_epoch2 + fExtendedWeek*SecPerWeek);
    fsec       -= fUTC_Delta;
    fsec       += fTimeOfWeek;

    rv.tv_sec  = (unsigned long) floor(fsec);
    rv.tv_nsec = (unsigned long) floor(((double)(fsec-rv.tv_sec))*1.0e9);
    return rv;
}
