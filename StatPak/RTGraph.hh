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
     *   NPTS - number of points if pre-allocate
     *
     * Returns:
     *
     * Errors:
     *
     */
    RTGraph(const char *Name=NULL, const char *Title=NULL);


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
    inline void AddPoint(double x, double y) 
	{fX.push_back(x); fY.push_back(y);};



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
    double MeanX(void);

    double Sigma(void);

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
    inline void SetLabelX(const char *label) {fLabelX = label;};

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
    inline void SetLabelY(const char *label) {fLabelY = label;};


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

    // Data arrays. 
    vector <double> fX;
    vector <double> fY;
};
#endif
