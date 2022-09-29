/**
 ******************************************************************
 *
 * Module Name : precisetime.cpp
 *
 * Author/Date : C.B. Lirakis / 31-Dec-03
 *
 * Description :
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
#include <ostream>
#include <fstream>
#ifdef __APPLE__
#  include <sys/time.h>
#endif


/// Local Includes.
#include "precisetime.hh"  // Function prototypes for this function. 
#include "debug.h"        // Primitive stack dump tools. 
// Number of nanoseconds in second. 
const long NanoSeconds = 1000000000;

/**
 ******************************************************************
 *
 * Function Name : Empty Constructor
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
PreciseTime::PreciseTime() : CObject()
{
    SET_DEBUG_STACK;
    memset(&now, 0, sizeof(timespec));
    SetName("Precise Time");
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : PreciseTime copy constructor
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
PreciseTime::PreciseTime (const PreciseTime &pt) : CObject()
{
    SET_DEBUG_STACK;
    now = pt.now;
    SetName("Precise Time");
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : PreciseTime constructor, timespec in
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
PreciseTime::PreciseTime (const struct timespec &pt) : CObject()
{
    SET_DEBUG_STACK;
    now = pt;
    SetName("Precise Time");
}
/**
 ******************************************************************
 *
 * Function Name : PreciseTime constructor
 * 
 * Description : sec and nanoseconds in.
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
PreciseTime::PreciseTime (long Seconds, long nanoSeconds) : CObject()
{
    SET_DEBUG_STACK;
    now.tv_sec  = Seconds;
    now.tv_nsec = nanoSeconds;
    SetName("Precise Time");
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name :PreciseTime::Now()
 *
 * Description : Get to machine precision the current time. 
 *
 * Inputs : none
 *
 * Returns : struct timespec
 *
 * Error Conditions :
 *
 *******************************************************************
 */
struct timespec PreciseTime::Now()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    now.tv_sec  = tv.tv_sec;
    now.tv_nsec = tv.tv_usec * 1000; 
#else
    SetError(clock_gettime(CLOCK_REALTIME, &now), __LINE__);
#endif
    SET_DEBUG_STACK;
    return now;
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
PreciseTime PreciseTime::operator = (const PreciseTime &arg2)
{
    SET_DEBUG_STACK;
    // Gotta take into account plus and minus entry values. DAMN!
    now = arg2.now; 
    SET_DEBUG_STACK;
    return *this;
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
PreciseTime PreciseTime::operator +=(const PreciseTime &arg2)
{
    SET_DEBUG_STACK;
   // Gotta take into account plus and minus entry values. DAMN!
    now.tv_sec  += arg2.now.tv_sec; 
    now.tv_nsec += arg2.now.tv_nsec;

    // both arguments are positive. 
    if (now.tv_nsec > NanoSeconds)
    {
	now.tv_nsec -= NanoSeconds;
	now.tv_sec++;
    }

    // Case where result is negative. 
    if (now.tv_sec < 0)
    {
	if (now.tv_nsec < -NanoSeconds)
	{
	    now.tv_nsec += NanoSeconds;
	    now.tv_sec--;
	}
    }
    else  // result is positive.
    {
	if (now.tv_nsec < 0)
	{
	    now.tv_nsec += NanoSeconds;
	    now.tv_sec--;
	}
    }
    SET_DEBUG_STACK;
    return *this;
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
PreciseTime PreciseTime::operator *=(const double val)
{
    SET_DEBUG_STACK;
    double temp = now.tv_nsec;
    temp *= 1.0E-9;
    temp += now.tv_sec;
    temp *= val;
    if (temp > 0)
    {
        now.tv_sec = (time_t) floor(temp);
	temp -= ((double)now.tv_sec);
	now.tv_nsec = (long int) floor(temp*NanoSeconds);
    }
    else
    {
        now.tv_sec = (time_t) ceil(temp);
	temp -= ((double)now.tv_sec);
	now.tv_nsec = (long int) ceil(temp*NanoSeconds);
    }

    SET_DEBUG_STACK;
    return *this;
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
double PreciseTime::DiffDoubleNow ()
{
    SET_DEBUG_STACK;
    PreciseTime Current;
    Current.Now();
    Current -= *this;
    SET_DEBUG_STACK;
   return Current.GetDouble();
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
struct tm PreciseTime::GetTM()
{
    SET_DEBUG_STACK;
    struct tm rv;
    Now();
    gmtime_r( (time_t*) &now.tv_sec, &rv);
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : operator << overload
 *
 * Description : Dump data
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
ostream& operator<<(ostream& os, const PreciseTime &TimeIn)
{
    SET_DEBUG_STACK;
    struct tm *now;
    char       dstring[128];
    time_t     intime = TimeIn.Get().tv_sec;
    now = localtime( &intime);
    strftime( dstring, sizeof(dstring), "PT: %a %F %H:%M:%S", now);
    os << dstring;
    SET_DEBUG_STACK;
    return os;
}
const char* PreciseTime::Format(void) const
{
    SET_DEBUG_STACK;
    struct tm   *now;
    static char dstring[128];
    time_t      intime = this->Get().tv_sec;
    now = localtime( &intime);
    memset( dstring, 0, sizeof(dstring));
    strftime( dstring, sizeof(dstring), "PT: %a %F %H:%M:%S", now);
    SET_DEBUG_STACK;
    return dstring;
}
