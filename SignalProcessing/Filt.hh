/**
 ******************************************************************
 *
 * Module Name : Filt.hh
 *
 * Author/Date : C.B. Lirakis / 22-Jan-23
 *
 * Description :  Handle basic filter coefficients. 
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
#ifndef __FILT_hh_
#define __FILT_hh_
#  include <vector>
#  include <complex>
#  include <stdint.h>

/// Filt documentation here. 
class Filt 
{
public:

    // What type of filter is it? 
    enum FilterType {kBANDPASS=0, kLOWPASS, kHIPASS, kALOWPASS};

    /// Default Constructor
    Filt(void);

    Filt(const Filt &p);

    /// Default destructor
    ~Filt(void);

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */

    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    bool ReadCSVFile(const char *Filename);

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
    friend ostream& operator<<(ostream& output, const Filt &n); 

protected:
    vector<double> fDenomCoeffs;   // a
    vector<double> fNumCoeffs;     // b

    FilterType     fType; 

private:

    /*!
     * Description: 
     *   ClearAll -- Clear the vectors for a recalculation
     *
     * Arguments:
     *   NONE
     *
     * Returns: NONE
     *
     * Errors: NONE
     *
     */
    void ClearAll(void); 

};
#endif
