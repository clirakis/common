/**
 ******************************************************************
 *
 * Module Name : NavPointList.hh
 *
 * Author/Date : C.B. Lirakis / 8-Sep-14
 *
 * Description : A collection of NavPoints using TList. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * https://root.cern.ch/doc/master/classTList.html
 *
 *******************************************************************
 */
#ifndef __NAVPOINTLIST_hh_
#define __NAVPOINTLIST_hh_
#include "NavPoint.hh"

class TList;
class TString;
class FileName;

class NavPointList 
{
public:
    enum NavPLErrors {kNPL_NONE=0, kNPL_FILENAME_ERROR, 
		      kNPL_READ_ERROR, kNPL_EMPTY};
    /*! class of generic points to be used in navigation. */
    NavPointList(const char *name=NULL, Int_t type=NavPoint::kWAYPOINT);
    /*! The destructor for the class. */
    ~NavPointList(void);
    Bool_t Load (const char *name);

    /*! Write the data to the file specified. */
    Bool_t Write(const char *name, bool Append);

    /*! Close out the file if open. */
    Bool_t Close(void);

    /*! Append data to the current filename.  */
    Bool_t AppendCurrent(void);

    /*! Add a data point to the list. X, Y, Z time between measurements
     * and some title or comment */
    void   Add  (Double_t Lon, Double_t Lat, Double_t Z=0.0, 
		 Double_t dt=0.0, const char* title=NULL);
    /*! Return the number of points currently in the list. */
    UInt_t  Size (void);

    /*! Access a particular point. */
    NavPoint* Point(UInt_t );

    /*! is this an XY file or Lat Lon file? */
    inline Bool_t IsXY(void) const {return fIsXY;};

    /*! Access to list */
    inline TList* GetList(void) {return fNavPoints;};

    /*! Check for error */
    inline Bool_t Error(void) const {return (fError!=kNPL_NONE);};

    /*! Get error code. */
    inline UInt_t GetError(void) const {return fError;};

    /*!
     * Tool to format data into a stream. 
     */
    friend ostream& operator<<(ostream& output, const NavPointList &n);

protected:
    /*! A list of TVector3, these are 3 dimensional and contain text. */
    TList*        fNavPoints;
    /*! What is the type of point, available are, Waypoint, Event... */
    Int_t         fType;
    /*! XY or Lat Lon storage? */
    Bool_t        fIsXY;
    /*! Current filename assigned. */
    TString*      fFilename;
    /*! tell the program to save now. False - we are up to date. */
    Bool_t        fSave;
    /*! Has the file ever been written to? */
    Bool_t        fFirst; 
    /*! A class that deals with how to name the files. */
    FileName*     fFileName;
    /*! A Tstring to hold any file specific title. */
    TString*      fTitle;

    /*! Ordinal point count. */
    UInt_t fNavPointCount;

private:
    /*! A private funciton to write the header for the file. */
    Bool_t        WriteHeader(ofstream &o);
    UInt_t        fError;
};
#endif
