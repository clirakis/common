/**
 ******************************************************************
 *
 * Module Name : HP3314A.hh
 *
 * Author/Date : C.B. Lirakis / 26-Nov-14
 *
 * Description : header file for HP3314A arb
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : HP3314 operating manual - The unit I have does not appear
 * to respond well to any vector commands. DT, VH, VL, VM
 * The operator manual starts talking about the specifics of GPIB 
 * commands on page 81.
 *
 * Immediate execute commands are found on page: 91
 * Set commands on page: 96
 * Query commands can be found on page: 97
 * Trigger message: 99
 * 
 * Summary Card for commands on page: 101
 *
 * Error codes on page 55
 * Details on specs page: 60
 *
 * 
 *******************************************************************
 */
#ifndef __HP3314A_h_
#define __HP3314A_h_
#    include "GPIB.hh"

// immediate commands 
#define CALIBRATE_ALL       "CA"
#define CALIBRATE_DISABLE   "CD"
#define CALIBRATE_ENABLE    "CE"
#define CALIBRATE_FREQUENCY "CF"
#define DELETE_VECTOR       "DV"
#define INSERT_VECTOR       "IV"
#define MANUAL_TRIGGER      "MN"
#define PRESET              "PR"
#define RANGE_DOWN          "RD"
#define RANGE_UP            "RU"

class HP3314A : public GPIB
{
public:
    enum QUERYS {kAMPLITUDE=0, kAMPLITUDE_MODULATION, kARB_MODE, 
		 kARB_WAVE_NUMBER, kARB_DT, kERROR_CODE, kFREQUENCY, 
		 kFREQUENCY_MODULATION, kFUNCTION_INVERT, kFUNCTION_SELECT, 
		 kMANUAL_SWEEP, kMARKER_FREQUENCY, kQMODE, kQN, kOFFSET, 
		 kPHASE, kSTART_FREQUENCY, kSTOP_FREQUENCY, kQSWEEP,
		 kSWEEP_MASK, kSYMMETRY, kQTRIGGER_INTERVAL,
		 kQTRIGGER_THRESHOLD, kQTRIGGER_SLOPE, kQTRIGGER_SOURCE, 
		 kQVCO, kVECTOR_HEIGHT, kVECTOR_LENGTH, kVECTOR_MARKER};

    enum ARB_MODE  {kARB_OFF=0,kARB_ON,kARB_ON_INIT};
    enum WAVEFORM   {kDCONLY=0, kSINE, kSQUARE, kTRIANGLE};
    enum MODE       {kFREERUN=1, kGATE, kN_CYCLE, kHALF_CYCLE, kFINXN, 
		     kFINOVERN, kARB};
    enum PLLMASK    {kPLL_MASKED=0, kACQUIRING_LOCK, kLOOSING_LOCK, 
		     kPLL_EITHER};
    enum SWEEP      {kSWEEP_OFF=0, kSWEEP_LINEAR, kSWEEP_LOG};
    enum SWEEP_MASK {kSWEEP_MASKED=0, kSWEEP_START, kSWEEP_STOP, 
		     kSWEEP_EITHER};
    enum TRIGGER_THRESHOLD {kPLUS_ONE=1, kZERO_VOLTS};
    enum TRIGGER_SLOPE     {kPOSITIVE=1, kNEGATIVE};
    enum TRIGGER_SOURCE    {kINTERNAL=1, kEXTERNAL};

    /* 
     * Build on CObject error codes. 
     */
    enum HP3314_Errors {kERR_AMPLITUDE=ENONE+1, kERR_FREQUENCY, kERR_OFFSET, 
			kERR_PARSE, kERR_RANGE, kERR_VECTOR_HEIGHT,
			kERR_VECTOR_LENGTH, kERR_VECTOR_MODIFY,
			kERR_ARB_DT, kERR_NCYCLE};

    /**
     * Default Constructor
     * gpib_address - The GPIB address (1:31) that the device is set to.
     */ 
    HP3314A(int gpib_address);
    /// Default destructor
    ~HP3314A();

    /*!
     * Description: 
     *   Restore to presets.
     *
     * Arguments:
     *   None
     * 
     */
    inline bool Restore(void) {return Command("PR",NULL,0);};

    /*!
     * Description: 
     *   Bump the range of the current register up. 
     *
     * Arguments:
     *   NONE
     * 
     */
    inline bool RangeUP(void) {return Command("RU",NULL,0);};

    /*!
     * Description: 
     *   Bump the range of the current register down. 
     *
     * Arguments:
     *   NONE
     * 
     */
    inline bool RangeDown(void) {return Command("RD",NULL,0);};

    // inline functions for enable/disable Page 93 in manual.
    /*!
     * Description: 
     *   Based on the boolean parameter create the command to
     * turn amplitude modulation on (true) or off(false).
     * Page 56
     *
     * Input impedence    10kOHM
     * Input Sensitivity  2Vp-p = 100%
     * Modulation         0:100%
     * 3dB Bandwidth      DC to 100kHz
     *
     * Arguments:
     *   e - boolean true - enable, false - disable
     * 
     */
    inline bool  AM(bool e) {if(e) return Command("AM1",NULL,0); else
			       return Command("AM0",NULL,0);};
    /*!
     * Description: 
     *   Query the current state of the Amplitude modulation bit. 
     *
     * Arguments:
     *   NONE
     */
    inline bool  AM(void) const 
	{ if(Query(kAMPLITUDE_MODULATION)>0.0) return true; 
	    else return false; };
    /*!
     * Description: 
     *   Based on the boolean parameter create the command to
     * turn frequency modulation (FM) on (true) or off(false). 
     *
     * Arguments:
     *   e - boolean true - enable, false - disable
     * 
     */
    inline bool  FM(bool e)  
	{if(e) return Command("FM1",NULL,0); else
	     return Command("FM0",NULL,0);};
    /*!
     * Description: 
     *   Query the current state of the Frequency modulation bit. 
     *   Page 
     *   Input impedence    10kOHM
     *   Input Sensitivity  2Vp-p = 100%
     *   Modulation         0:100%
     *   3dB Bandwidth      DC to 100kHz
     *
     * Arguments:
     *   NONE
     */
    inline bool  FM(void) const 
	{ if(Query(kFREQUENCY_MODULATION)>0.0) return true; 
	    else return false; };
    /*!
     * Description: 
     *   Based on the boolean parameter create the command to
     *   enable function inversion  (true) or off(false). 
     *
     * Arguments:
     *   e - boolean true - enable, false - disable
     * 
     */
    inline bool  FunctionInvert(bool e) {if(e) return Command("FI1",NULL,0);
	else return Command("FI0",NULL,0);};
    /*!
     * Description: 
     *   Query the current state of the Amplitude modulation bit. 
     *
     * Arguments:
     *   NONE
     */
    inline bool  FunctionInvert(void) const { if(Query(kFUNCTION_INVERT)>0.0) 
	    return true; else return false; };

    /*!
     * Description: 
     *   Based on the boolean parameter create the command to
     *   enable manual sweep  (true) or off(false). 
     *
     * Arguments:
     *   e - boolean true - enable, false - disable
     * 
     */
    inline bool  ManualSweep(bool e) {if(e) return Command("MA1",NULL,0); else
			       return Command("MA0",NULL,0);};
    /*!
     * Description: 
     *   Query the current state of the Amplitude modulation bit. 
     *
     * Arguments:
     *   NONE
     */
    inline bool  ManualSweep(void) const 
	{ if(Query(kMANUAL_SWEEP)>0.0) return true; else return false; };

    /*!
     * Description: 
     *   Set the sweep. Permitted values are:
     *       kSWEEP_OFF
     *       kSWEEP_LINEAR
     *       kSWEEP_LOG
     *
     * Free-Run Linear sweep summary on page 11
     *    internal waveforms 11-15 use this
     * Free-Run Log sweep page 12
     * Examples start on page 26 of the manual. 
     *
     * Arguments:
     *   Enumerated sweep type, shown above.
     * 
     */
    bool Sweep(SWEEP val);
    /*!
     * Description: 
     *   Query the current sweep mode. 
     *   
     * Arguments:
     *   NONE
     */
    inline SWEEP Sweep(void) const { return (SWEEP) Query(kQSWEEP);};


    /*!
     *
     * Description: 
     *   kSWEEP_MASKED - Sweep always
     *   kSWEEP_START  - sweep at start
     *   kSWEEP_STOP   - sweep at stop
     *	 kSWEEP_EITHER - sweep either
     *
     * Arguments:
     *   value to set
     * 
     */
    bool SweepMask(SWEEP_MASK val);
    /*!
     * Description: 
     *   Query the current sweep mask.
     *
     * Arguments:
     *   NONE
     */
    inline double SweepMask(void) const { return Query(kSWEEP_MASK);};


    /*!
     * Description: 
     *   Set the sweep trigger interval. See page 96 for details. 
     *   Permitted prefixes are MS(milliseconds) and SN(seconds)
     *
     * Arguments:
     *   Time in seconds for interval. 
     * 
     */
    inline bool SweepTriggerInterval(double v) {return SetTime("TI",v,20.0e-3);};
    /*!
     * Description: 
     *   Query the current sweep trigger interval. 
     *
     * Arguments:
     *   NONE
     */
    inline double SweepTriggerInterval(void) const
	{return Query(kQTRIGGER_INTERVAL);};

    /*!
     * Description: 
     *   Setup for a sweep. Specifically set the start frequency. 
     *
     * Arguments:
     *   F {0.01Hz, 19.99MHz}
     * 
     */
    inline bool StartFrequency(double F)  {return SetF("ST", F);};
    /*!
     * Description: 
     *   Query the current start sweep frequency in Hz. 
     *
     * Arguments:
     *   NONE
     */
    inline double StartFrequency(void) const {return Query(kSTART_FREQUENCY);};


    /*!
     *
     * Description: 
     *   Set the Stop frequency for a sweep.
     *
     * Arguments:
     *   F - Start Frequency {0.01Hz, 19.99MHz}
     * 
     */
    inline bool StopFrequency(double F)   {return SetF("SP", F);};
    /*!
     * Description: 
     *   Query the current start sweep frequency in Hz. 
     *
     * Arguments:
     *   NONE
     */
    inline double StopFrequency(void) const { return Query(kSTOP_FREQUENCY);};



    /*!
     * Description: 
     *   Based on the boolean parameter create the command to
     *   enable VCO  (true) or off(false). 
     *   page 59
     *
     *   Input impedence    10kOHM
     *   Input Sensitivity  10% per volt
     *   Specified Linear Range: 10:80%
     *   3dB Bandwidth      DC to 100kHz
     *   INPUT CONNECTOR is FM/VCO
     *
     * Arguments:
     *   e - boolean true - enable, false - disable
     * 
     */
    inline bool  VCO(bool e) {if(e) return Command("VC1",NULL,0); else
			       return Command("VC0",NULL,0);};
    /*!
     * Description: 
     *   Query the current state of the VCO bit.
     *
     * Arguments:
     *   NONE
     */
    inline bool  VCO(void) const 
	{ if(Query(kQVCO)>0.0) return true; else return false; };


    // Complex set functions ------------------------------------------------

    /*!
     * Description: 
     *   Set the output Function wavform. The choices are: 
     *       kDCONLY
     *       kSINE
     *       kSQUARE
     *       kTRIANGLE
     *
     * Arguments:
     *   Waveform from list above. 
     * 
     */
    bool Waveform(WAVEFORM val);

    /*!
     * Description: 
     *   Query the current waveform selected to output
     *   Apparently not supported. 
     * Arguments:
     *   NONE
     */
    inline WAVEFORM Waveform(void)  const 
	{return (WAVEFORM)Query(kFUNCTION_SELECT);};


    /*!
     * Description: 
     *   The modes are enumerated in the MODE enum above. Modes available are:
     *   FREERUN - run sine, square and triangle waves etc. continious or 
     *             swept. This is the most common use. 
     *
     *   GATE    - The output signal is gated
     *
     *   N_CYCLE - The output signal is a counted burst of N cycles
     *
     *   HALF_CYCLE - The output signal is alternate half cycles
     *
     *   FinXN   - The output frequency is locked to and N times the 
     *             reference frequency. 
     *
     *   FinOverN - The output frequency is locked to and 1/N times the 
     *              reference frequency. 
     *
     *   ARB      - the user provides buffers of arbitrary waveforms. 
     * 
     *   This is documented in the HP3314A operator manual,
     *   page 61
     *
     * Arguments:
     *   val - One of the mode values as shown above. 
     * 
     */
    bool Mode(MODE val);
    /*!
     * Description: 
     *   Query the current function generator mode.
     *   
     * Arguments:
     *   NONE
     */
    inline MODE Mode(void) const { return (MODE) Query(kQMODE);};

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     * 
     */
    bool PLLMask(PLLMASK val);

    /*!
     * Description: 
     *   Issue a software trigger. 
     *
     * Arguments:
     *     NONE (UNTESTED) 
     * 
     */


    /*
     * ====================== TRIGGER FUNCTIONS =================
     */
    inline bool Trigger(void) {return Command("MN",NULL,0);};
    
    /*!
     * Description: 
     *   Set the threshold on the trigge input
     *
     * Arguments:
     *   val one of: kPLUS_ONE=1, kZERO_VOLTS
     * 
     */
    bool TriggerThreshold(TRIGGER_THRESHOLD val);
    /*!
     * Description: 
     *   Query the current trigger threshold value
     *   
     * Arguments:
     *   NONE
     */
    inline MODE TriggerThreshold(void) const
	{ return (MODE) Query(kQTRIGGER_THRESHOLD);};


    /*!
     * Description: 
     *   Set the slope on the trigger input. 
     *
     * Arguments:
     *   val one of: kPOSITIVE=1, kNEGATIVE
     * 
     */
    bool TriggerSlope(TRIGGER_SLOPE val);
    /*!
     * Description: 
     *   Query the current trigger slope value
     *   
     * Arguments:
     *   NONE
     */
    inline MODE TriggerSlope(void) const
	{ return (MODE) Query(kQTRIGGER_SLOPE);};


    /*!
     * Description: 
     *   set trigger input source. 
     *
     * Arguments:
     *   val one of kINTERNAL=1, kEXTERNAL
     * 
     */
    bool TriggerSource(TRIGGER_SOURCE val);
    /*!
     * Description: 
     *   Query the current trigger input source
     *   
     * Arguments:
     *   NONE
     */
    inline TRIGGER_SOURCE TriggerSource(void) const
	{ return (TRIGGER_SOURCE) Query(kQTRIGGER_SOURCE);};


    // Use the generator
    /*!
     * Description: 
     *   Set the amplitude output of the function generator. There are 4 
     *   distinct ranges for the system:
     *   HPIB Number   Range    Min     Max     Attenuator
     *   -----------   -----    ---     ---     ----------
     *   1             10mV     0.01mV  10.00mV 60dB
     *   2            100mV     10mV    100.0mV 40dB
     *   3              1V      .100V   1.000V  20dB
     *   4             10V      1.00V   10.0V    0dB
     *
     *   The amplitude the user provides is input as a double and 
     *   converted approprately to set the unit. The steps happen internal
     *   to the unit. 
     *   3.5 digits of resolution. 
     *   This is documented in the HP3314A operator manual,
     *   page 52
     *
     * Arguments:
     *   Amplitude in volts to set based on the constraints outlined above.
     *   0.01mV to 10V range. 
     *
     */
    bool Amplitude(double A);

    /*!
     * Description: 
     *   Query the current output amplitude. 
     *
     * Arguments:
     *   NONE
     */
    inline double Amplitude(void) const { return Query(kAMPLITUDE);};

    /*!
     * Description: 
     *   Set the current Range hold value, 0(off) to 8, page 52
     *        Maximum   Minimum   Resolution   Minimum with Range Hold
     *   0    OFF
     *   1     1.999Hz     0.001Hz     0.001Hz  0.001Hz
     *   2     19.99Hz     1.50Hz      0.01Hz   0.01Hz
     *   3     199.9Hz     15.0Hz      0.1Hz    0.1Hz
     *   4    1.999kHz      150Hz      1Hz      1Hz
     *   5    19.99kHz    1.50kHz     10Hz     10Hz
     *   6    199.9KHz    15.0kHz    100Hz    100Hz
     *   7    1.999MHz    150kHz       1kHz     1kHz
     *   8    19.99MHz    1.50MHz     10kHz    10KHz
     *
     * Arguments:
     *   NONE
     */
    bool Range(int val = 0);

    /*!
     *
     * Description: 
     *   Set the output frequency for the function generator. 
     *   For the continious mode this is just the basic value. 
     *   This is documented in the HP3314A operator manual,
     *   page 52.
     *
     *   {0.01Hz:19.99MHz}
     *
     * Arguments:
     *   F - the frequency in Hz that the user decides as output. 
     * 
     */
    inline bool Frequency(double F)       {return SetF("FR",F);};

    /*!
     * Description: 
     *   Query the current continious frequency in Hz. 
     *
     * Arguments:
     *   NONE
     */
    inline double Frequency(void) const { return Query(kFREQUENCY);};


    /*!
     *
     * Description: 
     *   
     *
     * Arguments:
     *   
     * 
     */
    inline bool MarkerFrequency(double F) {return SetF("MK", F);};
    /*!
     *
     * Description: 
     *   Get the current marker frequency. 
     *
     * Arguments:
     *   
     * 
     */
    inline double MarkerFrequency(void) const 
	{return Query(kMARKER_FREQUENCY);};

    /*!
     *
     * Description: 
     *    Used in N-Cycle modes. Sets the number of cycles 
     *    in a burst. 
     *
     * Arguments:
     *   val {1:1999}
     * 
     */
    bool NCycles(unsigned int N);
    /*!
     *
     * Description: 
     *   Get the number of cycles in a burst.
     *
     * Arguments:
     *   none
     * 
     */
    inline unsigned int NCycles(void) const 
	{return (unsigned int)Query(kQN);};


    /*!
     * Description: 
     *   Set the DC offset on tht output stage.
     *
     *   This is documented in the HP3314A operator manual,
     *   page 53
     *  Range     Maximum   Minimum   Resolution
     *  1         +/-1.997  0.0       0.002
     *  2         +/-5.00   2.00      0.01
     *
     * Arguments:
     *    val - a value in volts to offset the output to. {-5.0, 5.0}
     *
     * Returns:
     *    true on success. 
     * 
     */
    bool DCOffset(double val);
    /*!
     * Description: 
     *   Query the current continious frequency in Hz. 
     *
     * Arguments:
     *   NONE
     */
    inline double DCOffset(void) const { return Query(kOFFSET);};

    /*!
     *
     * Description: 
     *   Set the phase value in degrees. 
     *
     * Arguments:
     *   val {-90:90} degrees
     * 
     */

    inline bool Phase(double val);
    /*!
     * Description: 
     *   Query the current phase values
     *
     * Arguments:
     *   NONE
     */
    inline double Phase(void) const { return Query(kPHASE);};


    /*!
     * Description: 
     *   Set the Symmetry of the output waveform
     *
     * Arguments:
     *   percent {10%,90%}
     * 
     */
    bool Symmetry(unsigned char percent);

    /*!
     * Description: 
     *   Query the current symmetry in percent. 
     *
     * Arguments:
     *   NONE
     */
    inline double Symmetry(void) const { return Query(kSYMMETRY);};


    // documentation claims 6 waveforms.
    // Page 19 is details about ARB
    // Page 78 in manual about storing and recalling waveforms.
    // dt form 0.2 to 19.99ms
    // height 0:+/- 1999
    // vector length 1:127
    // Marker range 1:150
    // INS/DEL insert/delete vector at marker location
    // FREQ 0.002 to 2.5KHz ?
    // Amplitude 0.01mV:10V pp
    // OFFSET 0:5VDC
    // Phase +/- 90 degrees


    /* --------------------- ARB SECTION ----------------------- */
    /* 
     * Page 19 
     * Page 43
     * Arb summary
     * 160 vectors (height,len) across 6 total waveforms. 
     * VHEIGHT {-1999:1999}
     * VLEN {1:127}
     * DT {0.2:19.99} ms
     */

    /*!
     * Description: 
     *   Set the arb state, choose one of:
     *   
     *
     * Arguments:
     *   val - kARB_OFF, kON, kARB_ON_INIT
     * 
     */
    bool ARBMode(ARB_MODE val);
    /*!
     * Description: 
     *   Query the current ARB mode
     *   
     * Arguments:
     *   NONE
     */
    inline double ARBMode(void) const { return Query(kARB_MODE);};


    /*!
     * Description: 
     *   Setup for a sweep. Specifically set time delta. 
     *
     * Arguments:
     *   DT {0.200:19.99} milli-seconds
     * 
     */
    bool ARB_DT(double dt);
    /*!
     * Description: 
     *   Query the current dt value, this is used for the ARB case
     *   seconds
     * Arguments:
     *   NONE
     */
    inline double ARB_DT(void) const { return Query(kARB_DT);};

    /*!
     * Description: 
     *   Recall the current stored waveform. 
     *
     * Arguments:
     *   Waveform {0-5}
     * 
     */
    bool ARB_Wave(unsigned char Number);

    /*!
     * Description: 
     *   Query the current ARB waveform in use: {0:5}
     *
     * Arguments:
     *   NONE
     */
    inline double ARB_Wave(void) const { return Query(kARB_WAVE_NUMBER);};


    /*!
     * Description: 
     *   Set the height of the current vector selected by the 
     *   VectorMarker command.
     *
     * Arguments:
     *   val {-1999:1999} (unitless)
     * 
     */
    bool VectorHeight(int val);
    /*!
     * Description: 
     *   Query the current Vector height value
     *   unitless
     * Arguments:
     *   NONE
     */
    inline int VectorHeight(void) const { return (int)Query(kVECTOR_HEIGHT);};

    /*!
     * Description: 
     *   Set the current vector as set by VectorMarker length
     *
     * Arguments:
     *   val {0:127}
     * 
     */
    bool VectorLength(unsigned int val);
    /*!
     * Description: 
     *   Query the current Vector length value in samples 
     *   
     * Arguments:
     *   NONE
     */
    inline unsigned int VectorLength(void)  const
	{ return (unsigned int)Query(kVECTOR_LENGTH);};

    /*!
     * Description: 
     *   specify the vector location. {0:150}
     *
     * Arguments:
     *   vect
     * 
     */
    bool VectorMarker(unsigned int v); 

    /*!
     * Description: 
     *   Query the current Vector pointer in the overall queue
     *   seconds
     * Arguments:
     *   NONE
     */
    inline double VectorMarker(void) const { return Query(kVECTOR_MARKER);};

    /*!
     * Description: 
     *  Use Height and Length to setup the vector at the current marker
     *  location then call this to insert it into the output stream. 
     *
     * Arguments:
     *   NONE
     * 
     */
    inline bool InsertVector(void) {return Command("IV", NULL, 0);};
    /*!
     * Description: 
     *   Deletes the vector at the current marker position. 
     *
     * Arguments:
     *   NONE
     * 
     */
    inline bool DeleteVector(void) {return Command("DV", NULL, 0);};

    /* 
     * There are a bunch of pre-programmed arb sequences included in 
     * the system - see page: 22 in the manual. 
     */


    /* ---------------------- HELPER FUNCTIONS ---------------- */

    /*!
     * Description: 
     *   The base function to query a value. 
     *
     * Arguments:
     *   something from the query set. 
     * 
     */
    double Query(QUERYS q) const;


    /*!
     * Description: 
     *   Helper functon to load up a full waveform if predefined. 
     *
     * Arguments:
     *   Height - an array of length Nsamples of heights to load {-1999:1999}
     *   Length - an array of length Nsamples of time intervals {1:127}
     *   NSamples - number of samples to load. 
     *   Append   - Reset marker (false) to the first position if true
     *              otherwise this will append (true). 
     *
     * returns:
     *    The total number of time bins in the waveform. 
     */
    unsigned int LoadWaveForm( const int Height[], 
			       const unsigned int Length[], 
			       unsigned int Nsamples,
			       bool Append = false);

    /*!
     * Description: 
     *   Helper functon that fills provided arrays with
     *   the waveform information for diagnostic puroses. 
     *
     * Arguments:
     *   Height - an array of length Nsamples of heights to load {-1999:1999}
     *   Length - an array of length Nsamples of time intervals {1:127}
     *   NSamples - size of provided arrays. 
     *
     * returns:
     *    The entries in the arrays. 0 on failure. 
     */
    size_t GetWaveForm( int *Height, unsigned int *Length, size_t N);

    void DumpWaveForm(void);

    const char* Version(void) const;

    /*!
     * format current state
     */
    friend std::ostream& operator<<(std::ostream& output, const HP3314A &p);


    /*!
     * Certain queries have associated units. Get it. 
     */
    inline const char* Units(void) const {return fSuffix;};


    /*! Helper function for mode string */
    const char *ModeStr(MODE m) const;

    /*! Helper function for Function string */
    const char *FunctionStr(WAVEFORM f);

    const char *ErrorCode(unsigned char val);

    /* ============================================================= */
private:
    /*!
     * Description: 
     *   A helper function that is at the base of all commands that need
     *   to format a frequency. 
     *
     * Arguments:
     *   prefix - The prefix that is specific to the command being issued. 
     *   value  - the double value {0.01Hz,19.99MHz}
     * 
     */
    bool SetF(const char *prefix, double value); // All frequency sets

    /*!
     *
     * Description: 
     *   
     *
     * Arguments:
     *   
     * 
     */
    bool SetValue(const char *prefix, double value, const char *suffix, double upper);

    /*!
     *
     * Description: 
     *   
     *
     * Arguments:
     *   
     * 
     */
    bool SetTime(const char *prefix, double value, double upper);

    /*!
     *
     * Description: 
     *   Helper function for Vector calls. 
     *
     * Arguments:
     *   
     * 
     */
    bool SetVector(const char *prefix, double value, double upper);

    /*
     * Helper function, check on the prefix and suffix and return
     * the resulting value. 
     */
    double ParseValue(const char *string, const char *prefix, const char *suffix) const;

    char fPrefix[4], fSuffix[4];
    bool fRangeHold;
};
#endif
