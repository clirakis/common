/**
 ******************************************************************
 *
 * Module Name : Trace.hh
 *
 * Author/Date : C.B. Lirakis / 06-Feb-21
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 * ADJTrace - placement of trace on screen. 
 *   Summary page 38
 *   Details start on page 44
 *
 * Trace - page 292 All the other properties. 
 *   
 *
 *******************************************************************
 */
#ifndef __TRACE_hh_
#define __TRACE_hh_
#  include "CObject.hh"
#  include "AdjTrace.hh"
#  include "DefTrace.hh"
#  include "DSA602_Types.hh"

/// Keep trace of which traces are on the screen. 
class Trace : public CObject
{
public:
    static const uint8_t kMaxTraces = 8;

    /*!
     * Description: 
     *   Constructor for Trace class. There can be upto 8 traces 
     *   total. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    ....
     */
    Trace(void);
    /*!
     * Description: 
     *   Clean up the mess we created. 
     *
     * Arguments:
     *    NONE
     *
     * returns:
     *    NONE
     */
    ~Trace(void);


    /* ======================================================== */
    /*             Private Data Access Functions                */ 
    /* ======================================================== */

    
    /* ======================================================== */
    /*        Adjust Trace  ADJtrace page: 44                   */ 
    /* ======================================================== */

    /*!
     * Description: 
     *   Query everything with adjust trace parameters. 
     *
     * Example: 
     * 'ADJTRACE1 PANZOOM:OFF,HMAG:-1.0E+0,HPOSITION:1.0E+16,HVPOSITION:1.0E+16,HVSIZE:-1.0E+0,TRSEP:1.0E+16,VPOSITION:1.0E+16,VSIZE:-1.0E+0,FSPAN:-1.0E+0,FRESOLUTION:-1.0E+0'
     * 
     * Arguments:
     *   init - if true, assume we have nothing. 
     *
     * returns:
     *    true on success
     */
    bool Update(bool init=false); 

    /*!
     * Description: 
     *   Get the number of traces available. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    Number available traces. {0:8}
     */
    unsigned int  GetNTrace(void);

    /*!
     * Description: 
     *   get the specified trace Adj if available. 
     *
     * Arguments:
     *   index to the trace number Trace number {0:7}
     *
     * returns:
     *    Class AdjTrace
     */
    inline AdjTrace* GetAdj(int n) {if (n<fNTrace) return fAdjTrace[n]; else
					 return NULL;};
    /*!
     * Description: 
     *   get the specified trace Definition if available. 
     *
     * Arguments:
     *   index to the trace number Trace number {0:7}
     *
     * returns:
     *    Class AdjTrace
     */
    inline DefTrace* GetDef(int n) {if (n<fNTrace) return fDefTrace[n]; else
					 return NULL;};

    inline uint8_t GetSelectedTrace(void) const {return fSelectedTrace;}
    inline void SetSelectedTrace(uint8_t sel)   {fSelectedTrace = sel;};
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
    friend ostream& operator<<(ostream& output, const Trace &n); 

    /* ======================================================== */
private:

    /*!
     * Description: 
     *   Find, given a trace identifier, see if it is already allocated 
     *   in the array. 
     *
     * Arguments:
     *   id - trace to find in array. 
     *
     * returns:
     *    index into the array where that number exists. 
     */
    uint8_t Find(uint8_t id);

    /*!
     * Description: 
     *   Delete all fAdjTrace and set to NULL
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    void Reset(void);



#ifdef DEBUG_TRACE
    void Test(void);
#endif
    int8_t    fNTrace;               // number of traces detected. 
    uint8_t   fSelectedTrace;        // This is an index into the trace 
    AdjTrace* fAdjTrace[kMaxTraces]; // Maximum number of traces.
    DefTrace* fDefTrace[kMaxTraces]; // Maximum number of traces.
};
#endif
