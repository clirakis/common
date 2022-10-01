/**
 ******************************************************************
 *
 * Module Name : geo_header.h
 *
 * Author/Date : C.B. Lirakis / 09-Sep-01
 *
 * Description : Header file to store the GEO data associated with
 * nos map files. 
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
#ifndef __GEO_HEADER_hh_
#define __GEO_HEADER_hh_
#include <vector>
#include <float.h>

/// Define a structure to hold the anual declination variation. 
typedef struct t_magnetic {
    double Declination, Change;
    int Year;
} MAGNETIC;

class LatLon 
{
public:
    /// Constructor, input LAT/Lon
    LatLon(const double Lat=90.0,const double Lon=180.0) 
    {latitude = Lat; longitude = Lon;visible=true;};
    /// Constructor with character data. 
    LatLon(const string &value);

    /// Access to private data.
    inline double Latitude() const {return latitude;};
    inline double Longitude() const {return longitude;};
    inline void   SetLatitude(const double X)  {latitude  = X;};
    inline void   SetLongitude(const double X) {longitude = X;};

    /// overloaded operators
    LatLon operator = (const LatLon &L);

    /// Control visibilty for drawing.
    inline bool Visible() const {return visible;};
    inline void SetVisible(bool s=false) {visible=s;};

    /// Print them out in a reasonable way
    friend ostream& operator<<(ostream& output, const LatLon &n);

private:
    double latitude, longitude;
    bool visible;
};

/// Define a class points that is a child of LatLon
class MapPoints : public LatLon
{
public:
    MapPoints(const double Lat=0.0, const double Lon=0.0, 
	      unsigned x=0, unsigned y=0):LatLon(Lat,Lon) {X0=x; Y0=y;};
    MapPoints(const string &value);
    inline int X() const {return X0;};
    inline int Y() const {return Y0;};

    /// overloaded operators
    inline bool operator==(const MapPoints &MP) 
    {return((X0==MP.X0)&&(Y0==MP.Y0));}
    inline bool operator!=(const MapPoints &MP) {return !(*this==MP);};
    double   SlopeX(const MapPoints &in) const;
    double   SlopeY(const MapPoints &in) const;
    unsigned Distance(const MapPoints &in) const;
    unsigned Distance(unsigned px, unsigned py) const;

    /// Print them out in a reasonable way
    friend ostream& operator<<(ostream& output, const MapPoints &n);
    
private:
    int  X0, Y0;
};

class XYLine
{
public:
    XYLine() {Mx = My = Bx = By = 0.0;};
    XYLine(const MapPoints &one, const MapPoints &two, const MapPoints &three);
    XYLine(const MapPoints &one, const MapPoints &two);
    LatLon Interpolate( unsigned px, unsigned py) const;
    MapPoints Interpolate (const LatLon &ll) const;

    XYLine operator = (const XYLine &n) {Mx=n.Mx;My=n.My;Bx=n.Bx;By=n.By;return *this;};

private:
    double Mx, My, Bx, By;
};

class GEO_Map
{
 public:
    /// Constructor.
    GEO_Map(const char *filename);
    /// Destructor
    ~GEO_Map();
    void Dump();

    /// Access to private variables.
    inline int    GetError()        const {return ErrorCode;};
    inline int    GetChartID()      const {return Id;};
    inline double GetChartScale()   const {return Scale;};
    inline double GetChartSkew()    const {return SkewAngle;} // In degrees
    inline LatLon GetUpperLeftLL()  const {return *UL;};
    inline LatLon GetLowerRightLL() const {return *LR;};
    inline int    GetResolution()   const {return Resolution;};
    inline string GetNOSName()      const {return *NOS_Name;};
    /// Access to the number of reference points
    inline int    GetRefNumber()    const {return Points.size();};
    const MapPoints *Reference(unsigned i) const;

    /**
     * The following function is applicable to this class
     * and should not be taken as a projection in the geodesy sense. 
     * It is a method for turning the XY-pixels to a corresponding 
     * LL. 
     */
    LatLon XY2LL(unsigned px, unsigned py);
    /// The reverse function. 
    MapPoints LL2XY(const LatLon &n);

private:
    /**
     * Name of chart. 
     */
    string *NOS_Name;
    /**
     * Type of chart: NAVIGATION...
     */
    string *Type;
    /**
     * Source - Who is responsible for the navigation data. EG: CHS, NOAA, 
     * NIMA/DMA
     */
    string *Source;
    /**
     * Chart Country. 
     */
    string *Country;
    /**
     * Provience/State covered by chart.
     */
    string *Province;
    /**
     * Order - order of equation used to georefernce the chart. Typically 2.
     */
    int    Order;
    /**
     * Chart ID
     */
    int    Id;
    /**
     * Name of chart
     */
    string *Name;
    /**
     * Edition
     */
    int    Edition;
    /**
     * Date published.
     */
    string *Published;
    /** 
     * Updated on: 
     */
    string *Updated;
    /// Units: Feet, Meters, Fathoms
    string *Depth_Units;
    /// mean low water.... What is the depth referenced to?
    string *SoundingDatum;
    /// Datum used in georeference, NAD83, ...
    string *Datum;
    /**
     * Offset for geographic coordinate points from the source to WGS84.
     * 
     */
    LatLon *Offset;
    /// Scale of chart, also skew angle of chart
    double Scale, SkewAngle;
    /// Projection used. EG: Mercator ...
    string *Projection;
    /// number of DPI 
    int    Resolution;
    /// Magnetic declination
    struct t_magnetic magnetic;
    /**
     * Upper left and lower right lat/lon of chart.
     */
    LatLon *UL, *LR;
    /// Bounding polygon
    vector <LatLon *> Vertex;

    /**
     * Points made up of lat lon and xy 
     * These points may be used for a polygon fit of the data
     * such that an input of x,y will yield a lat lon output.
     */
    vector <MapPoints *> Points;
    XYLine *fMaster;  // Master slope record using endpoints of Points.
    /// Error code
    int ErrorCode;
    
    // Private functions.
    void ParseMainChart(const char *line);
    void FillMaster();
    XYLine GetSlope( unsigned px, unsigned py);

};
void ToDegMin( double in, int *deg, double *min);

#endif
