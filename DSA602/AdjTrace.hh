/**
 ******************************************************************
 *
 * Module Name : AdjTrace.hh
 *
 * Author/Date : C.B. Lirakis / 13-Feb-21
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
#ifndef __ADJTRACE_hh_
#define __ADJTRACE_hh_
#    include "CObject.hh"
#    include "DSA602_Types.hh"

// Various parameters assoicated with up to 8 traces. 
class AdjTrace : public CObject
{
public:

    enum COMMANDs {kFRESOLUTION = 0, kFSPAN, kHMAG, kHPOSITION, 
		   kHVPOSITION, kHVSIZE, kPANZOOM, kTRSEP, kVPOSITION, 
		   kVSIZE, kEND_LIST};

    static const struct t_Commands Commands[kEND_LIST+1];

    /* 
     * Allowed magnification values, BUT this is dependent on record length
     * See page: 47. 
     */
    enum HMAG_VAL {kHM_1=0, kHM_2, kHM_2_5, kHM_4, kHM_5, kHM_8, kHM_10, 
		   kHM_16, kHM_20, kHM_25, kHM_40, kHM_50, 
		   kHM_80, kHM_100, kHM_160, kHM_200, kHM_250, 
		   kHM_400, kHM_500, kHM_800, kHM_1000, kHM_2000, 
		   kHM_5000, kHM_END};
    static const double HMAG_Values[kHM_END];

    /*!
     * Description: 
     *   The AdjTrace class holds all the parameters for a screen trace. 
     *   There are 8 possible traces available on the screen at any time. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    AdjTrace(void);

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
     *   QUERY ONLY the frequency resolution or FFT bin width
     *   ONLY VALID FOR FFT
     *   ADJ2? FRE
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    true on success
     */
    inline double FrequencyResolution(bool q=false) 
	{if(q) Query(kFRESOLUTION); return fFResolution;};

    /*!
     * Description: 
     *   QUERY ONLY the Frequency span of the FFT. 
     *   ONLY VALID FOR FFT
     *
     * Arguments:
     *   t - trace number
     *   q - true if we want an immediate query
     *
     * returns:
     *    double 
     */
    inline double FrequencySpan(bool q) 
	{if(q) Query(kFSPAN); return fFSpan;};

    /*!
     * Description: 
     *   Query the horizontal magnification factor when PANzoom is on. 
     *   HMAg also depends on record length, see page 47 for details. 
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    ZOOM see page 46
     */
    inline double HorizontalMagnification(bool q=false)
	{if(q) Query( kHMAG);return fHorizontialMagnification;};

    /*!
     * Description: 
     *   Set Horizontal Magnification
     *
     * Arguments:
     *   v - value based on enum above. HMAG_VAL
     *
     * returns:
     *    true - on success
     */
    inline bool HorizontalMagnification(HMAG_VAL v);

    /*!
     * Description: 
     *   Query the horizontal postion when PANZOOM is on. 
     *   ADJ2 HPO: val
     *   Range: length - ceil (10.24 * max_HMAG/HMAG)
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    
     */
    inline double HorizontalPosition(bool q=false) 
	{if(q) Query( kHPOSITION); return fHorizontialPosition;};

    /*!
     * Description: 
     *   Set the waveform horizontial position when PANZOOM is true. The HPO
     * value is in waveform points from 0(zero) to the upper value determined
     * by the setting of HMAG and the record length by the following formula. 
     * 
     *    length - ceil(10.24*max_HMAG/HMAG)
     *
     * Arguments:
     *   v - integer value value
     *
     * returns:
     *    true on success
     */
    bool HorizontalPosition(size_t v); 

    /*!
     * Description: 
     *   Query the XY position horizontally
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    
     */
    inline double HVPosition(bool q=false) 
	{if(q) Query(kHVPOSITION);return fHVPosition;};

    /*!
     * Description: 
     *   For XY waveforms created in floating-point mode, HVPosition
     *   sets the graphical position of the horizontal component of the
     *   waveform. 
     *   
     * Arguments:
     *   v - value {-1E15,1E15}
     *
     * returns:
     *    true on success
     */
    inline bool HVPosition(double v) {return PosSize(true, true, v);};

    /*!
     * Description: 
     *   Query the horizontal waveform size
     *
     * Arguments:
     *   t - trace number {1:8}
     *   q - true if we want an immediate query
     *
     * returns:
     *    
     */
    inline double HVSize(bool q) 
	{if(q) Query(kHVSIZE);return fHVSize;};
    /*!
     * Description: 
     *   For XY waveforms created in floating-point mode, HVSize
     *   sets the graphical size of the horizontal component of the
     *   waveform. 
     *   
     * Arguments:
     *   v - value {-1E15,1E15}
     *
     * returns:
     *    true on success
     */
    inline bool HVSize(double v) {return PosSize(true, false, v);}; 
    //bool HVSize(double v);

    /*!
     * Description: 
     *   Query the trace seperation for waveforms in integer mode. 
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    
     */
    inline double TraceSeparation(bool q=false) 
	{if(q) Query( kTRSEP); return fTRSEP;};

    /*!
     * Description: 
     *   For waveforms created in integermode, TRSep (trace separation)
     *   sets the window waveform separation in graticule divisions. 
     *   These waveforms must have been created on the WIN1 or WIN2 time base
     *   and can not be XY waveforms. 
     *
     * Arguments:
     *   v - value {-5.0:5.0}
     *   
     *
     * returns:
     *    true - on success
     */
    bool TraceSeparation(double v); 

    /*!
     * Description: 
     *   Query if the PANZOOM is enabled. 
     *
     * Arguments:
     *   t - trace number {1:8}
     *   q - true if we want an immediate query
     *
     * returns:
     *    
     */
    inline bool PanZoom(bool q=false) {if(q)Query(kPANZOOM); return fPanZoom;};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   q - true ON, false OFF
     *
     * returns:
     *    true on success. 
     */
    inline bool SetPanZoom(bool s); 


    /*!
     * Description: 
     *   Query the floating point mode Vertical Position
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    position {-1E15:1E15}
     */
    inline double VerticalPosition(bool q=false) 
	{if(q) Query( kVPOSITION);return fVPosition;};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   v - value {1E-15:1E15}
     *
     * returns:
     *    true - on success
     */
    inline bool VerticalPosition(double v) {return PosSize(false, true, v);};


    /*!
     * Description: 
     *   Query the wavefor size for floating point waveforms
     *
     * Arguments:
     *   q - true if we want an immediate query
     *
     * returns:
     *    v - value {1E-15:1E15}
     */
    inline double VerticalSize(bool q=false) 
	{if(q) Query(kVSIZE);return fVSize;};

    /*!
     * Description: 
     *   Set
     *
     * Arguments:
     *   t - trace {1:8}
     *   v - value {-1.0e15:1.0e15}
     *   
     *
     * returns:
     *    true - on success
     */
    inline bool VerticalSize(double v) {return PosSize(false, false, v);};


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
    inline size_t Number(void) const {return fTraceNumber;};
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
    inline void   Number(size_t n)   {if (n<9) fTraceNumber = n;};

    /*!
     * Description: 
     *   print out the entire data about this class. 
     *
     * Arguments:
     *   output - stream to stuff with information. 
     *   n - AdjTrace class to format
     *
     * returns:
     *   fully formatted output stream. 
     *    
     */
    friend ostream& operator<<(ostream& output, const AdjTrace &n); 


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


    size_t fTraceNumber; 
    bool   fPanZoom;
    double fHorizontialMagnification;
    double fHorizontialPosition;
    double fHVPosition;
    double fHVSize;
    double fTRSEP;
    double fVPosition;
    double fVSize;
    double fFSpan;
    double fFResolution;
};
#endif
