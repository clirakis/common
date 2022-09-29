/**
 ******************************************************************
 *
 * Module Name : H5read.h
 *
 * Author/Date : C.B. Lirakis / 09-Apr-22
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
#ifndef __H5READ_hh_
#define __H5READ_hh_
#    include <time.h>
// My local rendition of navigation points
#    include "NavPointList.hh"
class H5Logger;
/*!
 * H5read - a class built on top of the my H5Logger class to import into
 * a local NavPointList. 
 */
class H5read : public NavPointList
{
public:
    /*!
     * Constructor - input a file that contains GPS file format. 
     */
    H5read(const char *filename);

    /*! 
     * Doesn't do much at this time. 
     */
    ~H5read(void);

    /*! 
     * Did an error occur during the last operation. 
     */
    inline Bool_t Error(void) const {return fError;};


private:
    /*!
     * Format the full file to internal format. 
     */
    Bool_t FormatFile(H5Logger *);

    TString* fTrackTitle;

    /*! 
     * Time is composed of Time since epoch and milliseconds. 
     * This is the milliseconds from that value. 
     */
    double fMilli;
    /*! 
     * Time of Last point read. also the delta time assoicated with that. 
     */
    double fLast, fdt;

    /*!
     * Set to true if an error occured in the last call. 
     */
    Bool_t fError; 
};
#endif
