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

/*!
 * Class container for real time graph support. 
 */
class RTGraph {
public:

    /*!
     * Description: 
     *   constructor - name and title or NULL?
     *
     * Arguments:
     *   Name - name, for finding graph <br>
     *   Title for the graph <br>
     *   NPTS - number of points if pre-allocate, makes it wrap. <br>
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    RTGraph(const char *Name=NULL, const char *Title=NULL,uint32_t NEntries=0);

    /*!
     * Description: 
     *   RTGraph destructor
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    ~RTGraph(void);


    /*!
     * Description: 
     *   Add an (x,y) point pair to the graph to be plotted. This will 
     *   automatically increase the storage for the number of points
     *   stored until NEntries upper limit is reached. Once NEntries is reached
     *   the stored array will scroll left by an internally set number of 
     *   values, until that count is reached again. 
     *
     * Arguments:
     *   x - X axis value <br> 
     *   y - Y axis value <br> 
     * Returns:
     *     NONE
     * Errors:
     *     NONE
     */
    void AddPoint(double x, double y); 



    /*!
     * Description: 
     *   Since this inherits from the class average, this will return
     *   the mean of X
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    inline double MeanX(void)  {return Mean(fX);};

    /*!
     * Description: 
     *   Since this inherits from the class average, this will return
     *   the sigma of X
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    inline double SigmaX(void) {return Sigma(fX);};

    /*!
     * Description: 
     *   Since this inherits from the class average, this will return
     *   the mean of Y
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    inline double MeanY(void)  {return Mean(fY);};

    /*!
     * Description: 
     *   Since this inherits from the class average, this will return
     *   the Sigma of Y
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   NONE
     * Errors:
     *   NONE
     */
    inline double SigmaY(void) {return Sigma(fY);};

    /*!
     * Description: 
     *   On demand, write the JSON output of the data. 
     *
     * Arguments:
     *   Filename to write to. 
     *
     * Returns:
     *    NONE
     * Errors:
     *    NONE
     */
    bool WriteJSON(const char *Filename);

    /*!
     * Description: 
     *   Set the label for the X-Axis. 
     *
     * Arguments:
     *   label - character label to set the internal string to. 
     *
     * Returns:
     *   NONE
     *
     * Errors:
     *   NONE
     */
    inline void XLabel(const char *label) {fLabelX = label;};


    /*!
     * Description: 
     *   Set the label for the Y-Axis. 
     *
     * Arguments:
     *   label - character label to set the internal string to. 
     *
     * Returns:
     *   NONE
     *
     * Errors:
     *   NONE
     */
    inline void YLabel(const char *label) {fLabelY = label;};


    /*!
     * Description: 
     * ostream overload for providing formatted data about the class   
     *
     * Arguments:
     *    output - stream to attach to <br>
     *    n - implict RTGraph <br>
     *
     * Returns:
     *     Formatted ostream
     *
     * Errors:
     *     NONE
     *
     */

    friend ostream& operator<<(ostream& output, RTGraph &n); 


private:
    //! Name of object
    string          fName;
    //! Title for plotting 
    string          fTitle;
    //! Label for X axis
    string          fLabelX;
    //! Label for Y axis
    string          fLabelY;
    //! Maxiumum number of entries, if zero unlimited. 
    uint32_t        fNEntries;

    //! Vector to store X value of pair
    vector <double> fX;

    //! Vector to store Y value of pair
    vector <double> fY;

    double Mean(vector<double> &X);

    double Sigma(vector<double> &X);


};
#endif
