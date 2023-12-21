/**
 ******************************************************************
 *
 * Module Name : Butterworth.hh
 *
 * Author/Date : C.B. Lirakis / 15-Jan-23
 *
 * Description : Implementation of butterworth low pass filter
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions :
 * 16-Jan-23   CBL    Added in a bunch of code to do LP, HP etc. This is from
 *                    a different code base. 
 *                    - Adding in a read from CSV that is exported from 
 *                      python using Pandas. 
 *
 * Classification : Unclassified
 *
 * References :
 * https://github.com/nxsEdson/Butterworth-Filter/blob/master/butterworth.cpp
 * and a bunch more
 *
 *******************************************************************
 */
#ifndef __BUTTERWORTH_hh_
#define __BUTTERWORTH_hh_
#  include "Filt.hh"

/// Create a butterworth filter
class Butterworth : public Filt
{
public:

    /*!
     * Description: 
     *  Butterworth constructor.   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    Butterworth(uint16_t FilterOrder, double Lcutoff, double Ucutoff, 
		FilterType Type=kBANDPASS);

#if 0
    /*!
     * Description: 
     *  Butterworth constructor, import csv file that is made using
     *  the python tools.   
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    Butterworth(const char *Filename);
#endif

    /// Default destructor
    /*!
     * Description: 
     *  
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    ~Butterworth();

    /*!
     * Description: 
     *  Compute lowpass component values for an analog filter. 
     *  Based on code from:
     *  https://exstrom.com/journal/sigproc/alpbw.c
     *  Schematics and write up here: 
     *  https://exstrom.com/journal/sigproc/asigproc.html
     *
     * Arguments:
     *   ntaps       - filter order
     *   Frequency   - 3dB point in Hz
     *   Termination - true equal source and load resistances
     *
     * returns:
     *  vector of component values.  
     */
    vector<double> ALowPass(uint16_t ntaps, double Frequency, bool Termination);

    /*!
     * Description: 
     *   Digital equivalent of above. 
     *   https://exstrom.com/journal/sigproc/dsigproc.html
     *
     * Arguments:
     *   
     *
     * returns:
     *    
     */
    vector<double> LowPass(double HalfPowerF, double SampleRate);

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
    friend ostream& operator<<(ostream& output, const Butterworth &n); 

private:
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
    vector<double> TrinomialMultiply(vector<double> b, vector<double> c);

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
    vector<double> ComputeHP(void);
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
    vector<double> ComputeLP(void);

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

    void ComputeNumCoeffs(void);

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
    void ComputeDenCoeffs(void);

    uint16_t       fFilterOrder;
    double         fLowerCutoff;
    double         fUpperCutoff;
};
#endif
