/********************************************************************
 *
 * Module Name : geo_header.cpp
 *
 * Author/Date : C.B. Lirakis / 09-Sep-01
 *
 * Description : Support code for reading, creating and deleting 
 * the GEO file. 
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
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cstring>
#include <cmath>
#include <fstream>



// Local Includes.
#include "geo_header.hh"

#define MAX(a,b) ((a)>(b) ? (a):(b))

void ToDegMin( double in, int *deg, double *min)
{
    double Deg;
    if (in>0.0)
    {
	Deg = floor(in);
    }
    else
    {
	Deg = ceil(in);
    }
    *deg = (int) Deg;
    *min = (fabs(in)-fabs(Deg))*60.0;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
LatLon::LatLon(const string &value)
{
    // Find the delimeter
    unsigned N = value.find(" ");
    visible = false;
    if (N!=string::npos)
    {
	string Lon = value.substr(0,N);
	int N2 = value.substr(N+1,value.length()).find(" ");
	if (N2<=0)
	{
	    N2 = value.length();
	}
	string Lat = value.substr(N+1,N2);

        latitude   = atof(Lat.c_str());
	longitude  = atof(Lon.c_str());
	visible    = true;
    }
}

/**
 ******************************************************************
 *
 * Function Name : equals operator
 *
 * Description : make left and right equal.
 *
 * Inputs : LatLon point
 *
 * Returns : LatLon point
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
LatLon LatLon::operator = (const LatLon &L)
{
    latitude  = L.latitude;
    longitude = L.longitude;
    visible   = L.visible;
    return *this;
}
/**
 ******************************************************************
 *
 * Function Name :operator << used with LatLon
 *
 * Description : Dump the point data. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const LatLon &n)
{
    double min;
    int    deg;

    ToDegMin ( n.latitude, &deg, &min);
    output << "Lat: " << deg << " " << min;

    ToDegMin(n.longitude, &deg, &min);
    output <<  " Lon: " << " " << deg << " " << min;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
MapPoints::MapPoints(const string &value):LatLon(value)
{
    // Find the delimeter
    unsigned N = value.find(" "); // First Space
    if (N != value.npos)
    {
	// Find the second space
	string temp = value.substr(N+1,value.length());
	N = temp.find(" "); // Second space. 
	if (N!=temp.npos)
	{
	    temp = temp.substr(N+1,temp.length());
	    // Find the third space. 
	    N = temp.find(" ");
	    if (N != temp.npos)
	    {
		Y0 = atoi(temp.substr(0,N).c_str());
		X0 = atoi(temp.substr(N+1,temp.length()).c_str());
	    }
	}
    }
}

/**
 ******************************************************************
 *
 * Function Name : SlopeX
 *
 * Description : Return the slope in X (Longitude) for the 2 points
 *
 * Inputs : secondary point
 *
 * Returns : slope of system
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 29-Apr-09
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double MapPoints::SlopeX(const MapPoints &in) const
{
    double dx, M;
    dx = (double) (X0 - in.X());
    if (fabs(dx) > 0.01)
    {
	M = (Longitude() - in.Longitude())/dx;
    }
    else
    {
	M = 0.0;
    }
    return M;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double MapPoints::SlopeY(const MapPoints &in) const
{
    double dx, M;
    dx = (double) (Y0 - in.Y());
    if (fabs(dx) > 0.01)
    {
	M = (Latitude() - in.Latitude())/dx;
    }
    else
    {
	M = 0.0;
    }
    return M;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned MapPoints::Distance(const MapPoints &in) const
{
    double dx, dy;
    dx = (double) (X0 - in.X());
    dy = (double) (Y0 - in.Y());
    return (unsigned) sqrt(dx*dx + dy*dy);
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned MapPoints::Distance(unsigned px, unsigned py) const
{
    double dx, dy;
    dx = (double) (X0 - px);
    dy = (double) (Y0 - py);
    return (unsigned) sqrt(dx*dx + dy*dy);
}

/**
 ******************************************************************
 *
 * Function Name :operator << used with MapPoints
 *
 * Description : Dump the point data. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const MapPoints &n)
{
    double min;
    int    deg;

    ToDegMin ( n.Latitude(), &deg, &min);
    output << "Lat: " << deg << " " << min;

    ToDegMin(n.Longitude(), &deg, &min);
    output <<  " Lon: " << " " << deg << " " << min;

    output << " " << n.X0
	   << " " << n.Y0;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name :GEO_Map constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GEO_Map::GEO_Map (const char *filename)
{
    ErrorCode = 0;

    NOS_Name = NULL;
    Type     = NULL;
    Source   = NULL;
    Country  = NULL;
    Province = NULL;
    Name     = NULL;
    Published = NULL;
    Updated  = NULL;
    Depth_Units = NULL;
    SoundingDatum = NULL;
    Datum    = NULL;
    Offset   = NULL;
    Projection = NULL;
    UL       = NULL;
    LR       = NULL;

    /**
     * We need to open the file. Do the usual checks,
     * 1) is the filename NULL?
     * 2) Does it already contain the geo extention?
     *
     * We assume that the parent application is storing the path for us. 
     */
    if (filename == NULL)
    {
	ErrorCode = 1;
	return;
    }

    string FileName;
    if (strchr(filename,'.') == NULL)
    {
	// We don't have a suffix, add one. 
	FileName = string(filename) + string(".geo");
    }
    else
    {
	FileName = string(filename);
    }

    // Open the file and start to read it. 
    ifstream geo(FileName.c_str());
    if (geo.fail())
    {
	// Error opening file. Maybe it doesn't exist. 
	ErrorCode = 2;
	return;
    }
    char buffer[256];
    do {
	geo.getline(buffer,sizeof(buffer));
	if (!geo.fail())
	{
	    // Parse the line. 
	    ParseMainChart(buffer);
	}
    } while (!geo.eof());
    geo.close();
    FillMaster();
}
/********************************************************************
 *
 * Function Name : GEO_Map destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
GEO_Map::~GEO_Map ()
{
    if (NOS_Name)
	delete NOS_Name;
    if (Type)
	delete Type;
    if (Source)
	delete Source;
    if (Country)
	delete Country;
    if (Province)
	delete Province;
    if (Name)
	delete Name;
    if (Published)
	delete Published;
    if (Updated)
	delete Updated;
    if (Depth_Units)
	delete Depth_Units;
    if (SoundingDatum)
	delete SoundingDatum;
    if (Datum)
	delete Datum;
    if (Offset)
	delete Offset;
    if (Projection)
	delete Projection;
    if (UL)
	delete UL;
    if (LR)
	delete LR;

    vector <LatLon *>::iterator Vert;
    vector <MapPoints *>::iterator MP;

    for (Vert = Vertex.begin(); Vert != Vertex.end(); Vert++)
    {
	delete (LatLon *) *Vert;
    }
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	delete (MapPoints *) *MP;
    }
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GEO_Map::ParseMainChart(const char *line)
{
    const string VV("Vertex");
    const string PP("Point");
    int      Number;
    string   in(line);
    int      NumberVert = 0;
    unsigned N=in.find("=");

    if ((N != in.length()) && N>0)
    {
	// We have an assignment
	string Keyword = in.substr(0,N);
	string Data    = in.substr(N+1,in.length());

	if (Keyword == string("Bitmap"))
	{
	    NOS_Name = new string(Data);
	}
	else if (Keyword == string("Type"))
	{
	    Type = new string(Data);
	}
	else if (Keyword == string("Source"))
	{
	    Source = new string(Data);
	}
	else if (Keyword == string("Country"))
	{
	    Country = new string(Data);
	}
	else if (Keyword == string("Province/State"))
	{
	    Province = new string(Data);
	}
	else if (Keyword == string("Order"))
	{
	    Order = atoi(Data.c_str());
	}
	else if (Keyword == string("Id"))
	{
	    Id = atoi(Data.c_str());
	}
	else if (Keyword == string("Name"))
	{
	    Name = new string(Data);
	}
	else if (Keyword == string("Source Edition"))
	{
	    Edition = atoi(Data.c_str());
	}
	else if (Keyword == string("Last Update"))
	{
	    Updated = new string(Data);
	}
	else if (Keyword == string("Date Published"))
	{
	    Published = new string(Data);
	}
	else if (Keyword == string("Depth Units"))
	{
	    Depth_Units = new string(Data);
	}
	else if (Keyword == string("Sounding Datum"))
	{
	    SoundingDatum = new string(Data);
	}
	else if (Keyword == string("Datum"))
	{
	    Datum = new string(Data);
	}
	else if (Keyword == string("Latitude Offset"))
	{
	    if (Offset == NULL)
	    {
		Offset = new LatLon(atof(Data.c_str()),0.0);
	    }
	    else
	    {
		Offset->SetLatitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("Longitude Offset"))
	{
	    if (Offset == NULL)
	    {
		Offset = new LatLon(0.0,atof(Data.c_str()));
	    }
	    else
	    {
		Offset->SetLongitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("Scale"))
	{
	    Scale = atof(Data.c_str());
	}
	else if (Keyword == string("Skew Angle"))
	{
	    SkewAngle = atof(Data.c_str());
	}
	else if (Keyword == string("Projection"))
	{
	    Projection = new string(Data);
	}
	else if (Keyword == string("Resolution"))
	{
	    Resolution = atoi(Data.c_str());
	}
	else if (Keyword == string("Magnetic Declination"))
	{
	    magnetic.Declination = atof(Data.c_str());
	}
	else if (Keyword == string("Magnetic Declination Date"))
	{
	    magnetic.Year = atoi(Data.c_str());
	}
	else if (Keyword == string("Magnetic Declination Change"))
	{
	    magnetic.Change = atof(Data.c_str());
	}
	else if (Keyword == string("UL Latitude"))
	{
	    if (UL == NULL)
	    {
		UL = new LatLon(atof(Data.c_str()),0.0);
	    }
	    else
	    {
		UL->SetLatitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("UL Longitude"))
	{
	    if (UL == NULL)
	    {
		UL = new LatLon(0.0,atof(Data.c_str()));
	    }
	    else
	    {
		UL->SetLongitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("LR Latitude"))
	{
	    if (LR == NULL)
	    {
		LR = new LatLon(atof(Data.c_str()),0.0);
	    }
	    else
	    {
		LR->SetLatitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("LR Longitude"))
	{
	    if (LR == NULL)
	    {
		LR = new LatLon(0.0,atof(Data.c_str()));
	    }
	    else
	    {
		LR->SetLongitude(atof(Data.c_str()));
	    }
	}
	else if (Keyword == string("NumberVertices"))
	{
	    NumberVert = atoi(Data.c_str());
	}
	else if (Keyword.find(VV)<VV.length())
	{
	    Number = atoi(Keyword.substr(VV.length(),N).c_str());
	    LatLon *pt = new LatLon(Data);
	    Vertex.push_back(pt);
	}
	else if (Keyword.find(PP)<PP.length())
	{
	    Number = atoi(Keyword.substr(PP.length(),N).c_str());
	    MapPoints *pt = new MapPoints(Data);
	    Points.push_back(pt);
	}
    }
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GEO_Map::FillMaster()
{
    vector <MapPoints *>::iterator MP;
    MapPoints *point1, *point2;
    int  r, rmin, xmax, ymax;
    int index_x, index_y, count;

    xmax    = ymax    = 0;
    index_x = index_y = 0;
    count = 0;
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	r  = (*MP)->Distance(0, 0);
	if (r<rmin)
	{
	    rmin = r;
	    point1 = *MP;	    
	}
	if ((*MP)->X()>xmax)
	{
	    index_x = count;
	    xmax = (*MP)->X();
	}
	if ((*MP)->Y()>ymax)
	{
	    index_y = count;
	    ymax = (*MP)->Y();
	}
	count++;
    }
    if (index_x == index_y)
    {
	point2 = MP[index_x];
	fMaster = new XYLine( *point1, *point2);
    }
    else
    {
	fMaster = new XYLine();
	cout << "Index error." << endl;
    }
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description : Identify the nearest points and slope for 
 * interpolation. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
XYLine GEO_Map::GetSlope( unsigned px, unsigned py)
{
    vector <MapPoints *>::iterator MP;
    MapPoints *point, *point1, *point2, *point3;
    unsigned  r, rmin;
    int       index1, index2, index3, count;

    rmin = 1000000000;
    point1 = point2 = NULL;

    /*
     * Find the nearest map point by minimizing the 
     * distance. 
     */
    index1 = index2 = 0;
    count = 0;
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	r  = (*MP)->Distance(px, py);
	if (r<rmin)
	{
	    rmin = r;
	    point1 = *MP;	    
	    index1 = count;
	}
	count++;
    }

    /* 
     * So now I have a close point. 
     * I need one going horizontal and one going vertical.
     * Find the horizontal one. 
     */
    count = 0;
    // Now exclude that point and find the next nearest. 
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	point = *MP;
	if ( (fabs(point->Y()-point1->Y()) < 5.0) && ( *point != *point1) )
	{
	    point2 = *MP;
	    index2 = count;
	    break;
	}
	count++;
    }

    // And the vertical
    count = 0;
    // Now exclude that point and find the next nearest. 
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	point = *MP;
	if ( (point->X()==point1->X()) && ( *point != *point1) )
	{
	    point3 = *MP;
	    index3 = count;
	    break;
	}
	count++;
    }
#if 0
    cout << "Point1: " << index1 << " " << *point1 
	 << endl
	 << "Point2: " << index2 << " " << *point2
	 << endl
	 << "Point3: " << index3 << " " << *point3
	 << endl;
#endif

    return XYLine( *point1, *point2, *point3);
}
/**
 ******************************************************************
 *
 * Function Name : XY2LL
 *
 * Description : Using the map points provided, convert 
 * the pixel coordinates to corresponding LL. 
 *
 * Inputs : px and py
 *
 * Returns : Latitude and Longitude. 
 *
 * Error Conditions : returns (0,0) if off the map.
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
LatLon GEO_Map::XY2LL(unsigned px, unsigned py)
{
    XYLine p = GetSlope( px, py);
    return p.Interpolate(px, py);
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
MapPoints GEO_Map::LL2XY(const LatLon &n)
{
    MapPoints point;
    /* Use the fMaster linear interpolation to get close */
    point = fMaster->Interpolate(n);

    /* Now use the value point to get the closer verticies */
    XYLine p = GetSlope( point.X(), point.Y());

    /* And now use it. */
    return p.Interpolate(n);
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const MapPoints *GEO_Map::Reference(unsigned i) const
{
    MapPoints *rv;

    if (i<Points.size())
    {
	rv = Points[i];
    }
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GEO_Map::Dump()
{
    vector <MapPoints *>::iterator MP;
    vector <LatLon *>::iterator LL;

    cout << "Geo: " << *NOS_Name << endl
	 << "Type: " << *Type  << endl
	 << "Source: " << *Source 
	 << endl
	 << " Country: " << *Country
	 << " Province: " << *Province
	 << endl
	 << "Fit Data: " << Order
	 << " " << Id << " " << *Name
	 << endl
	 << "Offset: " << Offset->Latitude() << " " << Offset->Longitude()
	 << endl
	 << "Skew: " << SkewAngle << " Number verticies: " << Vertex.size()
	 << endl;
    cout << "Bounding Polygon: " << endl;

    for (LL=Vertex.begin(); LL != Vertex.end();LL++)
    {
	cout << (*LL)->Latitude()
	     << " " << (*LL)->Longitude()
	     << endl;
    }

    cout << "Map Points: " << endl;
    for (MP=Points.begin(); MP != Points.end();MP++)
    {
	cout << (**MP) << endl;
    }
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
XYLine::XYLine(const MapPoints &one, const MapPoints &two, 
	       const MapPoints &three)
{
    Mx = one.SlopeX(two);
    My = one.SlopeY(three);
    Bx = one.Longitude()  - Mx*one.X();
    By = one.Latitude()   - My*one.Y();
#if 0
    cout << "MX: " << Mx  << " Bx " << Bx
	 << " MY: " << My << " By " << By
	 << endl;
#endif
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
XYLine::XYLine(const MapPoints &one, const MapPoints &two)
{
    Mx = one.SlopeX(two);
    My = one.SlopeY(two);
    Bx = one.Longitude()  - Mx*one.X();
    By = one.Latitude()   - My*one.Y();
#if 0
    cout << "MX: " << Mx  << " Bx " << Bx
	 << " MY: " << My << " By " << By
	 << endl;
#endif
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
LatLon XYLine::Interpolate( unsigned px, unsigned py) const
{
    LatLon p( py*My+By, px*Mx+Bx);
#if 0
    cout << " Lat: " << p.Latitude() << " Lon: " << p.Longitude() << endl;
#endif
    return p;
}
/**
 ******************************************************************
 *
 * Function Name : Interpolate
 *
 * Description : interpolate between LL points to get XY points. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
MapPoints XYLine::Interpolate( const LatLon &ll) const
{
    MapPoints p( ll.Latitude(), ll.Longitude(), 
		 (unsigned)((ll.Longitude()-Bx)/Mx), 
		 (unsigned)((ll.Latitude()-By)/My));
#if 0
    cout << " X: " << p.X() << " Y: " << p.Y() << endl;
#endif
    return p;
}
