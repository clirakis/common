/**
 ******************************************************************
 *
 * Module Name : 
 *
 * Author/Date : C.B. Lirakis / 06-Jul-03
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
#ifndef __TIMESTAMP_hh_
#define __TIMESTAMP_hh_

/* This is a unix include. Make sure RT flag is set*/
#include <time.h> 

/**
 * Make a super class that will tell me when data has been
 * last updated, relative to local computer clock time. 
 */
class DataTimeStamp {
public:
    /**
     * Constructor in which we specify what constitutes a new reading. 
     */
    DataTimeStamp(const double delta = 0.5);
    /**
     * Constructor in which we provide another DataTimeStamp as input. 
     */
    DataTimeStamp(DataTimeStamp &in);
    virtual ~DataTimeStamp();
    /**
     * Return the diffence since last update.
     * Sets threshold crossed if true.
     */
    double Now(void);
    /*
     * Set LastTime - ie: Stamp it!
     */
    void Stamp(void);
    /**
     * Get dt
     */
    double DT(void) const;
    /*! is the data past threshold? */
    inline bool IsNew(void) const {return fCrossedThreshold;};
    /*! Has the data time stamp pass the threshold with respect to 
     * the current time? */
    bool AsComparedToNow(void) const;
    /*! Return the last time stamp value. */
    inline const struct timespec* LastTime(void) const {return &fLast;};

    // Return timestamp as a double. 
    double DLastTime(void) const;

    /*! Enable a more friendly way of printing the contents of the class. */
    friend std::ostream& operator<<(std::ostream& output, const DataTimeStamp &n);

private:
    /*! Threshold in seconds for when the data is marked as old. */
    double fThreshold;
    /*! Difference between current and last call.*/
    double fdt; 
    /*! Time of Last Call */
    struct timespec fLast;
    /*! Exceeded change   */
    bool fCrossedThreshold;

};
#endif
