/**
 ******************************************************************
 *
 * Module Name : WFMPRE.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : WFMPRE, waveform prefix data. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual, page 320
 * 
 *******************************************************************
 */
#ifndef __WFMPRE_h_
#define __WFMPRE_h_
#  include <string>
#  include "CObject.hh"
#  include "DSA602_Types.hh"
#  include "Units.hh"

class WFMPRE : public CObject
{
public:

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    WFMPRE(const char *val=NULL);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    ~WFMPRE(void);

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    bool Update(void);

    /*!
     * WFMPRE spefic commands. 
     */
    enum COMMANDs { kACSTATE, kBIT, kBNFMT, kBYTE, kBYTE_ORDER, 
		    kCURVE_CHECK, kDATE, kENCODING, kLABEL, kNRPOINT,
		    kPOINT_FORMAT, kTIME, kTSTIME, kWAVEFORM_ID, 
		    kXINCREMENT, kXMULTIPLIER, kXUNIT, kXZERO, 
		    kYMULTIPLIER, kYUNIT, kYZERO, kENDCOUNT};

    static const struct t_Commands WFMPRECommands[kENDCOUNT+1];


    /* ======================================================== */
    /*                     HELPER FUNCTIONS                     */ 
    /* ======================================================== */

    /*!
     * Description: 
     *     This indicates if the waveform was created with Enhanced (true)
     *     Accuracy or normal calibration configuration.    
     *
     * Arguments:
     *   q - qurey or return last quered value
     *
     * returns:
     *    true - ENHANCED
     */
    inline bool ACState(bool q=false) 
	{if(q) Query(kACSTATE); return fACState;};

    /*!
     * Description: 
     *     set how waveform is created with Enhanced (true)
     *     Accuracy or normal calibration configuration.
     *     NOT WORKING    
     *
     * Arguments:
     *   true - Enhanced
     *
     * returns:
     *    true - success
     */
    bool SetACState(bool q=false); 


    /*!
     * Description: 
     *   Query only - returns the number of bits per waveform point. {8,16}
     *   See page 70 on how this is setup and manipulated. 
     *
     * Arguments:
     *   q - perform query or return last known value. 
     *
     * returns:
     *    true = 16 bits, false = 8 bits
     */
    inline bool BITS(bool q=false) {if(q) Query(kBIT);return fBIT;};

    /*!
     * Description: 
     *   QUERY ONLY, returns the Tek cods and formats binary number
     *   format, which is always RI (Right-Justified, twos-complement
     *   integers). 
     *
     * Arguments:
     *   q - perform query or return last known value. 
     *
     * returns:
     *    
     */
    inline bool BN(bool q=false) {if(q) Query(kBNFMT); return fBN;};

    /*!
     * Description: 
     *   QUERY ONLY byte format in data stream. 
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    1 or 2 depending on the value of fByte. 
     *           true = 2 bytes, false = 1 byte
     */
    unsigned char Bytes(bool q=false);

    /*!
     * Description:
     *   QUERY ONLY 
     *   Byte order (litte endian or big endian)
     *   true = MSB, false = LSB
     * 
     * Arguments:
     *    q - if true ask, otherwise use last known value
     *
     * returns:
     *    
     */
    inline bool ByteOrder(bool q=false) 
	{if(q) Query(kBYTE_ORDER);return fBYTor;};

    /*!
     * Description: 
     *   QUERY ONLY
     *   Returns the type of checksum appended to the waveform data. 
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *     0 - CHKsm0, 1 - NONe, 2 - NULL zero checksum
     */
    inline unsigned char CRVchk(bool q=false) 
	{if(q) Query(kCURVE_CHECK);return fCRVchk;};

    /*!
     * Description: 
     *    return the date associated with the waveform.    
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    string date
     */
    inline string &DATE(bool q=false) {if(q) Query(kDATE);return fDate;};

    /*!
     * Description: 
     *    set the date associated with the waveform
     *    to the current computer clock value   
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true on success
     */
    bool SetDATE(void);

    /*!
     * Description: 
     *   Query Only - returns the encoding of the data.
     *   
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    true if binary, false if ASCII
     */
    inline bool Encoding(bool q=false) {if(q) Query(kENCODING);return fENCdg;};

    /*!
     * Description: 
     *   Query the label associated with the trace
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    string assocated with the label.
     */
    inline string Label(bool q=false) {if(q) Query(kLABEL);return fLABel;};

    /*!
     * Description: 
     *   Set the label associated with the trace
     *
     * Arguments:
     *   const char label value
     *
     * returns:
     *    true on success. 
     */
    bool Label(const char *label);

    /*!
     * Description: 
     *   Query the Number of points in the record. 
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    number of points in the waveform record, will be a power of 2
     *    up to {512:32768}
     */
    inline unsigned int NumberPoints(bool q=false) 
	{if(q) Query(kNRPOINT);return fNRpt;};
    /*!
     * Description: 
     *   Set Number of points in the record. 
     *
     * Arguments:
     *   Specify in bit length, {9,15}
     *   Number of points, a power of 2 in the set {512:32768}
     *
     * returns:
     * true on success
     */
    bool SetNumberPoints(unsigned int p); 

    /*!
     * Description: 
     *   Query the point format, one of {ENV,XY,Y}
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    point format, one of {ENV,XY,Y}
     */
    inline PT_TYPES PointFormat(bool q=false) 
	{if(q) Query(kPOINT_FORMAT);return fPTfmt;};

    /*!
     * Description: 
     *   Set the point format, one of {ENV,XY,Y}
     *   where
     *   ENV applies to a YT waveform and is composed of points 
     *   in the form of pt(n)(MAX,MIN)
     *   
     *   XY is gives a list of pt(n)(X,Y)
     *
     *   Y just gives pt(n) no pairs. 
     *
     * Arguments:
     *   point format, one of {ENV,XY,Y}
     *
     * returns:
     *    true on success
     */
    bool PointFormat(PT_TYPES p);

    /*!
     * Description: 
     *   return the time stamp on the waveform
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    string of time associated with waveform
     */
    inline string &Time(bool q=false) {if(q) Query(kTIME);return fTime;};
    /*!
     * Description: 
     *   set the time stamp on the waveform to current clock time
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true if successful
     */
    bool SetTime(void);

    /*!
     * Description: 
     *    QUERY ONLY, return the time between the actual trigger 
     *    point and the waveform sampel identified as 0 seconds   
     *
     * Arguments:
     *    q - if true ask, otherwise use last known value
     *
     * returns:
     *    double value in seconds
     */
    inline double TSTime(bool q=false) {if(q) Query(kTSTIME);return fTSTime;};

    /*!
     * Description: 
     *   QUERY ONLY - return the string associated with the waveform id
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    string
     */
    inline string WaveformID(bool q=false) 
	{if(q) Query(kWAVEFORM_ID);return fWFId;};

    /*!
     * Description: 
     *   horizontal sample interval of a YT waveform. 
     *   starts at 1ps/point
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    double value in seconds
     */
    inline double XIncrement(bool q=false) 
	{if(q) Query(kXINCREMENT);return fXINcr;};
    /*!
     * Description: 
     *   Set horizontal sample interval of a YT waveform. 
     *   starts at 1ps/point
     *
     * Arguments:
     *   value >= 1.0e-12
     *
     * returns:
     *    true on success
     */
    bool XIncrement(double v);

    /*!
     * Description: 
     *   QUERY ONLY - Multiplier for X data
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    double value - no units
     */
    inline double XMultiplier(bool q=false) 
	{if(q) Query(kXMULTIPLIER);return fXMUlt;};

    /*!
     * Description: 
     *    return the units associated with the X points   
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    one of the values in the enum UNIT
     *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
     *	   kWATT, kUNITSNONE};
     */
    inline UNITS XUnits(bool q=false) {if(q) Query(kXUNIT);return fXUNit;};
    /*!
     * Description: 
     *    set the units associated with the X points   
     *
     * Arguments:
     *    one of the values in the enum UNIT
     *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
     *	   kWATT, kUNITSNONE};
     *
     * returns:
     *    true on success
     */
    inline bool XUnits(UNITS u) {return SetUnits(true,u);}

    /*!
     * Description: 
     *   specify the number of seconds of pre-trigger or post-trigger
     *   of a YT waveform or specifies the vertical offset of the 
     *   horizontal component of an XY waveform. 
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *   offset in seconds {-1.0e15:1.0e15} 
     */
    inline double XZero(bool q=false) {if(q) Query(kXZERO);return fXZEro;};
    /*!
     * Description: 
     *   specify the number of seconds of pre-trigger or post-trigger
     *   of a YT waveform or specifies the vertical offset of the 
     *   horizontal component of an XY waveform. 
     *
     * Arguments:
     *    offset in seconds {-1.0e15:1.0e15} 
     *
     * returns:
     *    true on success
     */
    inline double XZero(double v) {return SetZero(true, v);};

    /*!
     * Description: 
     *   QUERY ONLY - Multiplier for Y data
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    double value - no units
     */
    inline double YMultiplier(bool q=false) 
	{if(q) Query(kYMULTIPLIER);return fYMUlt;};

    /*!
     * Description: 
     *    return the units associated with the Y points   
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *    one of the values in the enum UNIT
     *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
     *		kWATT, kUNITSNONE};
     */
    inline UNITS YUnits(bool q=false) {if(q) Query(kYUNIT);return fYUNit;};
    /*!
     * Description: 
     *    set the units associated with the Y points   
     *
     * Arguments:
     *    one of the values in the enum UNIT
     *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
     *	   kWATT, kUNITSNONE};
     *
     * returns:
     *    true on success
     */
    inline bool YUnits(UNITS u) {return SetUnits(false,u);}

    /*!
     * Description: 
     *   specify the vertical offset of a YT waveform or
     * the vertical offset of an XY waveform
     *
     * Arguments:
     *   q - if true ask, otherwise use last known value
     *
     * returns:
     *   offset in seconds {-1.0e15:1.0e15} 
     */
    inline double YZero(bool q=false) {if(q) Query(kYZERO);return fYZEro;};
    /*!
     * Description: 
     *   specify the number of seconds of pre-trigger or post-trigger
     *   of a YT waveform or specifies the vertical offset of the 
     *   horizontal component of an XY waveform. 
     *
     * Arguments:
     *    offset in seconds {-1.0e15:1.0e15} 
     *
     * returns:
     *    true on success
     */
    inline double YZero(double v) {return SetZero(false, v);};


    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const char *XAxis(void) const {return UnitString(fXUNit);};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const char *YAxis(void) const {return UnitString(fYUNit);};

    /*!
     * Description: 
     *   print out the entire data about this class. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    friend ostream& operator<<(ostream& output, const WFMPRE &n); 


    /*!
     * Description: 
     *   Helper function to determine how to scale Y points
     *   based on the values we gathered. 
     *
     * Arguments:
     *   double - y input point
     *
     * returns:
     *    double y - scaled output
     */
    double ScaleY(double y);
    /*!
     * Description: 
     *   Helper function to determine how to scale X points
     *   based on the values we gathered. 
     *   
     * Arguments:
     *    double - x input point
     *
     * returns:
     *    double x - scaled output
     */
    double ScaleX(double x);

    /*!
     * Description: 
     *   Helper function to determine how to scale XY points
     *   based on the values we gathered.
     *
     * Arguments:
     *   COMMENT THIS
     *
     * returns:
     *    
     */
    double ScaleXY( int i, short* DataIn, double &y);

#ifdef DEBUG_WFM
    void Test(void);
#endif

protected:
    /*!
     * Description: 
     *   Given a string that has returned from the inquiry decode it
     *   and fill all the variables in the class
     *
     * Arguments:
     *   val - input string to decode. 
     *
     * returns:
     *    true on success
     */
     bool     Decode (const char *val);


    /* ======================================================== */
private:
    void Reset(void);

    /*!
     * Description: 
     *   Helper function to make a query with a particular command 
     *   specified for this class. The function will subsequently 
     *   decode and stuff it in the appropriate variable. 
     *
     * Arguments:
     *   Command to issue query
     *
     * returns:
     *    true on success. 
     */
    bool Query(COMMANDs c);
    /*!
     * Description: 
     *   Helper function to set Units for X and Y
     *
     * Arguments:
     *   one of the values in the enum
     *    {kAMPS, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
     *		kWATT, kUNITSNONE};
     *
     * returns:
     *    true on success
     */
    bool SetUnits(bool X, UNITS u);
    /*!
     * Description: 
     *   Helper function to set Zero for X and Y
     *
     * Arguments:
     *     seconds from {-1e15:1e15}
     *
     * returns:
     *    true on success
     */
    bool SetZero(bool X, double v);


    static const char* Axis[9];

    bool     fACState; // true = ENHANCED
    bool     fBIT;     // true = 16 bit, false = 8 bit
    bool     fBN;      // Should always be true. 
    bool     fBYTE;    // true = 2 bytes, false = 1 byte
    bool     fBYTor;   // true = MSB, false = LSB
    char     fCRVchk;  // 0 - CHKsm0, 1 - NONe, 2 - NULL zero checksum
    string   fDate;    // current system date
    bool     fENCdg;   // true - binary, false - ASCII
    string   fLABel;   // waveform label
    unsigned fNRpt;    // number points 512,1024,2048,4096...32768
    PT_TYPES fPTfmt;   // 0 - ENV, 1 - XY, 2 - Y
    string   fTime;    // current system time
    double   fTSTime;  // time between trigger and first point in s
    string   fWFId;    // waveform id
    double   fXINcr;   // sec/pt
    double   fXMUlt;   // X multiplier
    UNITS    fXUNit;   // 0 - AMPS, 1 - DB, 2 - DEGrees, 3 - DIVS
                       // 4 - HERtz, 5 OHMs, 6 - SEConds, 7 - VOLts
                       // 8 - WATts
    double   fXZEro;   // number of seconds of pre/post trigger
    double   fYMUlt;
    UNITS    fYUNit;
    double   fYZEro;

};
#endif
