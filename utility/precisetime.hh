/**
 ******************************************************************
 *
 * Module Name : precisiontime.h
 *
 * Author/Date : C.B. Lirakis / 31-Dec-03
 *
 * Description : machine independent way of getting timespec filled
 * at varying degrees of precision. 
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
#ifndef __PRECISETIME_h_
#define __PRECISETIME_h_
#  include <time.h>
#  include <string.h>
#  include "CObject.hh"
/// Various useful constants. 
const struct timespec Zero       = {0,0};
const struct timespec One_Minute = {60,0};
const struct timespec One_Hour   = {3600,0};
const struct timespec One_Day    = {24*3600,0};
const struct timespec One_Week   = {7*24*3600,0};

class PreciseTime : public CObject
{
    /// overload << to print out data. 
    friend ostream& operator << (ostream &os, const PreciseTime &TimeIn);

 public:
    PreciseTime();
    PreciseTime (const PreciseTime &pt);
    PreciseTime (const struct timespec &pt);
    PreciseTime (long Seconds, long nanoSecconds);

    /** 
     * This is a layer on top of clock_gettime. Using whatever method I have 
     * available, I will maintain the clock time on the PC to the 
     * best of my ability. This may even change from platform to platform. 
     * This call will then take all the corrections available into account and
     * return a time good to what I hope is 1 uS. Plese note that this takes 
     * advantage of the POSIX RT extentions. 
     */
    inline struct timespec Get() const {return now;};

    /**
     * Update the private data to the current time 
     * and return the timespec value too.
     */
    struct timespec Now();
    /// Get TM structure.
    struct tm GetTM();

    /// Access the private data for number of seconds. 
    inline long GetSeconds() const {return now.tv_sec;};

    /// Access the private data for number of nano-seconds. 
    inline long GetNanoSeconds() const {return now.tv_nsec;};

    /// Negate the current value. 
    inline PreciseTime Negate() const 
	{PreciseTime A(now.tv_sec*-1, now.tv_nsec*-1);return A;};

    /// Set current value using a timespec. 
    inline void Set(struct timespec &val) {now = val;};

    /// Set the value using two longs. 
    inline void Set(unsigned sec, unsigned nsec)
	{now.tv_sec = sec; now.tv_nsec = nsec;};

    /// Zero the current time value. 
    inline void Zero() {memset(&now, 0, sizeof(struct timespec));};

    /// Return the double time value in seconds. 
    inline double GetDouble() const 
	{double rv = now.tv_nsec; rv /= 1.0E9; rv+=now.tv_sec; return rv;};


    /// Return the double value between this value and current time!
    double DiffDoubleNow ();

    // Operator methods.
    PreciseTime operator = (const PreciseTime &TimeIn);
    PreciseTime operator +=(const PreciseTime &TimeIn);
    PreciseTime operator  +(const PreciseTime &TimeIn) const 
	{PreciseTime rv(*this); rv += TimeIn; return rv;};
    PreciseTime operator -=(const PreciseTime &TimeIn)
	{*this += TimeIn.Negate();return *this;};
    PreciseTime operator  -(const PreciseTime &TimeIn) const
	{PreciseTime rv(*this); rv -= TimeIn; return rv;};

    PreciseTime operator *=(const double val);
    PreciseTime operator  *(const double val) const
	{ PreciseTime rv(*this); rv *= val; return rv;};
    PreciseTime operator /=(const double val) { return (*this * 1.0/val);};
    PreciseTime operator  /(const double val) const
	{ PreciseTime rc(*this); rc /= val; return rc;};

    const char *Format(void) const;

 private:
    struct timespec now;
};
#endif
