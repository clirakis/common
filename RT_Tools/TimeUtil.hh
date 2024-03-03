/**
 ******************************************************************
 *
 * Module Name : TimeUtil.hh
 *
 * Author/Date : C.B. Lirakis / 28-Feb-24
 *
 * Description : a collection of time utilities that should be machine 
 *               independent. 
 *               Replaces the old PreciseTime that I made awhile ago. 
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
#ifndef __TIMEUTIL_hh_
#define __TIMEUTIL_hh_
#    include <time.h>
#    include <stdint.h>

/// TimeUtil documentation here. 
class TimeUtil {
public:
    /// Default Constructor
    TimeUtil(void);
    /// Default destructor
    ~TimeUtil(void);

    /*!
     * Description: 
     *   System independent way of getting struct timespec. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    void Now(struct timespec *val);

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    inline void Update(void)                        {Now(&fLastTouched);};
    inline void Update(const struct timespec &user) {fUserTime = user;};
    inline void UserUpdate(void)                    {Now(&fUserTime);};
    inline void SetDifference(double val)           {fDelta = val;};
    inline bool IsOver(void)                  {return (Difference()>fDelta);};


    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    double Difference(const struct timespec &one, 
		      const struct timespec &two);
    /*!
     * Description: 
     *   Return the difference between the current time
     *   and the stored last touched time.
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    double Difference(void);

    /*!
     * Description: 
     *   Add some amount of seconds onto the fUserTime. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    void Add(time_t seconds); 

    /*!
     * Description: 
     *   return true when close to midnight. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    bool MidnightGMT(void);

    bool LocalTime(uint8_t hour, uint8_t min, uint8_t sec);


private:
    struct timespec fLastTouched;
    struct timespec fUserTime;
    double fDelta;             // seconds to elapse 
};
#endif
