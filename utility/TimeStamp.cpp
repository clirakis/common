/**
 ******************************************************************
 *
 * Module Name : TimeStamp.cpp
 *
 * Author/Date : C.B. Lirakis / 06-Jul-03
 *
 * Description : create a methodology to time stamp data on input. 
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
 * References : NONE
 *
 *******************************************************************
 */

// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <csignal>
#ifdef __APPLE__
#  include <sys/time.h>
#endif

/// Local Includes.
#include "debug.h"
#include "TimeStamp.hh"

/**
 ******************************************************************
 *
 * Function Name : DataTimeStamp
 *
 * Description : Create the DataTimeStamp class
 *
 * Inputs : delta - a threshold for when the data is expired in seconds. 
 *
 * Returns : 
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
DataTimeStamp::DataTimeStamp(const double delta)
{
    SET_DEBUG_STACK;
    fThreshold = delta;
    Stamp();
}
/**
 ******************************************************************
 *
 * Function Name : DataTimeStamp
 *
 * Description : a constructor for use with another DataTimeStamp
 *
 * Inputs : in - DataTimeStamp class input to copy. 
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
DataTimeStamp::DataTimeStamp(DataTimeStamp &in)
{
    SET_DEBUG_STACK;
    fThreshold = in.fThreshold;
    fdt = in.fdt;
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fLast.tv_sec  = tv.tv_sec;
    fLast.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fLast);
#endif
    fCrossedThreshold = false;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : DataTimeStamp
 *
 * Description : destructor - does nothing for the moment. 
 *
 * Inputs : none
 *
 * Returns : none
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
DataTimeStamp::~DataTimeStamp(void)
{
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Now
 *
 * Description : calculates the time delta between
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
double DataTimeStamp::Now()
{
    SET_DEBUG_STACK;
    fdt = DT();
    fCrossedThreshold = (fdt > fThreshold);
    SET_DEBUG_STACK;
    return fdt; 
}
/**
 ******************************************************************
 *
 * Function Name : AsComparedToNow
 *
 * Description : Calculate if we have cross the threshold in seconds
 * for old data
 *
 * Inputs : NONE
 *
 * Returns : true if threshold has been exceeded. Does the test, but
 * does not set the value in the class. 
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
bool DataTimeStamp::AsComparedToNow(void) const
{
    SET_DEBUG_STACK;
    struct timespec now;
    double dt_local;
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    now.tv_sec  = tv.tv_sec;
    now.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &now);
#endif
    dt_local = (double) (now.tv_nsec - fLast.tv_nsec);
    dt_local *= 1.0E-9;
    dt_local += (now.tv_sec - fLast.tv_sec);
    SET_DEBUG_STACK;
    return (dt_local > fThreshold);
}
/**
 ******************************************************************
 *
 * Function Name : Stamp
 *
 * Description : Set Last to the current time. 
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
void DataTimeStamp::Stamp(void)
{
    SET_DEBUG_STACK;
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fLast.tv_sec  = tv.tv_sec;
    fLast.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fLast);
#endif
    fCrossedThreshold = false;
    fdt         = 0.0;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : DT
 *
 * Description : set the current time in the class
 *               claculates the time delta between last and now
 *
 * Inputs : NONE
 *
 * Returns : double - number of seconds since last was set. 
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
double DataTimeStamp::DT(void) const
{
    SET_DEBUG_STACK;
    double ddt;
    struct timespec now;
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    now.tv_sec  = tv.tv_sec;
    now.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &now);
#endif
    ddt = (double) (now.tv_nsec - fLast.tv_nsec);
    ddt *= 1.0E-9;
    ddt += (now.tv_sec - fLast.tv_sec);
    SET_DEBUG_STACK;
    return ddt;
}
/**
 ******************************************************************
 *
 * Function Name : LastTime
 *
 * Description : Return the last stamped time as a double. 
 *
 * Inputs : NONE
 *
 * Returns : double - current time stamp 
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
double DataTimeStamp::DLastTime(void) const
{
    SET_DEBUG_STACK;
    double val = fLast.tv_nsec;
    val /= 1.0e9;     // The input was an integer nanoseconds, convert. 
    val += fLast.tv_sec;
    return val;
}
ostream& operator<<(std::ostream& output, const DataTimeStamp &n)
{
    SET_DEBUG_STACK;
    output << n.DLastTime(); 
    return output;
}
