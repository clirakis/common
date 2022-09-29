/**
 ******************************************************************
 *
 * Module Name : SolutionStatus.hh
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Created, moved out of lassen.hh, 
 * This contains data from packet 0x6D.
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
#ifndef __SOLUTIONSTATUS_hh_
#define __SOLUTIONSTATUS_hh_
#include "TimeStamp.hh"       // To mark when data is acquired. 
#include "TSIP_Constants.hh" 
/**
 * Solution Status, Contains data from 0x6D response packet
 */
class SolutionStatus : public DataTimeStamp
{
public:
    /// Constructor for solution status
    SolutionStatus();
    /// Desctructor for solution status
    ~SolutionStatus();
    void Clear();
    /*! Fill the values. */
    void Fill(unsigned char solution, unsigned char NSV, float PDOP, 
	      float HDOP, float VDOP, float TDOP);

    /* Access Data */
    /*! Return the number of satellites in view. */
    inline unsigned char NSV(void) const {return fNSV;};
    /*! return packed solution status information */
    inline unsigned char Solution(void) const {return fSolution;};
    /*! Return PRN associated with index i. */
    inline unsigned char PRN(unsigned char i) const 
	{if (i<MAXPRNCOUNT) return fPRN[i]; else return 0;};
    /*! return pointer to PRN array. */
    inline const unsigned char* PRN(void) const {return fPRN;};
    /*! Return Position Dilution of Precision. */
    inline float PDOP(void) const {return fPDOP;};
    /*! Return horizontal dilution of precision. */
    inline float HDOP(void) const {return fHDOP;};
    /*! Return Vertical Dilution of Precision. */
    inline float VDOP(void) const {return fVDOP;};
    /*! Return Time Dilution of Precision. */
    inline float TDOP(void) const {return fTDOP;};
    /*! unpack the solution type. 
     * TSIP Reference Manual, page 247. 
     * Table 3-59 
     * Byte 0, Fix Mode
     * Bit #   Meaning
     * -----   -------
     * 0-2     2D or 3D Mode:
     *             0: Unknown
     *             1: 0D
     *             2: 2D Clock Hold
     *             3: 2D
     *             4: 3D
     *             5: Overdetermined Clock
     *             6: DGPS Reference Station
     * 3        Auto or Manual Mode:
     *             0: Auto 
     *             1: Manual
     * 4-7      Number of satellites
     */
    unsigned char Mode(void) const;
    /*! 
     * unpack the solution selection Auto or Manual. 
     * true - Auto, false - manual
     * Correct in both TSIP reference manual and lassen-sk8 manual.
     */
    inline bool Auto(void) const {return ((fSolution&0x08)==0);};

    /* Set Data */
    /*! Set the number of satellites in view. */
    inline void NSV(unsigned char v) {fNSV=v;};
    /*! Set the solution status value. */
    inline void Solution(unsigned char v) {fSolution=v;};
    /*! Set the PRN value at index i.  */
    inline void PRN(unsigned char v, unsigned char i) 
	{if(i<MAXPRNCOUNT) fPRN[i]=v;};
    /*! Set the Position Dilution of Precision. */
    inline void PDOP(float v) {fPDOP=v;};
    /*! Set the horizontal dilution of precision. */
    inline void HDOP(float v) {fHDOP=v;};
    /*! Set the Vertical Dilution of Precision. */
    inline void VDOP(float v) {fVDOP=v;};
    /*! Set the Time Dilution of Precision. */
    inline void TDOP(float v) {fTDOP=v;};

    /*! Get a pointer to the beginning of the data storage. */
    inline void* DataPointer(void) {return (void*)&fPDOP;};
    /*! Return the overall data size for the structure. */
    inline static size_t DataSize(void) {
	return (4*sizeof(float) + (MAXPRNCOUNT+2)*sizeof(char));
		}

    /*! operator overload to output contents of class for inspection
     * this data is in character format. 
     */
    friend ostream& operator<<(ostream& output, const SolutionStatus &n); 


private:

    /*! Precision Dilution of Precision  */
    float fPDOP; 
    /*! Horizontal Dilution of Precision */
    float fHDOP;
    /*! Vertical Dilution of Precision   */
    float fVDOP;
    /*! Time Dilution of Precision       */
    float fTDOP;
    /*! PRN's participating in view.     */
    unsigned char fPRN[MAXPRNCOUNT];  
    /*!
     * Solution status - derived from mode, Bit 3: 0 - Auto, 1-Manual
     * Bits 0:2 3 - 2D, 4 - 3D.
     * This is byte 0 of the 0x6D report packet. 
     */
    unsigned char fSolution; 

    /*! Number of satellites in view, derived from mode */
    unsigned char fNSV;  
};
#endif
