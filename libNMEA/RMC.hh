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
#include "NMEA_Position.hh"

/*!
 * RMC Recommended minimum information class
 */
class RMC : public NMEA_Position
{
public:
    RMC(void);
    bool Decode(const char *);
    inline float Speed(void)  const {return fSpeed;};
    inline float CMG(void)    const {return fCMG;};
    inline float MagVar(void) const {return fMagVariation;};
    inline char  Mode(void)   const {return fMode;};
    inline float Delta(void)  const {return fDelta;};

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
