/**
 ******************************************************************
 *
 * Module Name : MeasurementA.hh
 *
 * Author/Date : C.B. Lirakis / 26-Dec-22
 *
 * Description : MeasurementA, keep track of all possible measurement types. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 27-Dec-22  CBL absorb MValue functionality into this.
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 *  MS <meas>    page 203  Query only the measurement in question. 
 *  MSLIST       page 201  Set up to 6 possible measurements to be performed 
 *                         on the waveform. 
 *  MEAS?        page 192  Executes the commands.  This is complex 
 *  MSN          page 205  Number of measurements currently programmed into
 *                         the unit. 
 *
 * 
 *******************************************************************
 */
#ifndef __MeasurementA_hh_
#define __MeasurementA_hh_
#  include "DSA602_Types.hh"
/*!
 * Keep track of all of the possible measurements we can make
 * off a waveform. Class MeasurementA stores on element of this list
 * and allows the user to manipulate the data in the list.
 */
class MeasurementA 
{
public:
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    MeasurementA(const char *l);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    ~MeasurementA(void);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    const char *Text(void) const;
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    bool Match(const char *l);
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline void     SetState(bool s)     {fEnabled = s;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline bool     State(void) const          {return fEnabled;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline double Value(void)            {return fValue;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline void     SetValue(double value) {fValue = value;};

    /*!
     * responses are setup such that they are a value with a qualifier. 
     * The qualifier is found in DSA602_Types.h
     * MEASUREMENT_QUALIFIER {kEQUAL=0, kLESS_THAN, kGREATER_THAN,kUNKNOWN, kERROR}
     *
     */
    bool Decode(const char *Response);

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline unsigned Qualifier(void) const {return (unsigned) fQualifier;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    inline bool OK(void) const {return (fQualifier!=kMNONE);};

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * returns:
     *    NONE
     */
    friend ostream& operator<<(ostream& output, const MeasurementA &n); 

private:
    std::string    *fMeas;     // object name
    double         fValue;     // Value stored for this measurement. 
    uint8_t        fQualifier; // qualifier on measurement. 
    bool           fEnabled;   // Since this is a list of known values, is it enabled? 
};


#endif
