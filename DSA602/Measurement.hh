/**
 ******************************************************************
 *
 * Module Name : Measurement.hh
 *
 * Author/Date : C.B. Lirakis / 24-Dec-22
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602 Programmers Reference manual, page: 191
 *
 *******************************************************************
 */
#ifndef __MEASUREMENT_hh_
#define __MEASUREMENT_hh_
#  include "CObject.hh"
#  include "MeasurementA.hh"

/*!
 * Manage measurements that pertain to the trace on the screen. 
 *
 * DSA602 Programming manual, 
 *  MS <meas>    page 203  Query only the measurement in question. 
 *  MSLIST       page 201  Set up to 6 possible measurements to be performed 
 *                         on the waveform. 
 *  MEAS?        page 192  Executes the commands.  This is complex 
 *  MSN          page 205  Number of measurements currently programmed into
 *                         the unit. 
 *
 * Other files/classes. 
 *  MValue - a way of storing measurements with a qualifier. 
 *  GParse - A way to parse values. 
 */
class Measurement : public CObject
{
public:
    /*!
     * Maximum number of requests to ask for. 
     */
    static const int8_t kMaxReadout = 6;
    /*!
     * Number of possible measurements from a trace on the screen. 
     */
    static const uint8_t kNMeasurements = 27;
    /*!
     * Pointers into those types. 
     */
    enum MTYPES {
	// Amplitude
	kGAIN=0, kMAX, kMEAN, kMID, kMIN, kOVERSHOOT, kPP, kRMS, kUNDERSHOOT,
	// Area/Energy
	kYTENERGY, kYTMNS_AREA, kYTPLS_AREA, 
	// Frequency Domain
	kSFREQ, kSMAG, kTHD, 
	// Timing/Frequency
	kCROSS, kDELAY, kDUTY, kFALLTIME, kFREQ, kPDELAY, kPERIOD, 
	kPHASE, kRISETIME, kSKEW, kTTRIG, kWIDTH
    };
    /*
     * List of avaible things to set and or read on any waveform. 
     * Add +1 to be NULL terminated. 
     */
     const char *Available[kNMeasurements] = {
	 "GAIN", "MAX", "MEAN", "MID", "MIN", "OVERSHOOT","PP", "RMS","UNDERSHOOT",
	 "YTENERGY", "YTMNS_AREA", "YTPLS_AREA",
	 "SFREQ","SMAG","THD",
	 "CROSS", "DELAY", "DUTY", "FALLTIME", "FREQ", "PDELAY","PERIOD",
	 "PHASE","RISETIME", "SKEW", "TTRIG","WIDTH"
     };

    /*!
     * Description: 
     *   Default Constructor for measurements. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    Measurement(void);

    /*!
     * Description: 
     *   Default destructor for measurements. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    ~Measurement(void);


    /* ======================================================== */
    /*                      FUNCTIONS                           */ 
    /* ======================================================== */

    /*!
     * Description: 
     *   Query all items in the MSLIST 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    bool Update(void);

    /*!
     * Description: 
     *   Query The specific requested value. 
     *
     * Arguments:
     *   t - type of measurement to perform, see above MTYPES enum. 
     *   q - perform query (default false)
     *
     * returns:
     *    value - Value and value qualifier. 
     */
    double GetValue(MTYPES t, bool q=false)  
	{if(q) Query(Available[t]); return fMeasurements[t]->Value();};

    /*!
     * Description: 
     *   Ask the scope to set this as an active readout value
     *
     * Arguments:
     *   Which one?
     *
     * returns:
     *   true on success.
     */
    bool SetActive(MTYPES type);

    /*!
     * Description: 
     *   Take the id specified out of the active list. 
     *
     * Arguments:
     *   Which one?
     *
     * returns:
     *   true on success.
     */
    bool SetInactive(MTYPES type);


    /*!
     * Description: 
     *   Query how many of the measurements are currently active, 
     *   This is based on what the scope thinks. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    number of available measurements. 
     */
    uint32_t NList(void);

    /* Same but based on this software. number locally maintained. */
    inline uint8_t  ActiveCount(void) const {return fNActive;};

    /*!
     * Description: 
     *   Query the scope which of the measurements are currently active, 
     *   This will also fill the fActive array. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    number of available measurements. 
     */
    uint32_t ActiveList(void);

    /*!
     * Description: 
     *   Get the id associated with the index into the fActive array. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    id based on enum MTYPES 
     */
    inline const uint8_t ActiveIndex(uint8_t i) {return fActive[i];};

    /*!
     * Description: 
     *   set the index value in the array of fActive. 
     *   returns false if this is full. 
     *
     * Arguments:
     *   index to put in array
     *
     * returns:
     *    false on failure. Array full. 
     */
     bool SetActiveIndex(uint8_t idx);

    /*!
     * Description: 
     *   Find the data (MeasurementA) in the array by name. 
     *
     * Arguments:
     *   name - name to search for in the array. 
     *
     * returns:
     *   MeasuerementA associated with that name. Returns NULL if not found. 
     */
    MeasurementA*  Find(const char *name);

    /*!
     * Description: 
     *   Return the MeasurementA data referenced by index. 
     *   The index value follows MTYPES. 
     *
     * Arguments:
     *   idx - index to reference the data. 
     *
     * returns:
     *   MeasurementA class structure. 
     */
    inline MeasurementA* GetByIndex(MTYPES idx) {return fMeasurements[idx];};

    /*!
     * Description: 
     *   Clear all the active elements in the measurement array. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *   NONE
     */
    void  Clear(void);

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
    friend ostream& operator<<(ostream& output, const Measurement &n); 

private:

    bool Query(const char *Command);

    void Decode(const char *Command, const char* Response);

    /*!
     * Keep track of all possible measurements. 
     */
    MeasurementA* fMeasurements[kNMeasurements];

    /*!
     * Which measurements are active?
     */
    int8_t  fActive[kMaxReadout];
    uint8_t fNActive;   // Count on above. 

    /*!
     * How many of these are there? 
     * Group by what they do. 27 total values, see enum above. 
     */

};
#endif
