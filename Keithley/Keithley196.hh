/**
 ******************************************************************
 *
 * Module Name : Keithley196.hh
 *
 * Author/Date : C.B. Lirakis / 26-Nov-14
 *
 * Description : general header file for keithley instruments
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Nov-14  CBL   Not implemented: SRQ, and Translate
 *
 * Classification : Unclassified
 *
 * References : 196 programming manuals
 *
 *
 * 
 *******************************************************************
 */
#ifndef __KEITHLEY196_hh_
#define __KEITHLEY196_hh_
#    include <fstream>
#    include "Keithley.hh"
// Decode status word
struct t_MachineStatus
{
    bool          AUTOCAL_Mult;
    bool          ReadAD;
    unsigned char Function;
    unsigned char DataFormat;
    bool          SelfTest;
    unsigned char EOI_Holdoff;
    unsigned char SRQ;
    bool          Exp_Filter;
    unsigned char Filter_Average;
    unsigned int  DataStoreRate;
    unsigned char Range;
    unsigned char DisplayRate;
    unsigned char Trigger;
    unsigned int  Delay;
    char          Terminator;
    unsigned char Zero;
    bool          Calibration;
};
/// Keithley documentation here. 
/**
 * Range DCV   ACV   DCA   ACA   Ohms   ACV dB   ACA dB   Offset ohms
 *       ---   ---   ---   ---   ----   ------   ------   -----------
 * R0 -  AUTO  AUTO  AUTO  AUTO  AUTO   AUTO     AUTO     AUTO
 * R1 -  300mV 300mV 300uA 300uA  300   AUTO     AUTO      300
 * R2 -    3V    3V    3mA   3mA   3k   AUTO     AUTO        3K
 * R3 -   30V   30V   30mA  30mA  30k   AUTO     AUTO       30K
 * R4 -  300V  300V  300mA 300mA 300k   AUTO     AUTO       30K
 * R5 -  300V  300V    3A    3A    3M   AUTO     AUTO       30K
 * R6 -  300V  300V    3A    3A   30M   AUTO     AUTO       30K
 * R7 -  300V  300V    3A    3A  300M   AUTO     AUTO       30K
 *
 * Factory default conditions
 * --------------------------------
 * Multiplex                    A1 - Enabled
 * Reading                      B0 - A/D converter
 * Function                     F0 - DC volts
 * Data Format                  G0 - Send prefix
 * Self-Test                    J0 - Clear
 * EOI                          K0 - Enable EOI and bus hold-off on X
 * SRQ                          M0 - Disabled
 * Internal Digital Filter      N1 - Enabled
 * Filter                       P0 - Disabled
 * Data Store Interval          Q0 - One shot into buffer
 * Data Store Size              I1 - One reading
 * Rate                         S3 - 6-1/2d, line cycle integraton
 * Trigger                      T6 - Continuous on external trigger
 * Delay                        W0 - no trigger
 * Terminator                   Y0 - CR LF
 * Zero                         Z0 - Disabled
 */

/* 
 * character positons for system status message. 
 * First 3 characters should be 196 for a valid read. 
 */
#define AUTOCAL_MULIPLEX   0 // Enabled 1, disabled 0
#define READ_MODE          1 // 0 - A/D converter, 1 - data buffer
#define FUNCTION_MODE      2
#define DATA_FORMAT        3
#define SELF_TEST          4
#define BUS_HOLDOFF        5
#define SRQ_SETUP          6 // 2 decimal numbers
#define FILTER_EPONENTIAL  8 // exponential filter off - 0
#define FILTER_SAMPLES     9 // 2 decimal places - 0 is off
#define STORE_RATE        11 // 6 digits
#define RANGE_VALUE       17
#define DISPLAY_RATE      18
#define TRIGGER_TYPE      19
#define DELAY_VALUE       20  // for 5 digits
#define TERMINATOR        25
#define ZERO_ENABLED      26
#define CAL_SW            27

// Error bit locations
#define ERROR_TRIG          4
#define ERROR_SHORT         5
#define ERROR_BIG_STRING    6
#define ERROR_UNCAL         7
#define ERROR_ALWAYS_ZERO1  8    // for 3 digits
#define ERROR_CAL_LOCKED   11
#define ERROR_CONFLICT     12
#define ERROR_TRANSERR9    13
#define ERROR_NO_REMOTE    14
#define ERROR_IDDC         15
#define ERROR_IDDCO        16
#define ERROR_ALWAYS_ZERO2 17
#define ERROR_TRANSERR14   18
#define ERROR_TRANSERR15   19
#define ERROR_TRANSERR16   20
#define ERROR_TRANSERR17   21
#define ERROR_TRANSERR18   22
#define ERROR_TRANSERR19   23
#define ERROR_TRANSERR20   24
#define ERROR_TRANSERR21   25
#define ERROR_ALWAYS_ZERO3 26
#define ERROR_TRANSERR23   27

         

class Keithley196 : public Keithley
{
public:
    enum Function {DCV=0,ACV,OHMS,DCA,ACA,ACVdB, ACAdB, COMP_OHMS};
    enum TriggerType  {ContinuousOnTalk=0, OneShotOnTalk, ContinuousOnGET,
		       OneShotOnGET, ContinuousOnX, OneShotOnX, 
		       ContinuousOnExternalTrigger, OneShotOnExternalTrigger};
    enum DataFormat   {ReadingWithPrefix=0, ReadingWithoutPrefix, 
		       BufferAddressWithPrefix, BufferAddressWithOutPrefix,
		       BufferWithPrefixWithoutLocation, 
		       BufferWithOutPrefixWithoutLocation};
    enum SRQ {Disable=0,ReadingOverflow=1, DataStoreFull=2,DataStoreHalfFull=4,
	      ReadingDone=8,Ready=16,Error=32};
    // Not effective for all settings, see manual. 
    enum Rate {Digit3_5=0,Digit4_5,Digit5_5,Digit6_5};
    enum StatusCommand {MachineStatus=0, ErrorConditions, TranslatorWords,
			AverageReading, LowestReading, HighestReading,
			PresentValue, InputSwitchStatus};

    /// Default Constructor
    Keithley196(int gpib_address, bool verbose=false);
    /// Default destructor
    ~Keithley196();

    // inline functions
    inline bool SetDefault(void) {return Command("L0X", NULL, 0);};
    inline bool SetAutoCalMult(bool b) {if (b) return Command("A1X", NULL, 0);
	else return Command("A0X", NULL, 0);};
    inline bool Read_AD(bool b) {if (b) return Command("B0X", NULL, 0);
	else return Command("B1X", NULL, 0);};
    inline bool ExpFilter(bool b) {if (b) return Command("N1X", NULL, 0);
	else return Command("N0X", NULL, 0);};
    inline bool   Zero(bool e) {if(e) return Command("Z1",NULL,0); else
			     return Command("Z0",NULL,0);};
    // 
    double GetData(void);
    bool   SetFunction(Function v);
    bool   ZeroSet(double value);
    bool   SetFilter(int value);  // 0 - off(default),1-99 measurements average
    bool   SetZero(double value);
    bool   SetRate(Rate r);       // This is display averaging rate. 
    bool   SetTrigger(TriggerType t);
    bool   SetDataStore(unsigned int interval, unsigned int size);
    bool   SetFormat(DataFormat f);
    bool   SetSRQ(SRQ f);
    bool   SetDelay(unsigned int val); // milliseconds. 
    bool   DisplayCharacter (char *c);

    // interval is ms between samples
    bool   GetBufferOfData(double *b, size_t s, unsigned int interval);

    // Status functions
    const char* GetStatus(StatusCommand c);

    // Get and display machine status to stdout.
    struct t_MachineStatus& GetMachineStatus(void);
    void DumpMachineStatus();

private:
    char       fStatus[64];  // Store any one of the many status words here. 
    struct t_MachineStatus fMStatus;
};
#endif
