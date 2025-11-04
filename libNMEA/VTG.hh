/**
 ******************************************************************
 *
 * Module Name : VTG.h
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
 * https://receiverhelp.trimble.com/alloy-gnss/en-us/nmea0183-messages-vtg.html?Highlight=VTG
 * https://docs.fixposition.com/fd/nmea-gp-vtg
 *
 *******************************************************************
 */
#ifndef __VTG_hh_
#define __VTG_hh_

/// VTG documentation here. 
class VTG
{
public:
    /*!
     * Constructor for NMEA VTG message. 
     */
    VTG(void);
    ~VTG(void);
    enum MODES{kAUTONOMOUS=0, kDIFFERENTIAL, kDR, kMANUAL, kSIMULATION, kNONE};

    bool Decode(const char *);
    inline float True(void)  const {return fTrue;};
    inline void  SetTrue(float v)  {fTrue = v;};
    inline float Mag(void)   const {return fMagnetic;};
    inline void  SetMag(float v)   {fMagnetic = v;};
    inline float Knots(void) const {return fSpeedKnots;};
    void  SetKnots(float v);
    inline float KPH(void)   const {return fSpeedKPH;};
    void  SetKPH(float v);
    inline int   Mode(void)  const {return (int) fMode;};
    inline void  SetMode(MODES v)  { fMode = v;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fTrue;};

    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(float)+4*sizeof(char));};

    string Encode(void);
    std::string ModeStr(void) const;

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const VTG &n); 

private:

    float fTrue;      // Course True - Radians
    float fMagnetic;  // course magnetic - Radians
    float fSpeedKnots;
    float fSpeedKPH; 
    char  fMode;      // Autonomous, Differential, Estimated
    char  fSpace[3];  // round out to even 4 byte boundry

};


#endif
