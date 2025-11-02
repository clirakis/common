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
 *
 *******************************************************************
 */
#ifndef __VTG_hh_
#define __VTG_hh_

/// VTG documentation here. 
class VTG
{
public:
    VTG(void);
    ~VTG(void);

    bool Decode(const char *);
    inline float True(void)  const {return fTrue;};
    inline float Mag(void)   const {return fMagnetic;};
    inline float Knots(void) const {return fSpeedKnots;};
    inline float KPH(void)   const {return fSpeedKPH;};
    inline int   Mode(void)  const {return (int) fMode;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fTrue;};

    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(float)+4*sizeof(char));};

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const VTG &n); 

private:
    float fTrue;      // Course True
    float fMagnetic;  // course magnetic
    float fSpeedKnots;
    float fSpeedKPH; 
    char  fMode;      // Autonomous, Differential, Estimated
    char  fSpace[3];  // round out to even 4 byte boundry
};


#endif
