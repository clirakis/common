/**
 ******************************************************************
 *
 * Module Name : Geodetic.hh
 *
 * Author/Date : C.B. Lirakis / 09-Feb-06
 *
 * Description : Wrapper around the proj-4 projection library
 * so that we can use the Root physics vectors.
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
#ifndef __GEODETIC_h_
#define __GEODETIC_h_
/*
 * Include file from proj.4
 */
#include "projects.h"

/*
 * Root include files.
 */
#include <TVector2.h>

class TEnv;

class Geodetic 
{
public: // public access methods.


    enum ePROJECTION {kMERCATOR=0, kUTM};

    /*!
     * This is the constructor for the Geodetic utilities. 
     * It represents a c++ wrapper around the proj4 library. 
     * The inputs are in decimal degrees as shown by the default. 
     */
    Geodetic(double LatDegrees = 41.5, double LonDegree=-71.2, ePROJECTION p=kUTM);

    /*!
     * This enables us to load parameters through the TEnv file. 
     */
    Geodetic(TEnv *);
    /*!
     * This closes the proj library and cleans up after ourselves.
     */
    ~Geodetic(void) { pj_free(fCurrentProjection);};

    /*! Static method for gaining access to the geodetic pointer.  */
    static Geodetic* GetGeodetic(void) {return fGeo;};

    /*! Load root preferences 
     * E.G. Projection Lat/Lon center. 
     */
    void   LoadPrefs(TEnv*);

    /*! Save root preferences */
    void   SavePrefs(TEnv*);


    /*!
     * Convert a Lat Lon in RADIANS! to XY in meters based 
     * on the projection center. 
     * Input is Latitdue and then Longitude
     */
    TVector2 ToXY(double Lat, double Lon) const;

    /*!
     * Convert an XY to Lat Lon in RADIANS! based
     * on the projection center. 
     * Input is a TVector
     */
    inline TVector2 ToXY(const TVector2 &Fp) const {
	return ToXY (Fp.X(),Fp.Y());};

    /*! Project X and Y in METERS to Lat and Lon in RADIANS
     * based on the current projection center. 
     */
    TVector2 ToLL(double X, double Y) const;


    /*! Project X and Y in METERS to Lat and Lon in RADIANS
     * based on the current projection center. 
     * Input here is a TVector3
     */
    inline TVector2 ToLL(const TVector2 &Fp) const { 
	return ToLL(Fp.X(),Fp.Y());};

    /*! Inline access method for returning the current
     *  projection Center latitude - Radians.
     */
    inline Double_t CenterLat(void) const {return fLat;};
    /*! 
     * Inline access method for returning the current projection
     * center longitude. - Radians
     */
    inline Double_t CenterLon(void) const {return fLon;};
    inline TVector2 XY0(void) const {return fXY;};

private:  // Private data and methods.

    void SetProjection(double LatDegrees, double LonDegree);

    Double_t fLat, fLon;        // Current projection center in radians.
    TVector2 fXY;               // Current projection center in meters
    Int_t    fZone; 
    PJ*      fCurrentProjection;
    ePROJECTION fSelectedProjection; 
    static Geodetic* fGeo;
};

#endif
