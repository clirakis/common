/********************************************************************
 *
 * Module Name : TimeUtil.cpp
 *
 * Author/Date : C.B. Lirakis / 28-Feb-24
 *
 * Description : Time utilities, meant to superceed PreciseTime 
 *               utility. Differences etc, set alarm for 
 *               time exceeded, etc. 
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
#include <ostream>
#include <cstring>
#include <time.h>

#ifdef __APPLE__
#  include <sys/time.h>
#endif

// Local Includes.
#include "debug.h"
#include "TimeUtil.hh"

/**
 ******************************************************************
 *
 * Function Name : TimeUtil constructor
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
TimeUtil::TimeUtil (void)
{
    SET_DEBUG_STACK;
    Now(&fLastTouched); // Set the last touched time. 
    memset(&fUserTime, 0, sizeof(struct timespec));
}

/**
 ******************************************************************
 *
 * Function Name : TimeUtil destructor
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
TimeUtil::~TimeUtil (void)
{
}


/**
 ******************************************************************
 *
 * Function Name : Now
 *
 * Description : System independent way of getting timespec filled. 
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
void TimeUtil::Now(struct timespec *val)
{
    SET_DEBUG_STACK;
    if (!val) return;    // no input given. 

#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    val->tv_sec  = tv.tv_sec;
    val->tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, val);
#endif
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Difference
 *
 * Description : Time difference between two struct timespecs
 *
 * Inputs : none
 *
 * Returns : time in seconds
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
double TimeUtil::Difference(const struct timespec &one, 
			    const struct timespec &two)
{
    double rv = 0.0;
    SET_DEBUG_STACK;

    rv  = (double)one.tv_sec + 1.0e-9 * (double) one.tv_nsec;
    rv -= (double)two.tv_sec + 1.0e-9 * (double) two.tv_nsec;

    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Difference
 *
 * Description : Time difference between the last time touched and now. 
 *
 * Inputs : none
 *
 * Returns : time in seconds
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
double TimeUtil::Difference(void)
{
    SET_DEBUG_STACK;
    struct timespec now;

    Now(&now);
    SET_DEBUG_STACK;
    return Difference(now, fLastTouched);
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
void TimeUtil::Add(time_t seconds)
{
    SET_DEBUG_STACK;
    fUserTime.tv_sec += seconds;
}
/**
 ******************************************************************
 *
 * Function Name : MidnightGMT
 *
 * Description : returns true when very close to Midnight GMT. 
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
bool TimeUtil::MidnightGMT(void)
{
    SET_DEBUG_STACK;
    bool      rv = false;
    time_t    current;
    struct tm *gmt;

    /* only good to the second. */
    current = time(NULL);
    gmt = gmtime(&current);
    
    rv = ((abs(gmt->tm_hour-23)<1) && 
	  (abs(gmt->tm_min-59)<1) && (abs(gmt->tm_sec-59)<=1));

    return rv;
}
bool TimeUtil::LocalTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    SET_DEBUG_STACK;
    bool      rv = false;
    time_t    current;
    struct tm *loc;

    /* only good to the second. */
    current = time(NULL);
    loc = localtime(&current);
    
    rv = ((abs(loc->tm_hour-hour)<1) && 
	  (abs(loc->tm_min-min)<1) && (abs(loc->tm_sec-sec)<=1));

    return rv;
}
