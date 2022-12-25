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
 * Summary page 38
 * Details start on page 44
 *
 *******************************************************************
 */
#ifndef __TRACE_hh_
#define __TRACE_hh_
#    include <vector>
#    include "CObject.hh"
#    include "AdjTrace.hh"
#    include "DSA602_Types.hh"


/// Trace documentation here. 
class Trace : public CObject
{
public:

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

    static const size_t kMaxTraces;


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
     *   put in trace number {1:8}, 0 - get all traces.
     *
     * returns:
     *    true on success
     */
    bool Update(size_t trace); 

    /*!
     * Description: 
     *   Get the number of traces available. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    Number available traces. 
     */
    unsigned int  GetNTrace(void);

    /*!
     * Description: 
     *   get the specified trace if available. 
     *
     * Arguments:
     *   Trace number
     *
     * returns:
     *    Class AdjTrace
     */
    inline AdjTrace* GetData(int n) {if (n<fNTrace) return fAdjTrace[n]; else
					 return NULL;};

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
     *   Decode the return string for any query. 
     *
     * Arguments:
     *   result - a standard string containing the parsed result. 
     *   main   - if set to true, then it is a main window result. 
     *
     * returns:
     *    true on success
     */
    bool Decode(const char *c);


    /*!
     * Description: 
     *   Find, given a trace identifier, see if it is already allocated 
     *   in the array. 
     *
     * Arguments:
     *   trace to find in array. 
     *
     * returns:
     *    index into the array where that number exists. 
     */
    size_t Find(size_t n);

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

    int    fNTrace;           // number of traces detected. 
    vector<AdjTrace*> fAdjTrace; // Maximum number of traces.
};
#endif
