/**
 ******************************************************************
 *
 * Module Name : TSIPVelocity.hh
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Pulled out of lassen, class instead of structure. 
 * Holds the data associated with the Velocity data, 0x56
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
#ifndef __VELOCITY_hh_
#define __VELOCITY_hh_
#include "TimeStamp.hh"       // To mark when data is acquired. 


/// Velocity documentation here. 
class TSIPVelocity : public DataTimeStamp
{
public:
    /// Default Constructor
    TSIPVelocity(void);
    /// Default destructor
    ~TSIPVelocity(void);
    /*! Set all the private variables to zero. */
    void Clear(void);

    /* Set private data. */
    inline void East(float v)  {fEast = v;};
    /*! Velocity North in M/S */
    inline void North(float v) {fNorth = v;};
    /*! Velocity up in M/S*/
    inline void Up(float v)    {fUp = v;};
    /*! Clock Bias rate Seconds/Second */
    inline void ClockBiasRate(float v) {fClkBiasRate = v;};
    /*! Time of data*/
    inline void Seconds(float v)       {fSec = v;};

    /* Get private data. */
    /*! Velocity east in M/S */
    inline float East(void) const  {return fEast;};
    /*! Velocity North in M/S */
    inline float North(void) const {return fNorth;};
    /*! Velocity up in M/S*/
    inline float Up(void) const    {return fUp;};
    /*! Clock Bias rate Seconds/Second */
    inline float ClockBiasRate(void) const {return fClkBiasRate;};
    /*! Time of data*/
    inline float Seconds(void) const {return fSec;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fEast;};
    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) { return (5*sizeof(float));};


    /*! Friend operator<< for outputing a formatted string of Velocity. */
    friend ostream& operator<<(ostream& output, const TSIPVelocity &n); 

private:

    /// Velocity East in meters per second
    float fEast;
    /// Velocity North in meters per second
    float fNorth;
    /// Velocity up in meters per second
    float fUp;
    /// Clock bias rate in seconds per second
    float fClkBiasRate;
    /// Time in seconds when this information was established
    float fSec; 
};
#endif
