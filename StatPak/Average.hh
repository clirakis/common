/**
 ******************************************************************
 *
 * Module Name : Average.hh
 *
 * Author/Date : C.B. Lirakis / 28-June-08
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 03-Mar-24 Change to std:vector
 * 09-Mar-24 Don't really want to provide a vector here, pass in 
 *           from above. Act as a general format. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef  __AVERAGE_hh_
# define __AVERAGE_hh_

/*!
 * Class to encapsulate data associated with a one dimensional average. 
 */
class Average
{
public:
    /*!
     * Description: 
     *   Constructor a class for performing averages. 
     *   currently just resets all internal variables. (See Reset)
     *   operates on single variables only. 
     *
     * Arguments:
     *     NONE   
     *
     * Returns:
     *     NONE
     * Errors:
     *     NONE
     *
     */
    Average(void);
    
    /*!
     * Description: 
     *   Add a single point to the equation. Does the following: <br>
     *   \f$ N = N + 1 \f$ increment number of variables <br>
     *   \f$ Sum = Sum + val \f$ add to overall sum. <br> 
     *   \f$ {Sum^2} = {Sum^2} + {val^2} \f$ <br>
     *
     * Arguments:
     *    val - value ot be added to the average   
     *
     * Returns:
     *    NONE
     *
     * Errors:
     *    NONE
     *
     */
    void   Add(double val);


    /*!
     * Description: 
     *   Calculate and return the sigma of the array. <br> 
     *   \f$ \bar{x} = Sum/N \f$ calculate average. <br> 
     *
     * Arguments:
     *   avg - if not NULL, put \f$\bar{x}\f$ into this user supplied variable
     *
     * Returns:
     *   sigma of distribution. <br> 
     *   \f$ \sigma = \sqrt{{Sum^2} - {\bar x}^2}\f$
     * 
     * Errors:
     *   NONE
     *
     */
    double Sigma(double *avg = NULL);

    /*!
     * Description: 
     *   Calculate and return \f$ \bar{x} = Sum/N \f$ average. <br> 
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     *
     * Errors:
     *   NONE
     *
     */
    double Get(void);

    /*!
     * Description: 
     *   Perform reset on all variables. 
     *      Number of entries, Sum and \f$ {Sum^2} \f$
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     *
     * Errors:
     *   NONE
     *
     */
    void   Reset(void);

    /*!
     * Description: 
     *   provide method to output the contents of the class
     *
     * Arguments:
     *   output - ostream to populate <br> 
     *   n - Average class to dump <br>
     *
     * Returns:
     *   populated ostream. 
     * 
     * Errors:
     *   NONE
     *
     */
    friend ostream& operator<<(ostream& output, Average &n); 

protected:
    //! Total number of entries.
    uint32_t fN;
    //! Running sum of entry values. 
    double   fSum;
    //! Running sum squared of entry values
    double   fSum2; 

private:

};

#endif
