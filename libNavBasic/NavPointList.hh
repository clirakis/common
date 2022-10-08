/**
 ******************************************************************
 *
 * Module Name : NavPointList.hh
 *
 * Author/Date : C.B. Lirakis / 20-Feb-17
 *
 * Description : rewrite using stl. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *
 *******************************************************************
 */
#ifndef __NAVPOINTLIST_hh_
#define __NAVPOINTLIST_hh_

#include <list>
#include <string>
#include "NavPoint.hh"

class FileName;

class NavPointList 
{
public:
    /*! class of generic points to be used in navigation. 
     * name - contains full filename specification. 
     */
    NavPointList(const char *name=NULL, 
		 int type=NavPoint::kWAYPOINT);

    /*! The destructor for the class. */
    ~NavPointList();

    bool Read (const char *name);
    /*! Write the data to the file specified. */
    bool Write(const char *name, bool Append);

    /*! Close out the file if open. */
    bool Close(void);

    /*! Append data to the current filename.  */
    bool AppendCurrent(void);

    /*! Add a data point to the list. X, Y, Z time between measurements
     * and some title or comment.
     * While Lon, Lat are called out, this doesn't really mean 
     * that the data couldn't be XY. 
     * Note, there is no conversion done here. It is upto the calling 
     * method to provide the appropriate conversion.
     */
    void  Add  (double Lon, double Lat, double Z=0.0, 
		double dt=0.0, const char* title=NULL);
    /*! Add from existing NavPoint 
     * Same comment applies, no internal conversion. 
     */
    void Add (const NavPoint &p);
    /*! Return the number of points currently in the list. */
    int  Size (void);

    /*! is this an XY file or Lat Lon file? */
    inline bool IsXY(void) const {return fIsXY;};

    /*! Set the value of XY(true) or LL (false) */
    inline void SetXY(bool v) {fIsXY=v;};

    /*! What type of list is this? see enum in NavPoint. */
    inline int Type(void) const {return fType;};

    /*! Access to list */
    inline std::list<NavPoint*> GetList(void) {return fNavPoints;};

    /*! Access the filename if necessary. */
    inline const std::string& Filename(void) const {return *fFilename;};

protected:
    /*! A list of TVector3, these are 3 dimensional and contain text. */
    std::list<NavPoint*> fNavPoints;
    /*! What is the type of point, available are, Waypoint, Event... */
    int          fType;
    /*! XY or Lat Lon storage? */
    bool         fIsXY;
    /*! Current filename assigned. */
    std::string* fFilename;
    /*! tell the program to save now. False - we are up to date. */
    bool         fSave;
    /*! Has the file ever been written to? */
    bool         fFirst; 
    /*! A class that deals with how to name the files. */
    FileName*    fFileName;
    /*! A Tstring to hold any file specific title. */
    std::string* fTitle;

    /*! Ordinal point count. */
    unsigned int fNavPointCount;

private:
    /*! A private funciton to write the header for the file. */
    bool        WriteHeader(std::ofstream &o);
};
#endif
