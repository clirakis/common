/**
 ******************************************************************
 *
 * Module Name : Channel.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : Channel, stores data on a per channel basis
 * Data for various plug ins:
 *        Sensitivity Range  Offset Range
 *        -----------------  ------------
 * 11A32    1e-3:99.5e-3        -1:1
 * 11A34  100e-3:995e-3        -10:10 
 * 11A52       1:10           -100:100 
 *
 * 11A71 single ended amplifier: range = -10*(SEN):10*(SEN)
 * 11A72 single ended amplifier: range = -25*(SEN):25*(SEN)
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 * Summary on page: 31 (Channel/Vertical)
 * Full explaination of sub commands begins on page: 78
 *
 * We haven't provided for queries of the type: CH<slot><ui>?
 *
 * This is subclassed to Module.xxx
 * 
 *******************************************************************
 */
#ifndef __CHANNEL_hh_
#define __CHANNEL_hh_
#  include <cfloat>
#  include <cstdint>
#  include "DSA602_Types.hh"
#  include "CObject.hh"

/*
 * Typical channel query data. issued the command CH? to get this data.
 * 
 * CHL1 MNSCOUPLING:DC,PLSCOUPLING:DC,PROTECT:OFF,OFFSET:0.0E+0,AMPOFFSET:0.0E+0,BW:2.0E+7,IMPEDANCE:5.0E+1,MNSOFFSET:0.0E+0,MNSPROBE:"NONE",PLSOFFSET:0.0E+0,PLSPROBE:"NONE",SENSITIVITY:1.0E+0,UNITS:"VOLTS",VCOFFSET:0.0E+0;CHC1 MNSCOUPLING:DC,PLSCOUPLING:DC,PROTECT:ON,OFFSET:0.0E+0,AMPOFFSET:0.0E+0,BW:1.5E+8,IMPEDANCE:1.0E+6,MNSOFFSET:0.0E+0,MNSPROBE:"NONE",PLSOFFSET:0.0E+0,PLSPROBE:"NONE",SENSITIVITY:1.0E+0,UNITS:"VOLTS",VCOFFSET:0.0E+0;CHR1 COUPLING:DC,OFFSET:2.35E-1,BW:4.0E+8,IMPEDANCE:1.0E+6,PROBE:"NONE",SENSITIVITY:1.0E-1,UNITS:"VOLTS";CHR2 COUPLING:DC,OFFSET:0.0E+0,BW:4.0E+8,IMPEDANCE:1.0E+6,PROBE:"NONE",SENSITIVITY:1.0E+0,UNITS:"VOLTS
 * 
 * looks like channels are seperated by ;
 * data seperated by commas
 * data label and value seperated by :
 * 
 * There are 3 possible channel slots (P)osition: Left, Center and Right 
 * then (N)umber 
 * CH(P)(N)
 */

class Channel : CObject 
{
public:
    /*!
     * Channel spefic commands.
     * page 77 in manual.  
     */
    enum COMMANDs { kCCOUPLING=0 , kPROBE, 
		    kAMPOFFSET, kPLSCOUPLING, kMNSCOUPLING, kPLSOFFSET,
		    kMNSOFFSET,  kPLSPROBE, kMNSPROBE, kPROTECT, kVCOFFSET,
		    kBW, kIMPEDANCE, kBWHI, kOFFSET, kBWLO, kSENSITIVITY,
		    kUNITS, 
		    kCHNL_END};

    static const uint8_t kNSINGLE_ENDED = 9;
    static const uint8_t kNDIFFERENTIAL = 16;
    const char *Labels[kCHNL_END] = {
	// Non differential
	"COUPLING", "PROBE",
	// Differential only
	"AMPOFFSET", "PLSCOUPLING", "MSCOUPLING", "PLSOFFSET",
	"MNSOFFSET", "PLSPROBE", "MNSPROBE", "PROTECT", "VCOFFSET",
	// ALL
	"BW", "IMPEDANCE", "BWHI", "OFFSET", "BWLOW", "SENSITIVITY",
	"UNITS"
    };


    /*!
     * Description: 
     *   Constuctor
     *
     * Arguments:
     *   m - the parent module
     *   c - the channel number
     *
     * returns:
     *    ...
     */
    Channel(SLOT s=kSLOT_NONE, uint8_t n=0);

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
    ~Channel();
    /* ======================================================== */
    /*             Private Data Access Functions                */ 
    /* ======================================================== */

    static const struct t_Commands ChannelCommands[ kCHNL_END];

    /*!
     * Description: 
     *    Given the specific enum in COMMANDS is this
     *    command applicable to this channel. This is module specific
     *    and depends on if it is differential or not. 
     *
     * Arguments:
     *   COMMAND
     *
     * returns:
     *    true if applicable
     */
    bool Applicable(uint8_t c);

    inline uint8_t NApplicable(void) const {
	if (fDifferential) return kNDIFFERENTIAL; else return kNSINGLE_ENDED;};

    
    /*!
     * Description: 
     *    Get the channel number.    
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    channel number for this class {0:1} 
     */
    inline unsigned char Number(void) const {return fNumber;};

    /*!
     * Description: 
     *   Set the channel number; 
     *
     * Arguments:
     *   channel number {0:1}
     *
     * returns:
     *    NONE
     */
    inline void Number(unsigned char c)  {if (c<2) fNumber = c;};

    /*!
     * Description: 
     *   Set if the module is differential. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    inline void SetDifferential(void)  {fDifferential = true;};
    inline bool IsDifferential(void) {return fDifferential;};

    /*!
     * Description: 
     *   Sets the voltages to be subtracted from the input signal, 
     *   after the plus and minus differentential input signals have been
     *   subratracted from each other. AMPoffset vertically positions 
     *   the signal on the display. APPLIES TO DIFFERENTIAL AMPS ONLY. 
     *   Page 79 top
     *
     * Arguments:
     *   Offset Voltage {-1.0:1.0}
     *
     * returns:
     *    true - on success. 
     */
    inline bool AmpOffset(double v) 
	{if(!fDifferential) return false; return SendCommand(kAMPOFFSET, v);};

    /*!
     * Description: 
     *   Queries the voltages to be subtracted from the input signal, 
     *   after the plus and minus differentential input signals have been
     *   subratracted from each other. AMPoffset vertically positions 
     *   the signal on the display. APPLIES TO DIFFERENTIAL AMPS ONLY. 
     *
     *   CH<slot><ui><sp>AMP:<NRx>
     *   Example command "CHR1 AMP:1.0"
     *
     * Arguments:
     *   q - query if false, just return the last value. 
     *
     * returns:
     *    the amplifier offset. 
     */
    double AmpOffset(bool q=false); 

    /*!
     * Description: 
     *   Query the channel bandwidth
     *
     * Arguments:
     *   q: true - issue a query, false - just return the last data. 
     *
     * returns:
     *    double value for bandwidth
     */
    inline double Bandwidth(bool q=false)  {if(q) Query(kBW); return fBW;};
    /*!
     * Description: 
     *   Set the bandwidth {2.0e7:0.0} DOES NOT WORK
     *
     * Arguments:
     *   Desired bandwidth
     *
     * returns:
     *    true on success. 
     */
    inline bool Bandwidth(double v) {return SendCommand(kBW, v);};

    /*!
     * Description: 
     *   Query the channel bandwidth hi
     *   doesn't seem to apply to the modules in the system. 
     *
     * Arguments:
     *   q: true - issue a query, false - just return the last data. 
     *
     * returns:
     *    double value for bandwidth
     */
    inline double Bandwidth_HI(bool q=false) 
	{if(q) Query(kBWHI); return fBWHi;};
    /*!
     * Description: 
     *   Set the bandwidth {2.0e7:0.0} DOES NOT WORK
     *
     * Arguments:
     *   Desired bandwidth
     *
     * returns:
     *    true on success. 
     */
    inline bool Bandwidth_HI(double v) {return SendCommand(kBWHI, v);};

    /*!
     * Description: 
     *   Query the channel bandwidth hi
     *   doesn't seem to apply to the modules in the system. 
     *
     * Arguments:
     *   q: true - issue a query, false - just return the last data. 
     *
     * returns:
     *    double value for bandwidth
     */
    inline double Bandwidth_Lo(bool q=false) 
	{if(q) Query(kBWLO); return fBWLo;};
    /*!
     * Description: 
     *   Set the bandwidth {2.0e7:0.0} DOES NOT WORK
     *
     * Arguments:
     *   Desired bandwidth
     *
     * returns:
     *    true on success. 
     */
    inline bool Bandwidth_Lo(double v) {return SendCommand(kBWLO, v);};

    /*!
     * Description: 
     *     Query the amplifier coupling. Only applies to non-differential
     *     modules. EG: 11A32    
     *
     * Arguments:
     *   q - issue query if true, otherwise return value from last query. 
     *
     * returns:
     *    COUPLING value
     *    {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     */
    COUPLING  Coupling(bool q); 

    /*!
     * Description: 
     *     Set the amplifier coupling. Only applies to non-differential
     *     modules. EG: 11A32    
     *
     * Arguments:
     *   COUPLING - one of the values in
     *      {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     *
     * returns:
     *    true on success
     *    false on failure, may mean does not apply. 
     *    
     */
    inline bool Coupling(COUPLING c) 
	{if (fDifferential) return false; 
	    return SendCommand(kCCOUPLING, c);};

    /*!
     * Description: 
     *   Query the current channel impedence. 
     *
     * Arguments:
     *   q - if set to true, perform query otherwise just return the
     *       value from the last query. 
     *
     * returns:
     *    Channel impedence {kFIFTY=0, kONE_MEG, kONE_GIG}
     */
    inline IMPEDANCE Impedance(bool q) 
	{if(q) Query(kIMPEDANCE);return fIMPedance;};
    /*!
     * Description: 
     *   set the current channel impedence. 
     *
     * Arguments:
     *   Channel impedence {kFIFTY=0, kONE_MEG, kONE_GIG}
     *
     * returns:
     *    true on success
     */
    bool Impedance(IMPEDANCE v);

    /* ======================================================== */
    /*             Differential unit data                       */ 
    /* ======================================================== */

    /*!
     * Description: 
     *    Sets the minus input coupling of the specified channel. 
     *    This applies to differential channels only. 
     *
     * Arguments:
     *   COUPLING one of 
     *     {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     *
     * returns:
     *    true on success
     */
    inline bool MNS_Coupling(COUPLING c) 
	{if (fDifferential) return SendCommand(kMNSCOUPLING,c); return false;};
    /*!
     * Description: 
     *    Queries the minus input coupling of the specified channel. 
     *    This applies to differential channels only. 
     *
     * Arguments:
     *    q - true - query the setting otherwise just return 
     *               the last queried value. 
     *
     * returns:
     *     COUPLING one of 
     *     {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     */
    inline COUPLING MNS_Coupling(bool q) 
	{if (fDifferential) {if(q) Query(kMNSCOUPLING);  return fMNSCoupling;} return kCOUPLING_NONE;};

    /*!
     * Description: 
     *   Queries the probe offset voltage that will be subtracted 
     *   from the minus input of the specified channel. This
     *   applies to differential amplifiers only. 
     *
     * Arguments:
     *   bool if true query the value, if false just return the last
     *   interrogated value. 
     *
     * returns:
     *    MNS Offset value
     */
    inline double MNS_Offset(bool q) 
	{if (fDifferential) {if(q) Query(kMNSOFFSET);return fMNSOffset;} return -DBL_MAX;};
    /*!
     * Description: 
     *   Sets the probe offset voltage that will be subtracted 
     *   from the minus input of the specified channel. This
     *   applies to differential amplifiers only. 
     *
     * Arguments:
     *   MNS Offset value
     *
     * returns:
     *    true on success
     */
    inline bool MNS_Offset(double v) 
	{if (fDifferential) {return SendCommand(kMNSOFFSET,v);} return false;};

    /*!
     * Description: 
     *   NOT IMPLEMENTED I DON"T HAVE THE PROBES
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const char* MNS_Probe(void) const {return fMNSProbe;};

    /*!
     * Description:
     *   query
     *   The differential offset link modifies the AMPoffset, MNSOffset
     *   PLSOffset or VCOffset links depending on coupling an probes.
     *   Refer to the module manual for details. 
     *
     *   Based on SENsitivy setting
     *   Sensitivity Range     Offset Range
     *     {1e-3:99.5e-3}       {-1:1}
     *   {100e-3:995e-3}       {-10:10}
     *        {1:10}          {-100:100}
     * Arguments:
     *   q = interrogate otherwise just return value
     *
     * returns:
     *    value or -DBL_MAX if non differential
     */
    inline double Offset(bool q)
	{if(q) Query(kOFFSET);return fOFFSet;};

    /*!
     * Description:
     *   Sets input vertial offset 
     *   The differential offset link modifies the AMPoffset, MNSOffset
     *   PLSOffset or VCOffset links depending on coupling an probes.
     *   Refer to the module manual for details. 
     *
     *   Based on SENsitivy setting
     *   Sensitivity Range     Offset Range
     *     {1e-3:99.5e-3}       {-1:1}
     *   {100e-3:995e-3}       {-10:10}
     *        {1:10}          {-100:100}
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool Offset(double v)
	{if (fDifferential) {return SendCommand(kOFFSET,v);}return false;};


    /*!
     *    Queries the plus input coupling of the specified channel. 
     *    This applies to differential channels only. 
     *
     * Arguments:
     *    q - true - query the setting otherwise just return 
     *               the last queried value. 
     * returns:
     *     COUPLING one of 
     *     {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     */
    inline COUPLING PLS_Coupling(bool q) 
	{if (fDifferential) {if(q) Query(kPLSCOUPLING); return fPLSCoupling;} return kCOUPLING_NONE;};

    /*!
     *    Sets the plus input coupling of the specified channel. 
     *    This applies to differential channels only. 
     *
     * Arguments:
     *    q - true - query the setting otherwise just return 
     *               the last queried value. 
     * returns:
     *     COUPLING one of 
     *     {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     */
    inline bool PLS_Coupling(COUPLING c) 
	{if (fDifferential) return SendCommand(kPLSCOUPLING,c); return false;};


    /*!
     * Description: 
     *   Queries the probe offset voltage that will be subtracted 
     *   from the plus input of the specified channel. This
     *   applies to differential amplifiers only. 
     *
     * Arguments:
     *   bool if true query the value, if false just return the last
     *   interrogated value. 
     *
     * returns:
     *    MNS Offset value
     */
    inline double PLS_Offset(bool q) 
	{if (fDifferential) {if(q) Query(kPLSOFFSET);return fPLSOffset;} return -DBL_MAX;};
    /*!
     * Description: 
     *   Sets the probe offset voltage that will be subtracted 
     *   from the Plus input of the specified channel. This
     *   applies to differential amplifiers only. 
     *
     * Arguments:
     *   PLS Offset value
     *
     * returns:
     *    true on success
     */
    inline bool PLS_Offset(double v) 
	{if (fDifferential) {return SendCommand(kPLSOFFSET,v);} return false;};

    /*!
     * Description: 
     *  NOT IMPLEMENTED - I DONT HAVE THE PROBES   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const char* PLS_Probe() const {return fPLSProbe;};
    //
    /*!
     * Description: 
     *   NOT IMPLEMENTED 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline const char* Probe()     const {return fPROBe;};

    /*!
     * Description: 
     *   query the protection value
     *
     * Arguments:
     *   perform query or not. 
     *
     * returns:
     *    Status true - on, false - off
     */
    inline bool Protect(bool q) 
	{if(fDifferential) {if(q) Query(kPROTECT); return fPROTect;} return false;};
    /*!
     * Description: 
     *   set the protection value
     *
     * Arguments:
     *   true - turn on
     *
     * returns:
     *    true on success. 
     */
    inline bool SetProtect(bool q) 
	{if(fDifferential) return SendCommand(kPROTECT,q); return false;};


    /*!
     * Description: 
     *   query the channel vertical size.
     *   For the SENsitivity range and stepsize of other plug in units
     *   refere to the appropriate User Reference Supplement.
     *
     * Arguments:
     *   q = perform query
     *
     * returns:
     *    
     */
    inline double Sensitivity(bool q) 
	{if(q) Query(kSENSITIVITY); return fSENsitivity;};
    /*!
     * Description: 
     *   set the channel vertical size.
     *   For the SENsitivity range and stepsize of other plug in units
     *   refere to the appropriate User Reference Supplement.
     *
     * Arguments:
     *   double value {1.0e-3:10}
     *
     * returns:
     *    true on success
     */
    inline bool Sensitivity(double v) 
	{return SendCommand(kSENSITIVITY,v);};

    /*!
     * Description: 
     *   query th units assocated with the channel. 
     *
     * Arguments:
     *   q - query current value
     *
     * returns:
     *    
     */
    inline string &Units(bool q) {if(q) Query(kUNITS); return fUNIts;};

    /*!
     * Description: 
     *   When either PLSCoupling or MNSCoupling is set to VC, VCOffset
     *   sets an internal comparison voltage; VCOffset has no other effect
     *   This applies to differential amplifiers only. 
     *
     * Arguments:
     *   q - query or just return the value
     *
     * returns:
     *    
     */
    inline double VCOffset(bool q) 
	{if(fDifferential) {if(q) Query(kVCOFFSET); return fVCOffset;} return -DBL_MAX;};

    /*!
     * Description: 
     *   When either PLSCoupling or MNSCoupling is set to VC, VCOffset
     *   sets an internal comparison voltage; VCOffset has no other effect
     *   This applies to differential amplifiers only. 
     *
     * Arguments:
     *   value to set to: 
     *
     * returns:
     *    
     */
    inline bool VCOffset(double v) 
	{if(fDifferential) {return SendCommand(kVCOFFSET,v);} return false;};



    /* ======================================================== *
     *             GPIB Commands                                *
     * Helper functions to format the commands for this class   *
     * into a format consumable by the underlying GPIB class.   *
     * ======================================================== */

    /*!
     * Description: 
     *   Send a command that sets a double value. 
     *   the value is bounds checked and then passed along. 
     *
     * Arguments:
     *   c - command to send to unit
     *   value - the double value assocated with the command
     *
     * returns:
     *    true on success. 
     */
    bool SendCommand(COMMANDs c, double     value);
    /*!
     * Description: 
     *   Send a command associated with a double value
     *
     * Arguments:
     *   c - command to send to unit
     *   value - the double value assocated with the command
     *   
     *
     * returns:
     *    true on success. 
     *    
     */
    bool SendCommand(COMMANDs c, bool       value);
    /*!
     * Description: 
     *   Send a command associated with a boolean value
     *   
     *
     * Arguments:
     *   c - command to send to unit
     *   value - the double value assocated with the command
     *
     * returns:
     *    true on success. 
     *    
     */
    bool SendCommand(COMMANDs c, IMPEDANCE  value);
    /*!
     * Description: 
     *   Send a command to set the coupling value. 
     *
     * Arguments:
     *   command to send 
     *   COUPLING value, one of 
     *       {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC}
     *
     * returns:
     *   true on success
     *    
     */
    bool SendCommand(COMMANDs c, COUPLING   value);

    /// Query will return the data from a specific command. 
    /// bool false will be 0 and true will be 1
    /*!
     * Description: 
     *   Given a command query the mainframe to obtain a result. 
     *   the result is parsed and stuffed into the appropriate value. 
     *
     * Arguments:
     *   c - COMMAND to query. 
     *
     * returns:
     *    true on success.
     */
    bool Query(COMMANDs c);

    /*!
     * Description: 
     *   Update the entire channel data. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *   true on success    
     */
    bool Update(void);  // Query and update data.

    /*!
     * Description: 
     *   Decode the data returned from the query CH0?
     *   or a more generic call like CHL? - which asks about all channels
     *   in the left module. 
     *  Page 77
     * 
     * Arguments:
     *   return string from query to decode. 
     *
     * returns:
     *    
     */
    void Decode(const char *ChannelString);

    inline string Text(void) {return *fText;};

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
    friend ostream& operator<<(ostream& output, const Channel &n); 

    /* ======================================================== */

private:
    /*!
     * Description: 
     *   Decode COUPLING from string provided
     *
     * Arguments:
     *   p - String to parse the COUPLING value from. 
     *   will look something like CHL1 COU:DC
     *
     * returns:
     *    COUPLING enum associated with string
     */
    COUPLING  DecodeCoupling(const char *p);
    /*!
     * Description: 
     *    Decode IMPEDENC from string provided
     *
     * Arguments:
     *   p - String to parse the IMPEDENCE value from. 
     *   will look something like CHL1 IMP:50
     *
     * returns:
     *    IMPEDENCE enum associated with string
     *    
     */
    IMPEDANCE DecodeImpedance(const char *p);

    /*!
     * Description: 
     *   given a COUPLING enum value, return the associated string. 
     *
     * Arguments:
     *   c - COUPLING value
     *
     * returns:
     *    associated string. 
     */
    const char* CouplingString(COUPLING c) const;

    string*       fText;       // text when querying channel. 
    SLOT          fSlot;
    unsigned char fNumber;       // Channel Number
    bool          fDifferential; // Is this channel on a differential module?

    // Page 90
    double    fAMPoffset;      // AMPoffset
    double    fBW;             // BW
    double    fBWHi;           // BWHi
    double    fBWLo;           // BWLo
    COUPLING  fCOUpling;       // COUpling {AC,DC,OFF,VC}
    IMPEDANCE fIMPedance;      // IMPedance
    COUPLING  fMNSCoupling;    // MNSCoupling
    double    fMNSOffset;      // MNSOffset
    char*     fMNSProbe;
    double    fOFFSet; 
    COUPLING  fPLSCoupling;
    double    fPLSOffset;
    char*     fPLSProbe;
    char*     fPROBe;
    bool      fPROTect;        //
    double    fSENsitivity;    // 11A32 amplifiers 1e-3:10V, 11A71 10e-3:1V
    string    fUNIts;
    double    fVCOffset;       // internal comparison voltage
};
#endif
