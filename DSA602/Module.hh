/**
 ******************************************************************
 *
 * Module Name : Module.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : Module, stores data on a per channel basis
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
 *  24-Jan-21   CBL Merged Module.hh and ModuleGPIB.hh
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 * 
 *******************************************************************
 */
#ifndef __MODULE_hh_
#define __MODULE_hh_
#  include <string>
#  include "CObject.hh"
#  include "DSA602_Types.hh"
#  include "Channel.hh"

class Module : public CObject
{
public:

    /*!
     * Description: 
     *   Given a string decode the module data and determine the
     *   number and configuration of the module channels. 
     *
     * Arguments:
     *   string to decode, if NULL, just does default stuff. 
     *
     * returns:
     *    
     */
    Module(const char *val=NULL);

    /*!
     * Description: 
     *   Desctructor - delete the allocated channel class
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    NONE
     */
    ~Module(void);

    /*!
     * Description: 
     *   stuff all the formatted data about Module and Channel 
     *   into the specified stream. 
     *
     * Arguments:
     *   output - stream to stuff
     *   n      - Module class input
     *
     * returns:
     *    
     */
    friend ostream& operator<<(ostream& output, const Module &n);

    /*!
     * Description: 
     *   Set the module type based on the provided string. 
     *
     * Arguments:
     *   String to parse.
     *
     * returns:
     *    
     */
    void Type(const string &s);
    /*!
     * Description: 
     *   return the module type 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    inline MODULE_TYPE Type(void) const {return fType;};

    /*!
     * Description: 
     *   return the string associated with the module serial number.
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    std::string associated with the module serial number. 
     */
    inline const string &Serial(void) const {return fUnitSerial;};
    /*!
     * Description: 
     *   Allow upstream functions to set the serial number
     *
     * Arguments:
     *   string to use for setting the serial number.
     *
     * returns:
     *    none
     */
    inline void Serial(string &s) {fUnitSerial=s;};

    /*!
     * Description: 
     *   Return the slot that this module is in.
     *   obtained via setup.  
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    SLOT that the module is in. 
     */
    inline SLOT     Slot(void)      const {return fSlot;};
    /*!
     * Description: 
     *   Allow upstream program to set the slot number.
     *   Really manipulated by StatusAndEvent class CONFIG
     *
     * Arguments:
     *     SLOT to use for this module
     *
     * returns:
     *    NONE
     */
    inline void     Slot(SLOT s) {fSlot=s;};

    /*!
     * Description: 
     *     return the number of channels associated with this module. 
     *
     * Arguments:
     *     NONE
     *
     * returns:
     *    number {0:2} of channels associated with the channel. 
     */
    inline unsigned char GetNChannel(void) const {return fNChannel;};

   
    /*!
     * Description: 
     *     return a pointer to the specified channel for upstream access. 
     *
     * Arguments:
     *     c - channel number in the module the user wants access to. {0:1}
     *     if out of bounds returns a NULL pointer. 
     *
     * returns:
     *    number {0:1} of channels associated with the channel. 
     */
    inline Channel*      GetChannel(uint8_t c) 
	{if (c<fNChannel) return fChannel[c]; else return NULL;};

    /*!
     * Description: 
     *     return the name of the module based on the type
     *
     * Arguments:
     *     NONE
     *
     * returns:
     *    character string that contains the module name. 
     */
    const char*     ModuleString(void);

    inline bool IsDifferential(void) const {return fDifferential;};

#ifdef DEBUG_MODULE
    /*!
     * Description: 
     *     Perform tests on the channel class - used for debugging
     *
     * Arguments:
     *     NONE
     *
     * returns:
     *    NONE
     */
    void Test(void);
#endif

private:
    
protected:
    void Decode(const char*);

    std::string fUnitSerial;    // Serial number of this module. 
    SLOT        fSlot;          // Slot position of this module.
    MODULE_TYPE fType;          // Module type, 11A32 - Single Ended
    Channel*    fChannel[2];    // Data on each channel in module. 
    size_t      fNChannel;      // Number of channels in module. 
    bool        fDifferential;  // Is the module differential?
};
#endif
