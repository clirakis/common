/**
 ******************************************************************
 *
 * Module Name : Keithley2x0.hh
 *
 * Author/Date : C.B. Lirakis / 24-Nov-14
 *
 * Description : general header file for keithley 220/230 instruments
 * Not currently implemented: 
 * SRQ - service request setup. 
 *
 *
 * The 220/230 programmable sources have the ability to program upto
 * 100 pre-programmed points. The important features of these are:
 * buffer 
 * dwell time - time spent at each point in the buffer as it is stepped.
 * memory location - pointer to current memory for programmed behaviour. 
 *
 * A trigger will be issued for memory steps to synchonize other equipment. 
 *
 * 230 Programmable Voltage Source
 *     Voltage range from 199.95mV to 101V
 *     Current limits of 2mA, 20mA, 100mA
 *
 * 220 Programmable Current Source
 *     Current range from 1nA to 100mA
 *     Voltage limits of 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Nov-14  CBL   Tested with Keithley 230 series system
 *
 * Classification : Unclassified
 *
 * References : 220/230 manuals
 *
 *
 * 
 *******************************************************************
 */
#ifndef __KEITHLEY2X0_h_
#define __KEITHLEY2X0_h_
#    include <fstream>
#    include "Keithley.hh"
/*!
 * Waveform creation 
 * This class helps aid the preprogrammed nature of the 220/230 units. 
 */ 
class KWavform
{
public:
    /*!
     * Constructor, enter the first point falues or nothing. 
     * This is primarily used as a structure for programming and interrogating
     * the unit. 
     */
    KWavform(double v, double l, double d) {fValue = v; fLimit=l;fDwell=d;};

    /*! 
     * Value for point. 
     */
    inline double Value(void) const {return fValue;};
    /*!
     * Value for unit, 230 - Voltage, 220- Current. 
     */
    inline double Limit(void) const {return fLimit;};
    /*! 
     * Dwell time at point. 1:5ms
     */
    inline double Dwell(void) const {return fDwell;};

    /*!
     * Set the value of this point. 220 - Current, 230 - voltage.
     */
    inline void SetValue(double V) {fValue = V;};
    /*! 
     * Set the limit of the point. 
     * 220 - Voltage limit
     * 230 - Current limit 1:100mA
     */
    inline void SetLimit(double V) {fLimit = V;};
    /*!
     * Set dwell time, 1:5ms
     */
    inline void SetDwell(double V) {fDwell = V;};

    /*!
     * Set all three parameters. 
     */
    inline void Set(double v, double l, double d){fValue = v; fLimit=l;fDwell=d;};

private:
    double fValue;  /*! 220 - Current, 230 - Voltage */
    double fLimit;  /*! 220 - Voltage Limit, 230 - Current Limit */
    double fDwell;  /*! Dwell time at point. */
};

/// Keithley documentation here. 
/**
 * Range  220  230
 * R0 -  AUTO  AUTO
 * R1 -   1nA  100mV
 * R2 -  10nA    1V
 * R3 - 100nA   10V
 * R4 -   1uA  100V
 * R5 -  10uA
 * R6 - 100uA
 * R7 -   1mA
 * R8 -  10mA
 * R9 - 100mA
 */
class Keithley2x0 : public Keithley
{
public:

    /*!
     * ENUM for date formats. 
     */
    enum DataFormat   {LocationWithPrefix=0, LocationWithoutPrefix, 
		       BufferAddressWithPrefix, BufferAddressWithOutPrefix,
		       FullBufferWithPrefix, FullBufferWithOutPrefix};
    /*!
     * SRQ 
     */
    enum SRQ          { Disabled=0, IDDC_NoRemote, OverLimit, EndOfBuffer, 
			EndOfDwell, InputPortChange};
    /*!
     * Trigger type, see text for enum. 
     */
    enum TriggerType  {StartOnTalk=0, StopOnTalk, StartOnGET, StopOnGET,
		       StartOnX, StopOnX, StartOnExternal, StopOnExternal};

    /*!
     * Default Constructor for Keithley 220 Current source or 230
     * Voltage source. The user needs to specify at startup.
     * Inputs:
     *    gpib_address - the address of the unit to be commanded. 
     *    Type = Either V for voltage source or I for current source. 
     *    verbose = true/false for how much output you want in the log.
     */
    Keithley2x0(int gpib_address, char Type='V', bool verbose=false);
    /// Default destructor
    ~Keithley2x0();

    /*!
     * Current and voltage source 
     * Put in standby mode. 
     */ 
    inline bool Standby(void) { return Command("F0X", NULL, 0);};
    /*! 
     * Operate mode
     */
    inline bool Operate(void) { return Command("F1X", NULL, 0);};

    /*! 
     * Display the source on the LED
     */
    inline bool DisplaySource(void)  { return Command("D0X", NULL, 0);};
    /*! 
     * Display the limit on the LED
     */
    inline bool DisplayLimit(void)   { return Command("D1X", NULL, 0);};
    /*! 
     * Display the dwell time on the LED
     */
    inline bool DisplayDwell(void)   { return Command("D2X", NULL, 0);};
    /*! 
     * Display the memory pointer on the LED
     */
    inline bool DisplayMemory(void)  { return Command("D3X", NULL, 0);};
    /*!
     * Set program as a single value only. No dwell.
     */
    inline bool ProgramSingle(void)  { return Command("P0X", NULL, 0);};
    /*!
     * Set program to automatically step through the buffer. 
     */
    inline bool ProgramContinuous(void) { return Command("P1X", NULL, 0);};
    /*!
     * Single step with some trigger though the buffer. 
     */
    inline bool ProgramStep(void)    { return Command("P2X", NULL, 0);};

    /*!
     * Query the unit for it's current set value. This call will return
     * the value from the unit. 
     */
    double Get(void);  // Issue this command first, it will parse into below. 
    /*!
     * Return the principal value depdending on the unit type. 
     */
    inline double Value(void)const   {if (fUnitType==220) return fCurrent; 
	return fVoltage;};
    /*!
     * Return the limit value depdending on the unit type. 
     */
    inline double Limit(void) const  {if (fUnitType==220) return fVoltage; 
	return fCurrent;};
    /*!
     * Return the dwell time for the the memory value if using programmed
     * value. This assumes we are stepping through a pre-programed memory
     * sequence.  
     */
    inline double Dwell(void) const  {return fDwell;};
    /*! 
     * Get the current buffer pointer ??
     */
    inline unsigned char BufferAddress(void)  const {return fBuffer;};
    /*!
     * Get the current memory location for programming. 
     */
    inline unsigned char MemoryLocation(void) const {return fDisplay;};

    /*!
     * Set all values in one call. 
     * 
     */
    bool Set(double value, double Limit, double Dwell, int Memory, int Buffer);

    inline bool Set(int b, KWavform &w) {return Set(w.Value(), w.Limit(), w.Dwell(), b, b);}

    /*!
     * What this does depends on the unit. 
     * For the 230 This sets the voltage, 1:100V. 
     * For the 220 Programmable current source, this sets the voltage limit.
     */
    bool SetVoltage( double Value);
    /*!
     * What this does depends on the unit. 
     * For the 230 Programmable voltage source this sets the curent limit.
     * For the 220 Programmable current source, this sets the primary output.
     */
    bool SetCurrent( double Value);
    /*!
     * This will set the dwell time between an operation and trigger. 
     * 1:5 MS
     */
    bool SetDwell( double Value);
    /*!
     * Set the buffer value, values depend on unit. 
     */
    bool SetBuffer (unsigned char Value);
    bool SetMemory (unsigned char Pointer);
    bool SetOutput (unsigned char Value);

    // 
    bool SetTrigger(TriggerType t);
    
    // 
    void DumpBuffer(void);

private:
    bool DefaultSetup(void);

    // General storage variables, what these mean depends on the device
    double        fVoltage;   // 230 - Value, 220 - Voltage limit (V)
    double        fCurrent;   // 230 - Current Limit, 220 - Value (I) 
    double        fDwell;     // dwell times (W) in ms
    unsigned char fBuffer;    // Buffer address (B) 100 locations
    unsigned char fDisplay;   // Display location pointer (L)  100 locations
};
#endif
