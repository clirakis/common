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
    /*!
     * Mode1  (M)anual or (A)utomatic.
     */
    inline char Mode1(void) const {return fMode1;};
    void SetMode1(char c) {if ((c=='M') || (c=='A')) fMode1 = c;};
    /*!
     * Mode 2 Fix Type:
     *     1 - not available
     *     2 - 2D
     *     3 - 3D
     */
    inline char Mode2(void) const {return fMode2;};
    inline void SetMode2(char c) {if((c>0) && (c<4)) fMode2 = c;};
    /*!
     * for satellite i, return the numerical id. 
     */
    inline unsigned char ID(int i) const {return fSatellite[i%12];};
    inline void SetID(int i, char ID) {fSatellite[i%12]=ID;};
    /*!
     * Get the full ID array!
     */
    inline const unsigned char* IDS(void) {return fSatellite;};
    /*!
     * get the PDOP
     */
    inline float PDOP(void) const {return fPDOP;};
    inline void  SetPDOP(float v) {fPDOP = v;};
    /*!
     * get the HDOP
     */
    inline float HDOP(void) const {return fHDOP;};
    inline void  SetHDOP(float v) {fHDOP = v;};
    /*!
     * Get the VDOP
     */
    inline float VDOP(void) const {return fVDOP;};
    inline void  SetVDOP(float v) {fVDOP = v;};
    /*!
     * Get the TDOP
     */
    inline float TDOP(void) const {return 0.0;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPDOP;};

    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) 
	{return (4*sizeof(float)+16*sizeof(char));};

    /*!
     * Encode the data into a NMEA string. 
     */
    std::string Encode(void);

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
