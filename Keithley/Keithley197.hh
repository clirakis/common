/**
 ******************************************************************
 *
 * Module Name : Keithley197.hh
 *
 * Author/Date : C.B. Lirakis / 26-Nov-14
 *
 * Description : general header file for keithley instruments
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Nov-14  CBL   Tested with Keithley 230 series system
 *
 * Classification : Unclassified
 *
 * References : 196 programming manuals
 *
 *
 * 
 *******************************************************************
 */
#ifndef __KEITHLEY197_h_
#define __KEITHLEY197_h_
#    include <fstream>
#    include "Keithley.hh"

/// Keithley documentation here. 
/**
 * Range Volts Ohms  Amps
 * R0 -  AUTO  AUTO  AUTO
 * R1 -  200m  200   200u
 * R2 -    2    2k     2m
 * R3 -   20   20k    20m
 * R4 -  200  200k   200m
 * R5 - 1000     M  2000m
 * R6 -   -    -      10A
 */

class Keithley197 : public Keithley
{
public:
    enum TriggerType  {ContinuousOnTalk=0, OneShotOnTalk, ContinuousOnGET, 
		       OneShotOnGET, ContinuousOnX, OneShotOnX};

    /// Default Constructor
    Keithley197(int gpib_address, bool verbose=false);
    /// Default destructor
    ~Keithley197();

    // Current and voltage source 
    inline bool ReadWithPrefix(bool o) {if (o) return Command("G1X", NULL, 0);
	else return Command("G0X", NULL, 0);};

    // Rel on supresses a stored baseline. 
    inline bool REL_ON(bool o) {if (o) return Command("Z1X", NULL, 0);
	else return Command("Z0X", NULL, 0);};

    bool GetStatus(void);
    void DumpStatus(void);

    inline unsigned char Functon(void)       const {return fFunction;};
    inline unsigned char Range(void)         const {return fRange;};
    inline unsigned char Relative(void)      const {return fRelative;};
    inline unsigned char EOI(void)           const {return fEOI;};
    inline unsigned char Trigger(void)       const {return fTrigger;};

    inline unsigned char DataBuffer(void)    const {return fDataBuffer;};
    inline unsigned char SQR_DataMask(void)  const {return fSRQDataMask;};
    inline unsigned char SRQ_ErrorMask(void) const {return fSRQErrorMask;};
    inline unsigned char Terminator(void)    const {return fTerminator;};

    double Get(void);
    bool   GetBuffer( double *data, size_t n);

private:
    // Status values
    char          fKStatus[32];
    unsigned char fFunction, fRange, fRelative, fEOI, fTrigger, fDataBuffer; 
    unsigned char fSRQDataMask, fSRQErrorMask;
    char          fTerminator;
};
#endif
