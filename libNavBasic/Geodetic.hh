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
 * References : https://geographiclib.sourceforge.io/1.49/C/geodesic_8h.html
 * http://proj4.org/index.html
 *
 *******************************************************************
 */
#ifndef __GEODETIC_h_
#define __GEODETIC_h_
#    include <stdint.h>
/*
 * Include file from proj.4
 */
#    include <proj_api.h>

/*
 * my include files.
 */
#    include "Point.hh"

class Geodetic 
{
public: // public access methods.


    enum PROJECTION {MERCATOR=0, UTM};

    /*!
     * This is the constructor for the Geodetic utilities. 
     * It represents a c++ wrapper around the proj4 library. 
     * The inputs are in decimal degrees as shown by the default. 
     */
	Geodetic(const double &LatDegrees = 41.5, 
		 const double &LonDegree=-71.2, 
		 PROJECTION p=UTM);

    /*!
     * This closes the proj library and cleans up after ourselves.
     */
    ~Geodetic(); 

    /*! Static method for gaining access to the geodetic pointer.  */
    static Geodetic* GetThis(void) {return fGeo;};

    /*!
     * Convert a Lat Lon in RADIANS! to XY in meters.
     */
    Point ToXY(const double &Lon, const double &Lat, const double &Z) const;

    /*!
     * Convert a Lat Lon in RADIANS! to XY in meters in place
     * Input: 
     *   Lon_X - longitude for conversion in radians
     *   Lat_Y - latitude for conversion in radians
     *
     * Returns in place
     *   Lon_X - Projected X in meters
     *   Lat_Y - Projected Y in meters
     * 
     */
    void ToXY(double &Lon_X, double &Lat_Y) const;

    /*!
     * Point in format of Lat,Lon, Z
     */
    Point ToXY(const Point &p) const;

    /*!  
     * XY->LL transform - returns a point. 
     */
    Point  ToLL(const double &X, const double &Y, const double &Z) const;

    /*!
     * Convert a XY XY in meters to Lat Lon in RADIANS in place
     * Input: 
     *   Lon_X - Projected X in meters
     *   Lat_Y - Projected Y in meters
     *
     * Returns in place
     *   Lon_X - longitude for conversion in radians
     *   Lat_Y - latitude for conversion in radians
     * 
     */
    void ToLL(double &Lon_X, double &Lat_Y) const;

    /*! 
     * Same with Point input 
     * 05-Mar-22, test 2
     */
    inline Point ToLL(const Point &p) const {return ToLL( p.X(), p.Y(), p.Z());};

    /*! Inline access method for returning the current
     *  projection Center latitude - Radians.
     */
    inline double CenterLat(void) const {return fLat;};
    /*! 
     * Inline access method for returning the current projection
     * center longitude. - Radians
     */
    inline double CenterLon(void) const {return fLon;};

    /*! Return the current projection center. */
    inline const Point &XY0(void) const {return fXY;};

private:  // Private data and methods.

    void SetProjection(const double &LatDegrees, const double &LonDegree, 
		       PROJECTION p);

    double   fLat, fLon;        // Current projection center in radians.
    Point    fXY;               // Current projection center in meters
    projPJ   fProjection;       // Pointer to projection in use
    uint8_t  fZone;

    static Geodetic* fGeo;
};

#endif
