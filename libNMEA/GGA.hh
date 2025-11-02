/**
 ******************************************************************
 *
 * Module Name : GGA.h
 *
 * Author/Date : C.B. Lirakis / 02-Nov-25
 *
 * Description : GGA Message decoding and encoding
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
#ifndef __GGA_hh_
#define __GGA_hh_

#include "NMEA_Position.hh"

/// GGA documentation here. 
class GGA : public NMEA_Position
{
public:
    GGA(void);
    bool Decode(const char *);
    /*!
     * Height of geoid (mean sea level) above WGS84 ellipsoid
     */
    inline float Geoid(void)      const {return fGeoidheight;};
    /*!
     * altitude in meters above MSL (geoid)
     */
    inline float Altitude(void)   const {return fAltitude;};
    /*! 
     * Fix Indicator made up of:
     *   0 - Invalid
     *   1 GNSS fix (SPS)
     *   2 DGPS fix
     *   3 PPS  fix
     *   4 RTK
     *   5 - UNASSIGNED
     *   6 - DR
     *   7 - Manual input mode
     *   8 - Simulation Mode
     */
    inline char  Fix(void)        const {return fFixIndicator;};
    /**
     * Number of satellites in used in fix. 
     */
    inline char  Satellites(void) const {return fSatellites;};
    /**
     * Horizontal dilution of position
     */
    inline float HDOP(void)       const {return fHDOP;};

    /* THINGS USED to relay in shared memory. --------------------- */
    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPCTime;};

    /*! 
     * Return the overall data size for the structure. 
     */
    inline static size_t DataSize(void) 
	{return (4*sizeof(uint8_t)+4*sizeof(float) + NMEA_Position::DataSize());};

    inline void SetGeoidHeight(float val) {fGeoidheight = val;};
    inline void SetAltitude(float val)    {fAltitude = val;};
    inline void SetFix(uint8_t val)       {fFixIndicator = val;};
    inline void SetNSat(uint8_t val)      {fSatellites = val;};
    inline void SetHDOP(float val)        {fHDOP = val;};


    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const GGA &n); 

private:
    /** Height of Geoid above mean sea level WGS84 */
    float   fGeoidheight;  
    /** altitude relative to mean geoid */ 
    float   fAltitude;
    /** 
     * Fix Indicator made up of:
     *   0 - Invalid
     *   1 GNSS fix (SPS)
     *   2 DGPS fix
     *   3 PPS  fix
     *   4 RTK
     *   5 - UNASSIGNED
     *   6 - DR
     *   7 - Manual input mode
     *   8 - Simulation Mode
     */
    uint8_t fFixIndicator; 
    uint8_t fSatellites;   // number in fix.
    uint8_t fspace1;
    uint8_t fspace2;
    float   fHDOP;
};
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

#endif
