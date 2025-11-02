/**
 ******************************************************************
 *
 * Module Name : GSA.h
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
#ifndef __GSA_hh_
#define __GSA_hh_

// Active satellites in solution.
class GSA 
{
public:
    GSA(void);
    bool Decode(const char *);
    inline char Mode1(void) const {return fMode1;};
    inline char Mode2(void) const {return fMode2;};
    inline unsigned char ID(int i) const {return fSatellite[i%12];};
    inline const unsigned char* IDS(void) {return fSatellite;};
    inline float PDOP(void) const {return fPDOP;};
    inline float HDOP(void) const {return fHDOP;};
    inline float VDOP(void) const {return fVDOP;};
    inline float TDOP(void) const {return 0.0;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPDOP;};

    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(float)+16*sizeof(char));};

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const GSA &n); 

private:
    float         fPDOP, fHDOP, fVDOP, fSpace;
    char          fMode1, fMode2;
    char          fSpace1, fSpace2; // Placeholders
    unsigned char fSatellite[12]; 
};

#endif
