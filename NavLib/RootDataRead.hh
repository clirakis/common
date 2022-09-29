/**
 ******************************************************************
 *
 * Module Name : RootDataRead.hh
 *
 * Author/Date : C.B. Lirakis / 22-Feb-22
 *
 * Description : Template for a main class
 *
 * Restrictions/Limitations : none
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
#ifndef __ROOTDATAREAD_hh_
#define __ROOTDATAREAD_hh_
#    include <iostream>
#    include <TVector3.h>
class TFile;
class TTree;

class RootDataRead 
{
public:

    /**
     * Constructor the root read tool. 
     * Filename to read
     */
    RootDataRead(const char *File);

    /**
     * Destructor for SK8. 
     */
    ~RootDataRead(void);

    /**
     * Error of any kind?
     */
    inline Bool_t Error(void) const {return fError;};

    /*! Access the This pointer. */
    static RootDataRead* GetThis(void) {return fRootDataRead;};

    /**
     * Main Module DO
     *   Use mostly for testing.
     */
    void Do(void);

    inline void Rewind(void) {fCurrent = 0;};

    inline void Zero(void) {fTime=fLat=fLon=fZ=fVE=fVN=fVZ=fHDOP=0.0; fNSV=0;};

    inline UInt_t CurrentIndex(void) const {return fCurrent;};

    /*
     * TVector3 data after read. 
     * All other data stored internally. 
     * -1 means just use internal index. 
     */
    const TVector3 &ReadPoint(Long64_t i=-1);

    inline Long64_t NEntries(void) const {return fNentries;};

    friend std::ostream& operator<<(std::ostream& output, const RootDataRead &n);

private:

    // Pointer to file
    TFile   *fFile; 
    // Pointer to tree in file. 
    TTree   *fTree;
    // Last point read
    TVector3 fPoint;

    Long64_t fNentries;
    Long64_t fCurrent;

    Double_t fTime, fLat, fLon, fZ, fVE, fVN, fVZ, fHDOP; 
    UChar_t  fNSV;
    Bool_t   fError;

    /// Open the file and ntuple. 
    bool OpenFile(const char* Filename);

    /*! The static 'this' pointer. */
    static RootDataRead *fRootDataRead;

};
#endif
