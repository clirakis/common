/**
 ******************************************************************
 *
 * Module Name : DSA602.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : header file for DS602 digitizing scope with GPIB 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 *
 * Commands are in the format of 
 * Delimiters----+
 *         |     |
 * ENCDG <sp> SET:BINARY
 * ----- ---- --- ------
 *   |         |     |
 * Header     Link Argument
 *
 * in document the various symbols are:
 * <CR>
 * <EOI> 
 * <LF>
 * <ui>  unsigned integer
 * <NR1> signed integer value
 * <NR2> Floating point value
 * <NR3> Floating point value with exponent
 * <NRX> could be any of the above range -1E+/-300, 0, 1E+/-300
 * <asc bin> ASCII formatted pixel bin count data
 * <asc curve> ASCII formatted waveform data
 * <asc data> ASCII formatted histogram data
 * <bblock> binary block formatted waveform data
 * <meas> One or more of the DSA measurement commands
 * <id> {A|...|Z}
 * <sp> space
 * <qstring> quoted string data
 * <slot> L,C, or R representing Left Center or Right for plug ins
 *
 * TBMAIN and TBWIN control the time base units and are on page: 271
 * need commands for Event and FALltime around page 148
 * FFT 150
 * FREQ 157
 * GAIN
 * HPGL support page 170 - plotter setup
 *   Graticle
 *   port
 * ID? - data about system page 174
 * Measurements...
 * MAX page 190
 * Amplitude: GAIn, MAX, MEAN, MID, MIN, OVErshoot, PP, RMS, UNDershoot
 * Area/Energy: YTEnergy, YTMns_area, YTPLs_area
 * Frequency: SFReq, SMAg, THD
 * Timing: CROss, DELay, DUTy, FALltime, FREq, PDElay, PERiod, 
 *         PHAse, RISetime, SKEW, TTRig, WIDth page 196
 *
 * MEAS - execute a list of measurements
 * 
 * binary block format - 
 *   <byte cnt><double><qual><chksum>
 *   <byte cnt> 2 byte integer MSB first length of block
 *   <double>   8 byte binary value in 64 bit IEEE FP
 *   <qual>     1 byte binary qualifier, page 195
 * 
 *   <chksum> see CURVE for definition. 
 *
 * Command summary page 30
 * NAVG, NENV page 217
 * OUTput page 221 
 *
 * GPIB parameters on page 4, select utility and go to page 2. 
 * 
 *******************************************************************
 */
#ifndef __DSA602_hh_
#define __DSA602_hh_
#  include <fstream>
#  include "GPIB.hh"
#  include "WFMPRE.hh"
#  include "System.hh"
#  include "StatusAndEvent.hh"
#  include "Measurement.hh"
#  include "Module.hh"
#  include "TimeBase.hh"
#  include "Trace.hh"

/// DS602 documentation here. 
class DSA602 : public GPIB
{
public:

    /*!
     * Description: 
     *   Constructor - Opens up the GPIB connection with the provided address.
     *   The user is responsible for checking to see if there was an 
     *   error in opening the GPIB connection. 
     *
     * Arguments:
     *   gpib_address - GPIB address of the scope {0:31}
     *
     * returns:
     * ....
     *    
     */
    DSA602(unsigned int gpib_address);


    /*!
     * Description: 
     *   Destructor - clean up after our mess. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    ~DSA602(void);

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
    static DSA602* GetThis()    {return fDSA602;};


    /* ======================================================== */
    /*                     Waveform FUNCTIONS                   */ 
    /* ======================================================== */

    inline bool GetWaveformHeader(void) {return fWFMPRE->Update();};
    

    /*!
     * Description: 
     *   Turn averaging on/off
     *
     * Arguments:
     *   state - true(on), false(off)
     *
     * returns:
     *    NONE
     */
    void    Avg(bool state);
    /*!
     * Description: 
     *   Gets the current status of the Average bit. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true if set, false if not. 
     */
    bool    Avg(void);   // Return the current state

    /*!
     * Description: 
     *   Get the points associated with the trace indicated.
     *   The arrays are allocated in the call, the user
     *   is expected to clean up these allocations.  
     *
     * Arguments:
     *   trace - 
     *   X - pointer to X return data. 
     *   Y - pointer to Y return data. 
     *
     * returns:
     *    returns number of points in the arrays.
     */
    size_t Curve(int trace, double **X, double **Y);

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
    inline double  Frequency(void) {return DQuery("FREQ");};
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
    inline double  FallTime(void) {return DQuery("FALLTIME");};
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
    inline double  RMS()      {return DQuery("RMS");};
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
    inline double  Gain()     {return DQuery("GAIN");};
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
    inline double  Max()      {return DQuery("MAX");};
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
    inline double  Mean()     {return DQuery("MEAN");};
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
    inline double  Mid()      {return DQuery("MID");};
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
    inline double  Min()      {return DQuery("MIN");};


    // Trace manipulation ========================================


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
    void SetDisplayTrace(unsigned char n, bool t);
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
//     inline bool GetDisplayTrace(unsigned char n) 
// 	{if(n<8) return fTrace[n]; else return false;};


    /*!
     * Description: 
     *   Give the calling fuction a string about the current 
     *   version, compliaton etc.
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    Current Version of the system.
     */
    const char* Version(void) const;

    /*!
     * Description: 
     *   Allow user to get access to the System calls
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *   pointer to system menu calls
     *    
     */
    inline System* pSystem(void) {return fSystem;};

    /*!
     * Description: 
     *   Allow user to get access to the Measurement calls
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *   pointer to system menu calls
     *    
     */
    inline Measurement* pMeasurement(void) {return fMeasurement;};

    /*!
     * Description: 
     *   Allow user to get access to the Status and Event calls
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *   pointer to status and event calls
     *    
     */
    inline StatusAndEvent* pStatusAndEvent(void) {return fpSE;};

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
    friend ostream& operator<<(ostream& output, const DSA602 &n); 


    /* ======================================================== */
private:
    static DSA602* fDSA602;         // This pointer

    //bool            fTrace[8];      // Is trace displayed on screen?
    System*         fSystem;        // Handle to all the System calls
    StatusAndEvent* fpSE;           // Handle to status and event calls.
    Measurement*    fMeasurement;   // Handle to measurement calls. 
    TimeBase*       fTimeBase;      // Handle to time base calls. 
    /*!
     * Waveform Preamble, query and data return. 
     */
    WFMPRE*         fWFMPRE;
    Trace*          fTrace;        // Trace class

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    true on success
     */
    bool        Init();

    /*!
     * Description: 
     *   Helper function to query something from the system. 
     *
     * Arguments:
     *   command - 
     *
     * returns:
     *    
     */
    double      DQuery(const char *command);
};
#endif