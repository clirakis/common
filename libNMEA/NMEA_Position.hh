/**
 ******************************************************************
 *
 * Module Name : NMEA_Position.h
 *
 * Author/Date : C.B. Lirakis/02-Nov-25
 *
 * Description : This is the base class for storing position information
 *               from NMEA messages like GGA. 
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __NMEA_POSITION_hh_
#define __NMEA_POSITION_hh_
#  include <stdint.h>
#  include <time.h>

/*!
 * NMEA_Position - all the information associated with position information
 */
class NMEA_Position 
{
public:
    /*! Constructor */
    NMEA_Position(void);
    /*! Return fix latitude in radians. */
    inline float  Latitude(void)  const {return fLatitude;};
    /*! Return fix longitude in radians */
    inline float  Longitude(void) const {return fLongitude;};
    /*! 
     * Epoch Seconds 
     */
    inline time_t Seconds(void)   const {return fSeconds;};
    /*! Milliseconds on epoch */
    inline float  Milli(void)     const {return fMilliseconds;};
    /*! UTC of current day seconds */
    inline time_t UTC(void)       const {return fUTC;};

    /*! Get PC time at time of receipt of message */
    inline struct timespec PCTime(void) const {return fPCTime;};

    /* THINGS USED to relay in shared memory. --------------------- */
    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPCTime;};
    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (sizeof(struct timespec)+3*sizeof(float)+2*sizeof(time_t));};

    // Helper funcitons to make this more generic
    inline void SetPCTime(struct timespec &ts) {fPCTime = ts;};
    inline void SetLatitude(float val)         {fLatitude = val;};
    inline void SetLongitude(float val)        {fLongitude = val;};
    inline void SetTime(time_t val)            {fSeconds = val;};
    inline void SetUTC(time_t val)             {fUTC = val;};
    inline void SetMilliseconds(float val)     {fMilliseconds = val;};

    /*! 
     * operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const NMEA_Position &n); 

protected:
    struct timespec fPCTime;
    float  fLatitude, fLongitude; // In radians. 
    time_t fSeconds;              // epoch if decoded correctly
    time_t fUTC;                  // seconds in current day, UTC
    float  fMilliseconds;         // ms on time of fix
};
#endif
