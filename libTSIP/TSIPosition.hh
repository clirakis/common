/**
 ******************************************************************
 *
 * Module Name : TSIPosition.hh
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Position class for TSIP packets, This contains
 * all the data associated with position packets, 0x4A - single precision
 * position, or 0x8A double precision position. 
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
#ifndef __TSIPosition_hh_
#define __TSIPosition_hh_
#include "TimeStamp.hh"       // To mark when data is acquired. 


/// TSIPosition documentation here. 
class TSIPosition : public DataTimeStamp
{
public:
    /// Default Constructor
    TSIPosition();
    /// Default destructor
    ~TSIPosition();
    /*! Zero all data */
    void Clear(void);

    /* Methods to set data. */
    /*! Set the data valid or not. */
    inline void Valid(bool v) {fValid=v;};
    /*! Set the Latitude, radians */
    inline void Latitude(double v)  {fLatitude = v;};
    /*! Set the Longitude in radians. */
    inline void Longitude(double v) {fLongitude = v;};
    /*! Set the Altitude, meters. */
    inline void Altitude(double v)  {fAltitude = v;};
    /*! Set the clock bias, seconds */
    inline void ClockBias(double v) {fClk = v;};
    /*! set the fix time, seconds*/
    inline void Seconds(double v)   {fSec = v;};


    /* Methods to access data. */
    /*! Is data valid? */
    inline bool Valid(void)       const {return fValid;};
    /*! Latitude in radians. */
    inline double Latitude(void)  const {return fLatitude;};
    /*! Longitude in radians. */
    inline double Longitude(void) const {return fLongitude;};
    /*! Altitude. */
    inline double Altitude(void)  const {return fAltitude;};
    /*! Clock bias in sec */
    inline double ClockBias(void) const {return fClk;};
    /*! Time of fix, GPS week, seconds. */
    inline double Seconds(void)   const {return fSec;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fLatitude;};
    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(double)+sizeof(float)+sizeof(bool));};

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const TSIPosition &n); 

private:

    /// Latitude and Longitude in radians.
    double fLatitude, fLongitude; 
    /// Altitude, not sure of reference in meters
    double fAltitude;  
    /// Clock bias data
    double fClk;
    /// Time of Fix. 
    float fSec;
    /// Fix is valid, true/false
    bool   fValid;
};
#endif
