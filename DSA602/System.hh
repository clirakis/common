/**
 ******************************************************************
 *
 * Module Name : System.hh
 *
 * Author/Date : C.B. Lirakis / 24-Jan-21
 *
 * Description : Encapsulate all the "System" functions
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602 Programmers reference manual, page 37 is summary
 *              of all available commands. 
 *
 *******************************************************************
 */
#ifndef __SYSTEM_hh_
#define __SYSTEM_hh_
#  include "CObject.hh"

/// System documentation here.
 
class System : public CObject
{
public:
    /*!
     * Description: 
     *   System command set encapsulation. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    
     */
    System(void);

    /*!
     * Description: 
     *   Desctructor - doesn't do much right now. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    ~System(void);


    /* ======================================================== */
    /*                      FUNCTIONS                           */ 
    /* ======================================================== */

    /*!
     * Description: 
     *   Setup Calibrator Amplitude (CALIBRATOR) Page 72
     *   The Amplitude that is permitted depends on the current 
     *   Frequency setting. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    bool CalibratorAmplitude(double val);
    /*!
     * Description: 
     *   Query calibrator setup. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    current amplitude of output in Volts. 
     */
    double CalibratorAmplitude(void);

    /*!
     * Description: 
     *   Setup Calibrator Frequency (CALIBRATOR) Page 72
     *
     * Arguments:
     *   val - one of: CAL_FREQ_ZERO=0, CAL_FREQ_1KHZ, CAL_FREQ_1024MHZ
     *
     * returns:
     *    true on success.
     */
    bool CalibratorFrequency(CALIBRATOR_FREQ val);
    /*!
     * Description: 
     *   Query calibrator setup. 
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    int CalibratorFrequency(void);

    /*!
     * Description: 
     *   Query calibrator setup, impedence in ohms 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    impedence {50, 450} only two possible outputs
     */
    double CalibratorImpedence(void);

    /*!
     * Description: 
     *   Set the date to the current computer system value. 
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true on success. 
     */
    bool SetDate(void);
    /*!
     * Description: 
     *   Query current sytem DATE, page 113
     *
     * Arguments:
     *     NONE   
     *
     * returns:
     *    string containing the current Date. 
     */
    const char* Date(void);

    /*!
     * Description: 
     *   Define logical names for command strings (DEF)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool Define(int val) {return true;};
    /*!
     * Description: 
     *   Query
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool Define(void) const {return true;};

    /*!
     * Description: 
     *   Query which display menu is active. (DSYMENU?)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool DisplayMenu(void) const {return true;};

    /*!
     * Description: 
     *   Set the internal date format for stored waveforms. 
     *   milliseconds  or ?? (DSYSTOFMT)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool DateFormat(int val) {return true;};
    /*!
     * Description: 
     *   Query
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool DateFormat(void) const {return true;};

    /*!
     * Description: 
     *   INITIALIZE mainframe! (INIT)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool INITALIZE(int val) {return true;};

    /*!
     * Description: 
     *   Query the installed options (OPTIONS?)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool Options(void) const {return true;};

    /*!
     * Description: 
     *   Query the number of times the unit has been powered on. 
     *   (POWERON?)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    Number of times the unit has been powered on. 
     */
    unsigned PowerOn(void);

    /*!
     * Description: 
     *   Setup the sample clock to be dithered.(SCLOCKD)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool SampleClock(int val) {return true;};
    /*!
     * Description: 
     *   Query
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool SampleClock(void) const {return true;};

    /*!
     * Description: 
     *   Set the mainframe system clock to the computer clock value. (TIME)
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    true on success.
     */
    bool SetTime(void);
    /*!
     * Description: 
     *   Query the mainframe system time. page 290
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    string containing the system time. 
     */
    const char* Time(void);

    /*!
     * Description: 
     *   Undefines a previous string. (UNDEF)
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    inline bool Undefine(int val) {return true;};


    /*!
     * Description: 
     *   returns a value in seconds of the amount of time the system 
     *   has been turned on.  (UPTIME?) page 317
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    uptime in seconds. 
     */
    double  UpTime(void);


    /*!
     * Description: 
     *   Set a unique user id for the system. (USERID) page 317
     *
     * Arguments:
     *   character string to set - 
     *
     * returns:
     *    true on success - CURRENTLY DOES NOT WORK
     */
    bool UserID(const char* val);
    /*!
     * Description: 
     *   Query the User ID for the system
     *
     * Arguments:
     *   NONE
     *
     * returns:
     *    charcter up to 127 in length user specificed descriptor.
     */
    const char* UserID(void);


private:

};
#endif
