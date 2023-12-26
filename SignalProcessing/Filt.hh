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
 * ~/Projects/Scope/SignalProcessing
 * ~/Projects/Jupyter/Filter
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
    enum FilterType {kBANDPASS=0, kLOWPASS, kHIPASS, kALOWPASS, kNOTDEFINED};

    /// Default Constructor
    Filt(void);

    Filt(const Filt &p);

    /// Default destructor
    ~Filt(void);
    inline void Do(void) {fType=kBANDPASS;};

    /*!
     * Description: 
     *   operate on the data provided and returned the filtered data
     *
     * Arguments:
     *   x - the input data
     *
     * Returns:
     *   The filtered data
     *
     * Errors:
     *
     */
    vector<double> Filter(const vector<double> &x);

    /*!
     * Description: 
     *  If a csv fie withe the filter coefficients is available, 
     * use this to create the filter! This is typically use with
     * the python equivalent functions to create the filter 
     * and then export the coefficients into a CSV file.    
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
