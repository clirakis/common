/**
 ******************************************************************
 *
 * Module Name : TimeBase.hh
 *
 * Author/Date : C.B. Lirakis / 30-Jan-21
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
 * Details start on page 272  TBMain, TBWin
 *
 *******************************************************************
 */
#ifndef __TIMEBASE_hh_
#define __TIMEBASE_hh_
#    include "CObject.hh"
#    include "DSA602_Types.hh"

/// TimeBase documentation here. 
class TimeBase : public CObject
{
public:

    /*!
     * Description: 
     *   Constructor for TimeBase class
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    ....
     */
    TimeBase(void);
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
    ~TimeBase(void);

    /*!
     * Description: 
     *   static member such that other areas in a parent program
     *   can gain access. 
     *
     * Arguments:
     *     NONE
     *
     * returns:
     *    pointer to this.
     */
    static TimeBase* GetThis()    {return fTimeBase;};


    enum COMMANDs { kLENGTH=0, kTIME, kXINCREMENT, kEND_LIST};
    enum FRAME    {kMAIN=0, kWINDOW, kEND_FRAME};
    /* Possibl length enums. */
    enum LENGTH   {k512=0, k1024, k2048, k4096, k5120, k8192, 
		   k16384, k20464, k32768, k_LENGTH_END};
    // Permitted time base values. 
    enum PERIOD {k50ps=0, k100ps, k200ps, k400ps, k500ps, 
		 k1ns, k2ns, k4ns, k5ns, k10ns, k20ns, k25ns, k50ns, k100ns,
		 k200ns, k250ns, k400ns, k500ns, k800ns, 
		 k1us, k2us, k2p5us, k4us, k5us, k8us, k10us, k20us,
		 k40us, k50us, k100us, k200us, k500us, k1ms, k2ms, 
		 k5ms, k10ms, k20ms, k50ms, k100ms, k200ms, k500ms,
		 k1s, k2s, k5s, k10s, k20s, k50s, k100s,
		 kTB_END}; 
    // Time base will apparently support from 100us to 100s to be shown. 


    static const struct t_Commands TBCommands[kEND_LIST+1];
    static struct t_TBLength PossibleLengths[10];
    static const struct t_Period Period[kTB_END];

    /* ======================================================== */
    /*             Private Data Access Functions                */ 
    /* ======================================================== */


    /* ======================================================== */
    /*    Time Base TB specific commands, starting page 272     */
    /* ======================================================== */

    /*!
     * Description: 
     *   Query the main time base to the specified record length
     *   scaled in points per waveform. 
     * 
     *   Page 272  Length, Time, XIN
     * 
     * Arguments:
     *   q - query current value (true) or return last interrogated value
     *
     * returns:
     *    Time base for main window length in samples. 
     */
    inline double MainLength(bool q=false) 
	{if(q) Query(kLENGTH,true); return fLength[kMAIN];};

    /*!
     * Description: 
     *   Set the main time base to the specified record length
     *   scaled in points per waveform. 
     * 
     *   Page 272  Length, Time, XIN
     *
     * Arguments:
     *   v the time length : 
     * 
     *
     * returns:
     *    
     */
    inline bool MainLength(LENGTH v) {return SetLength(v, true);};

    /*!
     * Description: 
     *   Query the horizontal scale (time per division.)
     *   Page 272  Length, Time, XIN
     * Arguments:
     *   q - query current value (true) or return last interrogated value
     *
     * returns:
     *    Time base for main window length in samples. 
     */
    inline double MainTime(bool q=false) 
	{if(q) Query(kTIME, true); return fTime[kMAIN];};

    /*!
     * Description: 
     *   Set the horizontal scale (time per division.)
     *   Page 272  Length, Time, XIN
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool MainTime(PERIOD i) { return SetPeriod(i,true);};


    /*!
     * Description: 
     *   Query the sample intervale of the time base in seconds per point.
     *   TBMain XINcr * (TBMain LENgth-1)
     *   Page 274  Length, Time, XIN
     *   QUERY ONLY
     *
     * Arguments:
     *  q - true - make a query 
     *
     * returns:
     *    
     */
    inline double MainXIncrement(bool q= false)
	{if(q) Query(kXINCREMENT, true); return fXIncrement[kMAIN];};

    /*!
     * Description: 
     *   Query
     *   Page 272  Length, Time, XIN
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline double WindowLength(bool q) 
	{if(q) Query(kLENGTH, false); return fLength[kWINDOW];};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool WindowLength(LENGTH v) {return SetLength(v, false);};

    /*!
     * Description: 
     *   Query
     *   Page 272  Length, Time, XIN
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline double WindowTime(bool q=false) 
	{if(q) Query(kTIME, false); return fTime[kWINDOW];};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool WindowTime(PERIOD i) {return SetPeriod(i,false);};


    /*!
     * Description: 
     *   Query the sample intervale of the time base in seconds per point.
     *   TBMain XINcr * (TBMain LENgth-1)
     *   Page 274  Length, Time, XIN
     *   QUERY ONLY
     *
     * Arguments:
     *  q - true - make a query 
     *
     * returns:
     *    
     */
     inline double WindowXIncrement(bool q=false) 
	{if(q) Query(kXINCREMENT, false); return fXIncrement[kWINDOW];};


    /*!
     * Description: 
     *   Given a retrieved time value, determine what the index into
     *   the array of values is. 
     * 
     * Arguments:
     *   t - value to return index on. 
     *
     * returns:
     *    index
     */
    uint32_t IndexFromTime(double);

     

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
    friend ostream& operator<<(ostream& output, const TimeBase &n); 

    /* ======================================================== */
private:

  /*!
     * Description: 
     *   Given the time index from the enum PERIOD above 
     *   setup the array of possible sample lenghts possible for 
     *   that period. 
     * 
     * Arguments:
     *   PERIOD index. 
     *
     * returns:
     *    none
     */
    bool SampleLengthsFromTimeIndex(PERIOD index);

    /*!
     * Description: 
     *   Query general query function. 
     *
     * Arguments:
     *   command to issue for query. Stuffs result into corresponding 
     *   variable supported in class. If we pass in kEND_LIST as a command
     *   all variables will be interrogated. 
     *
     *   All looks something like: 'TBMAIN LENGTH:512,TIME:5.0E-7,XINCR:1.0E-8'
     * 
     *   Main - set to true if Main window. 
     *
     * returns:
     *    true on success. 
     */
    bool Query(COMMANDs c, bool Main);

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
    bool Decode(const char *c, bool Main);

    /*!
     * Description: 
     *   Check Time and length values for compatiblity.
     *   It will also check that the suggested time base is realistic. 
     *   and the suggested Length.  
     *
     *   During the check it will fill the following variables:
     *      fNPossibleLengths - the size of the array for possible time/length
     *                          pairs. 
     *      fPossibleLengths  - const pointer to array of possible lengths.
     *
     * Arguments:
     *   Time in seconds
     *   
     *
     * returns:
     *   closest time value that is permissable. 
     */
    double CheckLength(double Time);

    /*!
     * Description: 
     *   Set Time Length based on the enum LENGTH value and WINDOW selected. 
     * Arguments:
     *   enum for one of the permissible values. 
     *   main   - if set to true, then it is a main window result. 
     *
     * returns:
     *   true on success. 
     */
    bool SetLength(LENGTH Lindex, bool main);

    /*!
     * Description: 
     *   Set Time period based on the enum PERIOD value and WINDOW selected. 
     * Arguments:
     *   enum for one of the permissible values. 
     *   main   - if set to true, then it is a main window result. 
     *
     * returns:
     *   true on success. 
     */
    bool SetPeriod(PERIOD Time, bool main);
#ifdef DEBUG_TB
    void Test(void);
#endif

    // Possible varibles associated with timebase. 

    double fMainPosition;

    double fLength[kEND_FRAME];
    double fTime[kEND_FRAME];
    double fXIncrement[kEND_FRAME];

    // Given a time in the CheckTimeLength, return the values for
    // possible lengths and number of lengths in the index here. 
    size_t fNPossibleLengths; 

    static TimeBase* fTimeBase;         // This pointer

};
#endif
