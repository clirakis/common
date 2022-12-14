/**
 ******************************************************************
 *
 * Module Name : NavPoint.hh
 *
 * Author/Date : C.B. Lirakis / 26-Nov-15
 *
 * Description : Generic navigation point. Major overhaul, 
 * now inherits from TVector3, this inherits from TObject. 
 * we will store fNumber in UniqueID of TObject
 * and we will override GetTitle for the charater data. The status
 * bits will be held and manipulated in SetBit, ResetBit, TestBit, TestBits
 * InvertBit...  
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 13-Jan-18    CBL        Really need to deal with X/Y vs decimal degree
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __NAVPOINT_hh_
#define __NAVPOINT_hh_
#  include "Point.hh"
#  include "CObject.hh"
#  include <string>
/* 
 * Define the string seperator for all the log files. 
 * Using , enables us to import the data to excel. CSV
 */
#  define NAV_SEPARATOR ","


class NavPoint : public Point, public CObject
{
public:
    /*! are the points saved in radians or not? */
    enum PointFormat  {kFORMAT_NONE, kFORMAT_RADIANS, kFORMAT_DEGREES};
    /*! What type of point is this? */
    enum NavPointType {kPOINT_NONE=0, kWAYPOINT, kEVENT};
    /**
     * the navigation point is the base class for all points created 
     * on the screen. X typically will be Longitude, and Y will typically
     * be Latitude. n is some name. 
     */
    NavPoint(double X, double Y, double Z=0.0, 
	     int type=NavPoint::kWAYPOINT,
	     double dt=0.0, const char *n=NULL);

    /*! Constructor from a Point */
    NavPoint(Point& pt, int type=NavPoint::kWAYPOINT,
	     double dt=0.0, const char *n=NULL) {
	NavPoint(pt.X(), pt.Y(), pt.Z(), type, dt, n);
    };
    /*! Constructor from another NavPoint. */
    NavPoint(const NavPoint&);

    /*! Constructor with a line read from a file. */
    NavPoint(const char *s);
    ~NavPoint();

    void SetName(const char*);

    const char* GetName(void) const {if (fName) return fName->c_str(); else return NULL;};
    bool SameName(const char *);
    inline void   SetDt(double v) {fdt=v;};
    inline double GetDt(void) const {return fdt;};
    inline int    Type(void)  const {return fType;};

    /*! 
     * Range
     * Determine the distance between two NavPoints 
     * if a variable address for Phi is provided, the 
     * true bearing between the two points is also given. 
     */
    double Range(const NavPoint&, double *Bearing=NULL);

    /*! Compute line parameters */
    double ComputeLine(const NavPoint&, const NavPoint&);

    /* How is the internal data stored? */
    inline void SetFormat(PointFormat s) {fFormat = s;};
    inline PointFormat Format(void) const {return fFormat;};
    void SetComment(const char *s);
    const char* CommentData(void) const;

    friend std::ostream& operator<<(std::ostream& output, const NavPoint &n);

private:
    static unsigned int fCount;
    /*! Type can be
     * NONE
     * Waypoint
     * Event
     */ 
    int         fType; 

    /**
     * Set the character data for the point. 
     */
    void SetData(const char*);
    /// Some name or other character string associated with the point
    std::string* fName;
    /*! Used for events, store any data associated with event. */
    std::string* fComment;
    /// Integration time, where applicable. 
    double       fdt;
    /*! Set the format of the point, */
    PointFormat  fFormat;
};
#endif
