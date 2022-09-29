/**
 ******************************************************************
 *
 * Module Name : NavPoint.hh
 *
 * Author/Date : C.B. Lirakis / 26-Nov-15
 *
 * Description : Generic navigation point. Major overhaul, 
 *               now inherits from TVector3, this inherits from TObject. 
 *               we will store fNumber in UniqueID of TObject
 *               and we will override GetTitle for the charater data. 
 *               The status bits will be held and manipulated in 
 *               SetBit, ResetBit, TestBit, TestBits InvertBit...  
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 26-Mar-22  CBL added in fStatusBits
 * 27-Mar-22  CBL Added in kTRACK
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __NAVPOINT_hh_
#define __NAVPOINT_hh_
#  include <TVector3.h>
#  include <TString.h>
/* 
 * Define the string seperator for all the log files. 
 * Using , enables us to import the data to excel. CSV
 */
#  define NAV_SEPARATOR ","

class TCompositeFrame;

class NavPoint : public TVector3
{

public:
    enum NavPointType {kWAYPOINT=0, kEVENT, kTRACK, kNONE};
    enum NavPointStatus {
	kPOINT_CONNECT = BIT(0),
	kPOINT_USER    = BIT(16)
    };
    

    /**
     * the navigation point is the base class for all points created 
     * on the screen. X typically will be Longitude, and Y will typically
     * be Latitude. n is some name. 
     * Bits that are offlimits are: 0, 3, 4, 5, 6, 8, 13
     */
    NavPoint(Double_t X, Double_t Y, Double_t Z=0.0, 
	     Int_t type=NavPoint::kWAYPOINT,
	     Double_t dt=0.0, const char *n=NULL);

    /*
     * Constuctor used typically with a readback from a file where
     * the format is specified on save. 
     */
    NavPoint(const char *s);
    /*
     * Destructor. 
     */
    ~NavPoint(void);

    void SetName(const char*);

    const char* GetName(void) const {if (fName) return fName->Data(); else return NULL;};

    /*!
     * SameName, provide a character name as we are cycling through a
     * list searching for a specific point. This returns true
     * when the name provided equals the name of the point. 
     */
    Bool_t SameName(const char *);

    inline Double_t GetDt(void) const {return fdt;};
    inline Int_t    Type(void)  const {return fType;};
    inline Double_t Epoch(void) const { if (fTime) return *fTime; else return 0.0;};
    void SetTime(UInt_t seconds, UInt_t milliseconds);

    /*! 
     * Range
     * Determine the distance between two NavPoints 
     * if a variable address for Phi is provided, the 
     * true bearing between the two points is also given. 
     */
    Double_t Range(const NavPoint&, Double_t *Bearing=NULL);

    /*! Compute line parameters */
    Double_t ComputeLine(const NavPoint&, const NavPoint&);

    /*!
     * See bit fields above. 
     */
    inline Bool_t TestBit(UInt_t bit) const { return (fStatusBits&bit);};
    void   SetBit (UInt_t bit, Bool_t val=kTRUE);

    /*!
     * Tool to format data into a stream. 
     */
    friend ostream& operator<<(ostream& output, const NavPoint &n);

private:
    /*!
     * Set the character data for the point. 
     */
    void SetData(const char*);

   /*!
     * A unique count on the number of navpoints we have. 
     */
    static unsigned int fCount;

    /*!
     * What type of point is this? See ENUM above. 
     */
    Int_t         fType;
    /*!
     * Status bits that affect how the points are rendered. 
     */
    UInt_t        fStatusBits;    // Used to tell the system to do other stuff

    /*!
     * Some name or other character string associated with the point.
     * could be a name. 
     */
    TString       *fName;

    /*! 
     * Maybe you want time in here too. Create a field for that. 
     * Seconds since epoch. 
     */
    Double_t       *fTime; 
    /*!
     * Integration time, where applicable. 
     * In the case where I create a point from a distribution 
     * this is the integration time. 
     */
    Double_t       fdt;
};
#endif
