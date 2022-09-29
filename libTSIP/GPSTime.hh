/**
 ******************************************************************
 *
 * Module Name : GPSTime.hh
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Store and access all GPSTime data, 0x41
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
#ifndef __MODULE_h_
#define __MODULE_h_
#include "TimeStamp.hh"       // To mark when data is acquired. 

/// GPSTime documentation here. 
class GPSTime : public DataTimeStamp
{
public:
    /// Default Constructor
    GPSTime();
    /// Default destructor
    ~GPSTime();

    /*! Reset all variables to zero. */
    void Clear(void);

    /* Methods to set data. */
    /*! Set the GPS time of week. */
    inline void GPSTimeOfWeek(float v)     {fTimeOfWeek = v;};
    /*! Set the Extended GPS week. */
    inline void ExtendedGPSWeek(short v)   {fExtendedWeek = v;};
    /*! Set the UTC Delta seconds. */
    inline void UTC_Delta(float v)         {fUTC_Delta = v;};
    /*! Set the PC time at acquisition */
    inline void PCTime(struct timespec *v) {fPCTime = *v;};
    /*! Set the delta between the GPS and PC. */
    inline void Delta(double v)            {fDelta = v;};
    /*! Mark PCTime with the current computer clock value */
    void SetPCTime(void);
    /*! Compute delta with the times provided. */
    void SetDelta(void);
    /*! Check bounds, return true if bounds are exeeded. */
    bool CheckBounds(void);

    /* Methods to get data */
    /*! GPS Time of week, seconds */
    inline float GPSTimeOfWeek(void)    const {return fTimeOfWeek;};
    /*! Extended GPS week data. */
    inline short ExtendedGPSWeek(void)  const {return fExtendedWeek;};
    /*! Delta between GPS and UTC */
    inline float UTC_Delta(void)        const {return fUTC_Delta;};
    /*! PC Time at acquisition */
    inline struct timespec PCTime(void) const {return fPCTime;};
    /*! Delta seconds between GPS and PC */
    inline double Delta(void)           const {return fDelta;};
    /*! Convert GPS Time to timespec */
    struct timespec DateFromGPSTime(void) const;

    /*! Get a pointer to the beginning of the data storage. */
    inline const void* DataPointer(void) const 
	{return (const void*)&fTimeOfWeek;};
    /*! Return the overall data size for the structure. */
    static inline size_t Size(void) {
	return (2*sizeof(float) + sizeof(short) + sizeof(struct timespec) + 
		sizeof(double));}

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const GPSTime &n); 

private:
    /*! GPS Time of week */
    float fTimeOfWeek;
    /*! Extended GPS week number */
    short fExtendedWeek;
    /*! GPS/UTC offset in seconds */
    float fUTC_Delta;
    /*! Time that this data was acquired, PC clock */
    struct timespec fPCTime;
    /*! delta between the GPS and PC */
    double fDelta;
};
#endif
