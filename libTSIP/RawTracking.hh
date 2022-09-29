/**
 ******************************************************************
 *
 * Module Name : RawTracking.hh 
 *
 * Author/Date : C.B. Lirakis / 24-Nov-17
 *
 * Description : Raw Tracking Data. Message 0x5C
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
#ifndef __RAWTRACKING_hh_
#define __RAWTRACKING_hh_

#include "TimeStamp.hh"       // To mark when data is acquired. 
#include "TSIP_Constants.hh" 

/**
 * Provides the raw tracking information. This is the data on a satellite
 * by satellite (PRN) basis.
 */
class RawTracking : public DataTimeStamp 
{
public:
    /// Default constructor
    RawTracking();
    /// Constructor with input Raw Tracking class
    RawTracking(const RawTracking &Fp);
    /// Clear the data stored.
    void Clear();
    //struct PackedAzEl   CompactAzEl();
    //struct PackedSignal CompactS();

    /*! Set one Raw Tracking equal to another. */
    RawTracking &operator =(const RawTracking &Fp);

    /* ---- Access the data. ---- */
    /*! Is data valid? */
    inline bool Valid(void) const {return fValid;};
    /*! */
    inline float SignalLevel(void) const {return fSignalLevel;};
    /*! Elevation of PRN in radians. */
    inline float Elevation(void) const {return fElevation;};
    /*! Azimuth of PRN in radians. */
    inline float Azimuth(void) const {return fAzimuth;};
    /*! PRN time of last measurement, seconds. */
    inline float GPS_Time(void) const {return fGPS_TimeofLastMeasurement;;};
    /*! PRN of the stored data. */
    inline unsigned char PRN(void) const {return fPRN;};
    /*! Channel Code of PRN */
    inline unsigned char ChannelCode(void) const {return fChannelCode;};
    /*!
     *      0 never acquired
     *      1 acquired
     *      2 re-opened search 
     */
    inline unsigned char AcquisitionFlag(void) const {return fAcquisitionflag;};
    /*!
     *  false -  good ephemeris for this satellite
     * (<4 hours old, good health)
     */
    inline bool EphemerisFlag(void) const {return fEphemerisFlag;};

    /*!
     * false - the last measurement is too old to use for a 
     *         fix computation. 
     */
    inline bool OldMeasurementFlag(void) const {return fOldMeasurementFlag;};
    /*! 
     * Don't have good knowledge of integer millisecond 
     * range to this satellite:
     *     1 msec from sub-frame data collection
     *     2 verified by a bit crossing time
     *     3 verified by a successful position fix
     *     4 suspected msec error 
     */
    inline unsigned char MSecFlag(void) const {return fMSecFlag;};
    /*! 
     *     0 flag not set
     *     1 bad parity
     *     2 bad ephemeris health 
     */
    inline unsigned char BadDataFlag(void) const {return fBadDataFlag;};
    /*!
     *  false - The receiver currently is trying to
     *      collect data from this satellite.
     */
    inline unsigned char DataCollectionFlag(void) const 
	{return fDataCollectionFlag;};


    /* ---- Set the data. ----- */
    /*! Set the data as valid */
    inline void SetValid(void) {fValid = true;};

    /*! Signal Level */
    inline void SignalLevel(float v) {fSignalLevel = v;};
    /*! Elevation in radians. 
     *  Approximate elevation of this satellite above the horizon. 
     *  Updated aboutevery 15 seconds. Used for searchingand 
     *  computing measurementcorrection factors.
     */
    inline void Elevation(float v) {fElevation = v;};
    /*! Azimuth radians. 
     * Approximate azimuth from true north to this satellite. 
     * Updated typically aboutevery 3 to 5 minutes. 
     * Used forcomputing measurement correction factors.
     */
    inline void Azimuth(float v) {fAzimuth = v;};
    /*! Time of last measurement - 
     * Byte 8 - 11 value:
     *          <0 no measurements have been taken
     */
    inline void GPS_TimeofLastMeasurement(float v) 
	{fGPS_TimeofLastMeasurement = v;};
    /*! PRN (0-32) */
    inline void PRN(unsigned char v) {fPRN = v;};
    /*! Channel code - Bit 4-6, channel number, 0-7*/
    inline void ChannelCode(unsigned char v) {fChannelCode = v;};
    /*! Acquisition flag 
     *      0 never acquired
     *      1 acquired
     *      2 re-opened search
     */
    inline void Acquisitionflag(unsigned char v) {fAcquisitionflag = v;};
    /*! Ephermeris Flag 
     *  0 flag not set good ephemeris for this satellite
     * (<4 hours old, good health)
     */
    inline void EphemerisFlag(bool v) {fEphemerisFlag = v;};
    /*!
     *     0 flag not set>0 the last measurement is too old touse for 
     *       a fix computation.
     */
    inline void OldMeasurementFlag(bool v) { fOldMeasurementFlag= v;};
    /*! 
     * Don't have good knowledge of integer millisecond 
     * range to this satellite:
     *     1 msec from sub-frame data collection
     *     2 verified by a bit crossing time
     *     3 verified by a successful position fix
     *     4 suspected msec error 
     */
    inline void MSecFlag(unsigned char v) { fMSecFlag = v;};
    /*!
     *     0 flag not set
     *     1 bad parity
     *     2 bad ephemeris health 
     */
    inline void BadDataFlag(unsigned char v) { fBadDataFlag = v;};
    /*!
     * Byte 23 value:
     *      0 flag not set>0 The receiver currently is trying to
     *      collect data from this satellite.
     */
    inline void DataCollectionFlag( bool v) {fDataCollectionFlag=v;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fSignalLevel;};
    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{ return (4*sizeof(float) + 6*sizeof(char) + 4*sizeof(bool));};


    /*! Friend operator<< for outputing a formatted string of RawTracking. */
    friend ostream& operator<<(ostream& output, const RawTracking &n); 

private:

    /// Same as packet 47                  
    float fSignalLevel; 
    /// Satellite elevation in radians.
    float fElevation;
    /// Satellite Azimuth in radians
    float fAzimuth;                      // Radians
    /// Time of last measurement in seconds. 
    float fGPS_TimeofLastMeasurement;
    /*! PRN this data is assocated with,  range 0-32. */
    unsigned char  fPRN; 
    /*! Channel code */
    unsigned char  fChannelCode;
    /*! Acquisition flag. */
    unsigned char  fAcquisitionflag;
    /*! Ephermeris flag */
    unsigned char  fEphemerisFlag;
    /*! 
     * Byte 20 value:
     *     0 flag not set>0 the last measurement is too old touse for 
     *       a fix computation.
     */
    bool  fOldMeasurementFlag; 
    /*! 
     * Byte 21 value:
     * Don't have good knowledge of integer millisecond 
     * range to this satellite:
     *     1 msec from sub-frame data collection
     *     2 verified by a bit crossing time
     *     3 verified by a successful position fix
     *     4 suspected msec error
     */
    unsigned char  fMSecFlag;
    /*!
     * Byte 22 value:
     *     0 flag not set
     *     1 bad parity
     *     2 bad ephemeris health
     */
    unsigned char  fBadDataFlag;

    /*! 
     * Byte 23 value:
     *     0 flag not set>0 The receiver currently is trying to
     *       collect data from this satellite.
     */
    bool fDataCollectionFlag;
    /*
     * Is data valid? */
    bool fValid;
    bool roundItoutForEvenNumberOfBytes;
};

#endif
