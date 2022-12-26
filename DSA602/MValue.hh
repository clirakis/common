/**
 ******************************************************************
 *
 * Module Name : MValue.h
 *
 * Author/Date : C.B. Lirakis / 11-Jan-01
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __MVALUE_hh_
#define __MVALUE_hh_
#  include "DSA602_Types.hh"

/*!
 * responses are setup such that they are a value with a qualifier. 
 * The qualifier is found in DSA602_Types.h
 * MEASUREMENT_QUALIFIER {kEQUAL=0, kLESS_THAN, kGREATER_THAN,kUNKNOWN, kERROR}
 *
 */
class MValue
{
public:
    MValue(void);
    MValue(const char *Response);

    bool Decode(const char *Response);

    inline double Value(void) const {return fValue;};
    inline unsigned Qualifier(void) const {return (unsigned) fQualifier;};
    inline bool OK(void) const {return (fQualifier!=kMNONE);};
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
    friend ostream& operator<<(ostream& output, const MValue &n); 

private:
    double fValue;
    unsigned char fQualifier;
};
#endif
