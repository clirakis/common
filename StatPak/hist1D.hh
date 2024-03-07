/**
 ******************************************************************
 *
 * Module Name : hist1D.h
 *
 * Author/Date : C.B. Lirakis / 23-May-21
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
#ifndef __HIST1D_hh_
#    define __HIST1D_hh_
#    include <stdint.h>
#    include "Average.hh"

/// Hist1D documentation here. 
class Hist1D : public Average
{
public:
    /// Default Constructor
    Hist1D(const char *name, uint32_t NBins, double min, double max);

    /// Default destructor
    ~Hist1D(void);

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
    void Fill(double val, double weight = 1.0);

    void Print(uint32_t type=0);

    friend ostream& operator<<(ostream& output, Hist1D &n); 

private:

    char     *fName;
    double   fMin, fMax;
    uint32_t fUnder, fOver;
    double   fBinSize;

};
#endif
