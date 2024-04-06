/**
 ******************************************************************
 *
 * Module Name : RTGraph.hh
 *
 * Author/Date : C.B. Lirakis / 23-Mar-24
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
#ifndef __RTGRAPH_hh_
#define __RTGRAPH_hh_
#    include <stdint.h>
#    include <vector>

/// A place to store Realtime Graph components. 
class RTGraph {
public:

    /*!
     * Description: 
     *   constructor - name and title or NULL?
     *
     * Arguments:
     *   Name - name, for finding graph
     *   Title for the graph
     *   NPTS - number of points if pre-allocate, makes it wrap. 
     *
     * Returns:
     *
     * Errors:
     *
     */
    RTGraph(const char *Name=NULL, const char *Title=NULL,uint32_t NEntries=0);

    /// Default destructor
    ~RTGraph(void);


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
    void AddPoint(double x, double y); 



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
    inline double MeanX(void)  {return Mean(fX);};

    inline double SigmaX(void) {return Sigma(fX);};

    inline double MeanY(void)  {return Mean(fY);};

    inline double SigmaY(void) {return Sigma(fY);};

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
    bool WriteJSON(const char *Filename);

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
    inline void XLabel(const char *label) {fLabelX = label;};

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
    inline void YLabel(const char *label) {fLabelY = label;};



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

    friend ostream& operator<<(ostream& output, RTGraph &n); 


private:
    string          fName;
    string          fTitle;
    string          fLabelX;
    string          fLabelY;
    uint32_t        fNEntries;

    // Data arrays. 
    vector <double> fX;
    vector <double> fY;

    double Mean(vector<double> &X);

    double Sigma(vector<double> &X);


};
#endif
