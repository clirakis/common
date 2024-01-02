/**
 ******************************************************************
 *
 * Module Name : SFilter.h
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Implementation of a super simple low pass filter
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * https://stackoverflow.com/questions/27106205/low-pass-filter-in-c
 *
 *******************************************************************
 */
#ifndef __SFILTER_hh_
#define __SFILTER_hh_
#  include <stdint.h>
#  include <math.h>

/// SFilter documentation here. 
class SFilter {
public:

    /*!
     * Description: 
     *   Simple Low Pass Filter  Constructor
     *
     * Arguments:
     *   CuttoffFrequency in Hz
     *   SampleRate in Hz
     *
     * Returns:
     *     NONE
     *
     * Errors:
     *     NONE
     */
    SFilter(double CutoffFrequency, double SampleRate=1.0 );
    /// Default destructor is not necessary at this time. 

    /*!
     * Description: 
     *   Filter an entire buffer of data. 
     *
     * Arguments:
     *    in  - input buffer to filter
     *    out - resulting filtered data
     *    N   - number of samples in both in and out buffers.     
     *
     * Returns:
     *    NONE
     * Errors:
     *    NONE
     */
    void Filter(const double *in, double *out, uint32_t N);

    /*!
     * Description: 
     *   Sample by Sample filtering operation
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    double Filter(const double &in);

    inline void Reset(void) {fPrev = 0.0;};
    void ChangeConstant(double CutoffFrequency);

    /// Access to values
    inline double Cutoff(void) const {return fCutoff;};
    inline double SampleRate(void) const {return fSampleRate;};

private:
    /*!
     * Sample rate of input 
     */
    double   fSampleRate;
    /*!
     * Cutoff frequency. 
     */
    double   fCutoff;
    /*! 
     * The calculated filter value
     */
    double   fBeta;
    /*! 
     * For running low pass averages, fPrev is the previous filtered value. 
     * for feed forward
     */
    double   fPrev;    

};
#endif
