/**
 ******************************************************************
 *
 * Module Name : Keithley.hh
 *
 * Author/Date : C.B. Lirakis / 24-Nov-14
 *
 * Description : general header file for keithley instruments
 * Not currently implemented: 
 * SRQ - service request setup. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Nov-14  CBL   Tested with Keithley 230 series system
 *                  Seperated into Keithley and then super classes. 
 * Classification : Unclassified
 *
 * References : 220/230 manuals
 *
 *
 * 
 *******************************************************************
 */
#ifndef __KEITHLEY_hh_
#define __KEITHLEY_hh_
#    include <fstream>
#    include "GPIB.hh"
/**
 * EOI K0 - Send EOI
 *     K1 - Send no EOI
 */
/// Keithley documentation here. 
class Keithley : public GPIB
{
public:
    enum UnitType     {CurrentSource=220, VoltageSource=230, Multimeter=196,
    Multimeter2=197};

    /// Default Constructor
    Keithley(int gpib_address, bool verbose=false);
    /// Default destructor
    ~Keithley();


    void SetUnitType(UnitType c);
    inline int  GetUnitType(void) const {return fUnitType;};

    // Return read results. 
    inline char  ReadStatus(void)   const {return fReadStatus;};
    inline const char* Prefix(void) const {return fPrefix;};


protected:
    void DecodePrefix(const char *v);

    unsigned char fUnitType;  // 220, 230, 196 ...
    // Result of read from any device. 
    char          fReadStatus;
    char          fPrefix[8];
};
#endif
