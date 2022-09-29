/**
 ******************************************************************
 *
 * Module Name : GPXread.h
 *
 * Author/Date : C.B. Lirakis / 11-Jan-01
 *
 * Description : 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *   26-Mar-22 CBL Small updates
 *
 * Classification : Unclassified
 *
 * References :
 * https://root.cern/doc/master/classTXMLFile.html
 *
 *******************************************************************
 */
#ifndef __GPXREAD_hh_
#define __GPXREAD_hh_
#    include <time.h>
#    include <TXMLEngine.h>
// My local rendition of navigation points
#    include "NavPointList.hh"

/*!
 * GPXread - a class built on top of the cern root XML  class and
 * my local NavPointList. 
 *
 * The objective is to read the GPX file provided and produce a root list
 * that contains a cern root TList, but extended to use some of the fields 
 * of interest. NavPointList has read/write functions in a CSV format 
 * that is common. This class will support TRK - track and WPT - waypoint
 * Nominally this will save the files if requested. 
 */
class GPXread : public NavPointList
{
public:
    /*!
     * Constructor - input a file that contains the GPX file format. 
     * This is an XML file format that contains waypoint and track (this is 
     * all I support right now) and turns it into a list for 
     * display and analysis. 
     */
    GPXread(const char *filename);

    /*! 
     * Doesn't do much at this time. 
     */
    ~GPXread(void);

    /*! 
     * Did an error occur during the last operation. 
     */
    inline Bool_t Error(void) const {return fError;};

    /*!
     * Read a new file. deletes the old list. 
     */
    Bool_t xmlreadfile(const char* filename = "example.xml");

private:
    /*!
     * Pointer to the cern root XML parsing tool. 
     */
    TXMLEngine *fXML;

    /*!
     * Debugging tool to dump the entire node structure. This is re-entrant
     * and recursive. 
     */
    void   DisplayNode(XMLNodePointer_t node, Int_t level);
    /*! Once a track node is found, traverse the node for children, 
     * usually track points, and parse them. 
     */
    void   ParseTrack(XMLNodePointer_t node);
    /*!
     * Within a track, there can be track segments. The Track function calls
     * this 
     */
    void   ParseTrackSegment(XMLNodePointer_t node);
    /*!
     * and at the bottom of the segments, there are track points. 
     * The points, waypoint, track etc are formatted all the same
     * and can use this same function. 
     */
    void   ParsePoint(XMLNodePointer_t node, UInt_t type);
    /*!
     * This parses the time associated with the point, event etc. 
     */
    time_t ParseTime(const char *n);

    /*!
     * each track contains a unique name. This is the TString that 
     * contains that name. 
     */
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
