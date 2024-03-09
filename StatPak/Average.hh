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

class Average
{
public:
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
    Average(void);
    
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
    void   Add(double val);
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
    double Sigma(double *avg = NULL);
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
    double Get(void);
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
    void   Reset(void);

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
    friend ostream& operator<<(ostream& output, Average &n); 

protected:
    uint32_t fN;        // Total entries
    double   fSum;      // Sum of entries
    double   fSum2;     // Sum^2 of entries

private:

};

#endif
