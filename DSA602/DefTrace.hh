/**
 ******************************************************************
 *
 * Module Name : DefTrace.hh
 *
 * Author/Date : C.B. Lirakis / 29-Dec-22
 *
 * Description : Contains the information about how the trace is defined.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 * Summary page 292
 *
 *******************************************************************
 */
#ifndef __DEFTRACE_hh_
#define __DEFTRACE_hh_
#  include <string.h>
#  include "CObject.hh"
#  include "DSA602_Types.hh"

// Various parameters assoicated with up to 8 traces. 
class DefTrace : public CObject
{
public:

    enum COMMANDs {kACCUMULATE=0, kACSTATE, kDESCRIPTION, kGRLOCATION, 
		   kGRTYPE, kWFMCALC, kXUNIT, kYUNIT,
		   kEND_LIST};

    static const struct t_Commands Commands[kEND_LIST];


    /*!
     * Description: 
     *   The DefTrace class holds all the parameters for a screen trace. 
     *   There are 8 possible traces available on the screen at any time. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    DefTrace(void);
    ~DefTrace(void);

    /*!
     * Description: 
     *   Get the data associated with this trace. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true on success.
     */
    bool Update(void);

    /*!
     * Description: 
     *   Given a string from a query for a specific trace decode it 
     *   and fill the appropriate field within the class. 
     *
     * Arguments:
     *   s - string to decode
     *
     * returns:
     *    true on success. 
     */
    bool Decode(const string &s);


    /*!
     * Description: 
     *   Clear all values in the class to zero or false
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    void Clear(void);


    /* ======================================================== */
    /* Set and query the elements of the class.                 */
    /* ======================================================== */

    /*!
     * Description: 
     *   Queries the display persistance for the specified trace. 
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    kINFPERSIST=0, kACC_OFF, kVARPERSIST
     */
    inline ACCUMULATE Accumulate(bool q=false) 
	{if(q) Query(kACCUMULATE); return fACCumulate;};

    /*!
     * Description: 
     *  Sets the display persistance for the specified trace. 
     *
     * Arguments:
     *   Val - value to set to. kINFPERSIST=0, kACC_OFF, kVARPERSIST
     *
     * returns:
     *    true on success. 
     */
    bool Accumulate(ACCUMULATE val); 

    /*!
     * Description: 
     *   QUERY ONLY - return the accurace mode for the waveform. 
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    true - ENHANCED, false - Not ENHANCED
     */
    inline bool ACState(bool q) 
	{if(q) Query(kACSTATE); return fACState;};

    /*!
     * Description: 
     *   string description of source trace.
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    string value
     */
    inline const char* Description(bool q=false)
	{if(q) Query(kDESCRIPTION);return fDescription->c_str();};

    /*!
     * Description: 
     *   set the string value of the description
     *
     * Arguments:
     *   string value
     *
     * returns:
     *    true - on success
     */
    inline bool Description(const char *description);

    /*!
     * Description: 
     *   What is the graticule location?
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     * true if upper, false if lower. 
     *    
     */
    inline bool GRLocation(bool q=false) 
	{if(q) Query( kGRLOCATION); return fGRLocation;};

    /*!
     * Description: 
     *   Set the waveform position on the graticule.
     *
     * Arguments:
     *   v - true for upper, false for lower. 
     *
     * returns:
     *    true on success
     */
    bool SetGRLocation(bool v); 

    // SKIP GRTYPE, doesn't do much. 

    /*!
     * Description: 
     *   Query the XY position horizontally
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    true  - (HIPrec) High precision floating point mode. 
     *    false - (FASt)   fast integer mode. 
     */
    inline bool WFMCalc(bool q=false) 
	{if(q) Query(kWFMCALC);return fWFMCalc;};

    /*!
     * Description: 
     *   Query the X axis units
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    from enum UNITS
     */
    inline UNITS XUNIT(bool q) 
	{if(q) Query(kXUNIT);return fXUNit;};

    /*!
     * Description: 
     *   Query the YUNIT
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *   enum UNITS
     */
    inline UNITS YUNIT(bool q=false) 
	{if(q) Query( kYUNIT); return fYUNit;};


    /* ======================================================== */
    /* ======================================================== */

    /*!
     * Description: 
     *    Return the trace number associated with this collection 
     *    of characteristics. 
     *
     * Arguments:
     *    none
     *
     * returns:
     *    trace number, {1:8}
     */
    inline uint8_t Number(void) const {return fTraceNumber;};
    /*!
     * Description: 
     *     Sets the trace number, used by calling program.    
     *
     * Arguments:
     *     n - trace number to set to 0<n<=8
     *
     * returns:
     *     none
     */
    inline void   Number(uint8_t n)   {if (n<9) fTraceNumber = n;};

    /*!
     * Description: 
     *   print out the entire data about this class. 
     *
     * Arguments:
     *   output - stream to stuff with information. 
     *   n - DefTrace class to format
     *
     * returns:
     *   fully formatted output stream. 
     *    
     */
    friend ostream& operator<<(ostream& output, const DefTrace &n); 


#ifdef DEBUG_TRACE
    void Test(void);
#endif

private:

    /*!
     * Description: 
     *   Query general query function. 
     *
     * Arguments:
     *   command to issue for query. 
     *
     * returns:
     *    true on success. 
     */
    bool Query(COMMANDs c);

    /*!
     * Description: 
     *   Helper function for Horizontal/Vertial position/size
     *
     * Arguments:
     *   Horizontial - true for horizontal, false for Vertical. 
     *   Position    - true for position, false for size.
     *   value    {-1.0e15:1.0e15}
     *
     * returns:
     *    true on success. 
     */
    bool PosSize(bool Horizontal, bool Position, double value);


    uint8_t fTraceNumber;    // {1:8} note no index 0. 

    /*
     * Parameters associated with ADJTrace command. 
     */
    ACCUMULATE fACCumulate; 
    bool       fACState;
    string*    fDescription;
    bool       fGRLocation;
    bool       fWFMCalc;
    UNITS      fXUNit;
    UNITS      fYUNit;
};
#endif
