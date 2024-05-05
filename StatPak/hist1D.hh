/**
 ******************************************************************
 *
 * Module Name : hist1D.h
 *
 * Author/Date : C.B. Lirakis / 3-Mar-24
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
#    include <vector>
#    include "Average.hh"

/*!
 * Hist1D 
 * A class for creating, filling and outputing 1 dimensional histograms. 
 * The user will specify a name, number of bins, and upper and lower edges
 * to generate the histogram. 
 */
class Hist1D : public Average
{
public:
    /*!
     * Description: 
     *   1-D histogram constructor. 
     *
     * Arguments:
     *   name  - Name of histogram for output
     *   NBins - number of desired bins, used to calculate bin size. 
     *   min   - Lower edge of histogram, used to calculate bin size. 
     *   max   - Upper edge of histogram, used to calculate bin size. 
     *
     *   Bin size calculated by \f${(Max-Min)\over{N}}\f$ and is unitless. 
     *
     * Returns:
     *     NONE
     *
     * Errors:
     *     NONE
     *
     */
    Hist1D(const char *name, uint32_t NBins, double min, double max);

    /*!
     * Description: 
     *   1-D histogram destructor, clean up after yourself. 
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *     NONE
     *
     * Errors:
     *     NONE
     *
     */
    /// Default destructor
    ~Hist1D(void);

    /*!
     * Description: 
     *   Fill the histogram with a single value with a weight. 
     *
     * Arguments:
     *   val - value to bin
     *   weight - give it a weight (default = 1.0)
     *
     * Returns:
     *     NONE
     *
     * Errors:
     *     NONE
     *
     */
    void Fill(double val, double weight = 1.0);

    /*!
     * Description: 
     *   Print to stdout the histogram for quick view purposes.
     *
     * Arguments:
     *   type <br> 
     *        0) Dump the contents of the vector <br>
     *        1) Vertical (down the page) <br>
     *        2) Horizontal \todo 
     *
     * Returns:
     *     NONE
     *
     * Errors:
     *     NONE
     *
     */
    void Print(uint32_t type=0);

    /*!
     * Description: 
     *   Write a JSON file with the description of the histogram 
     *   for plotting
     *
     * Arguments:
     *   Filename - filename to produce. 
     *
     * Returns:
     *     true on success
     *
     * Errors:
     *     NONE
     *
     */
    bool WriteJSON(const char *Filename);


    friend ostream& operator<<(ostream& output, Hist1D &n); 

private:
    //! Vector for the histogram data
    vector<double> fData;
    //! Name for printing the histogram. 
    char           *fName;
    //! Lower edge of the histogram
    double         fMin;
    //! Upper edge of the histogram
    double         fMax;
    //! Count on number of entries where (x<Min)
    uint32_t       fUnder;
    //! Count on number of entries where (x>Max)
    uint32_t       fOver;
    /*! 
     * Bin size calculated by \f$ {(Max-Min)\over{N}}\f$
     */
    double         fBinSize;    
};
#endif
