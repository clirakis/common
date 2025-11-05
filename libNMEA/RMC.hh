/**
 ******************************************************************
 *
 * Module Name : RMC.h
 *
 * Author/Date : C.B. Lirakis / 23-May-21
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
#ifndef __RMC_hh_
#define __RMC_hh_
#  include <string>
#  include "NMEA_Position.hh"
#  include "tools.h"
/*!
 * RMC Recommended minimum information class
 */
class RMC : public NMEA_Position
{
public:
    /*!
     * RMC constructor
     */
    RMC(void);
    /*!
     * Given a RMC NMEA string, decode it and put it into the structure 
     * elements. 
     */
    bool Decode(const char *);
    /*!
     * Speed in Knots
     */
    inline float Speed(void)  const {return fSpeed;};
    inline void SetSpeed(float v) {fSpeed = v;};

    /*!
     * Course made good in degrees.
     */
    inline float CMG(void)    const {return fCMG * RadToDeg;};
    inline void  SetCMG(float v) {fCMG = v * DegToRad;};
    /*! 
     * Magnetic variation in degrees 
     */
    inline float MagVar(void) const {return fMagVariation * RadToDeg;};
    inline void SetMagVar(float v) {fMagVariation = v*DegToRad;};

    /*!
     * Mode:
     */
    inline char  Mode(void)   const {return fMode;};
    /*!
     * time difference between fix and PC time of fix. 
     */
    inline float Delta(void)  const {return fDelta;};

    std::string Encode(void);

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPCTime;};

    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(float)+4*sizeof(char) + NMEA_Position::DataSize());};

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const RMC &n); 

private:
    /** difference between PC time and GPS time  in seconds */
    float  fDelta;
    /**
     * Speed in knots
     */
    float  fSpeed;
    /**
     * Course made good
     */
    float  fCMG; 
    /**
     * Magnetic Variation. 
     */
    float  fMagVariation;
    /**
     * FIX MODE
     */
    char   fMode;
    char   fPlaceholder[3];
};

#endif
