/**
 ******************************************************************
 *
 * Module Name : Point.hh
 *
 * Author/Date : C.B. Lirakis / 13-Feb-16
 *
 * Description : Original from years ago, this is a bit of rework. 
 * renamed as point, deals mostly with cartesian values, but will convert
 * from cartesian back and forth. 
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
#ifndef __POINT_hh_
#define __POINT_hh_

#include <cmath>
#include "tools.h"

/// Point documentation here. Standard RHS.
class Point {
public:
    friend std::ostream& operator<<(std::ostream& output, const Point &in);

    /// Default Constructor
    Point(const double &x=0.0, const double &y=0.0, const double &z=0.0) {fX=x;fY=y;fZ=z;};

    /// Constructor from another cartesian. 
    Point(const Point &c) {fX=c.fX;fY=c.fY;fZ=c.fZ;};

    inline void Zero(void) {fX=0.0;fY=0.0;fZ=0.0;};
    inline bool EqualsZero(void) const {return ((fX==0.0)&&(fY==0.0)&&(fZ==0.0));};

    /*! Assume a polar input */
    void Polar(double r, double phi, double Z=0.0);

    /// Default destructor

    /// Access methods. 
    inline double X() const {return fX;};
    inline double Y() const {return fY;};
    inline double Z() const {return fZ;};
    inline void X(const double val) {fX=val;};
    inline void Y(const double val) {fY=val;};
    inline void Z(const double val) {fZ=val;};
    inline void Set(Point& p) {fX=p.X();fY=p.Y();fZ=p.Z();};
    inline void Set(double X=0.0, double Y=0.0, double Z=0.0) 
	{fX=X;fY=Y;fZ=Z;};

    /*!
     * Rotate about  x axis by angle specified. 
     * Changes state of this pointer, and returns value of this pointer. 
     */
    Point &RotateX(const double angle);

    /*!
     * Rotate about  y axis by angle specified. 
     * Changes state of this pointer, and returns value of this pointer. 
     */
    Point &RotateY(const double angle);

    /*!
     * Rotate about  z axis by angle specified. 
     * Changes state of this pointer, and returns value of this pointer. 
     */
    Point &RotateZ(const double angle);

    // Operator methods.
    /*! Equate two points. */
    inline Point &operator = (const Point &P) 
	{ fX = P.fX; fY=P.fY;fZ=P.fZ; return *this;};
    /*! Vector addition add P to this */
    inline Point &operator +=(const Point &P) 
	{ fX += P.fX; fY+=P.fY;fZ+=P.fZ; return *this;};

    /*! Vector subtract */
    inline Point &operator -=(const Point &P)
	{ fX -= P.fX; fY-=P.fY;fZ-=P.fZ; return *this;};

    /*! Multiplication by a constant. */
    inline Point &operator *=(const double value) 
	{ fX*=value;fY*=value;fZ*=value; return *this;};

    /*! Division by a constant */
    inline Point &operator /=(const double &value)
	{fX /= value; fY/=value;fZ/=value; return *this;};

    /*! Return magnitude squared of vector. */
    inline double Magnitude2(void) const 
	{return (fX*fX + fY*fY + fZ*fZ);};

    /*! Return magnitude of vector. */
    inline double Magnitude(void) const 
	{return sqrt(fX*fX + fY*fY + fZ*fZ);};

    /*! Return magnitude squared of XY. */
    inline double Mod2(void) const 
	{return (fX*fX + fY*fY);};

    /*! Return magnitude of XY. */
    inline double Mod(void) const 
	{return sqrt(fX*fX + fY*fY);};

    /*! Return distance between two points */
    inline double Distance (const Point &Fp) const 
	{Point rv(*this); rv-=Fp; return rv.Magnitude(); };

    /*! return the dot product, same as * above. */
    double Dot(const Point &P) const; 

    /*! return the cross product, this is costly. */
    Point Cross(const Point &Fp) const {
        return Point((fY*Fp.fZ-Fp.fY*fZ),(fZ*Fp.fX-Fp.fZ*fX),(fX*Fp.fY-Fp.fX*fY));};
    /*! Return cos of angle between the two vectors */
    inline double cos(const Point &P) { double value = this->Dot(P);
        return (value/(this->Magnitude()*P.Magnitude()));
    };

    /*! Return angle in radians between this and point 2. */
    inline double Angle(Point &point2) const { 
        double value = this->Dot(point2);
        return acos(value/(this->Magnitude()*point2.Magnitude()));
    };

    /*! return delta x between two points. */
    double DeltaX(const Point &point2) const 
	{return point2.fX - this->fX;};

    /*! return delta y between two points. */
    double DeltaY(const Point &point2) const 
	{return point2.fY - this->fY;};

    /*! return delta z between two points. */
    double DeltaZ(const Point &point2) const 
	{return point2.fZ - this->fZ;};

    /*!
     * Rotate 90 degrees around X, direction of positive
     * rotation is Y into Z.
     */
    inline Point &RotX90(bool Positive=true) 
	{
	    double temp = fY; 
	    if(Positive)
	    { 
		fY=-fZ;
		fZ=temp;
	    }
	    else
	    {
		// Negative rotation. 
		fY = fZ;
		fZ = -temp;
	    }
	    return *this;
	};
    /*!
     * Rotate 90 degrees around Y. Direction of postive rotation
     * is X into Z
     */
    inline Point &RotY90(bool Positive=true) 
	{
	    double temp = fX; 
	    if(Positive)
	    { 
		fX=-fZ;
		fZ=temp;
	    }
	    else
	    {
		// Negative rotation. 
		fX = fZ;
		fZ = -temp;
	    }
	    return *this;
	};

    inline void Clear(void) {fX = fY = fZ = 0.0;};

    /// In order to use this in a generic container  
    /// we must provide the following overloaded operators.

    /*! Operator, is this equal to point P? */
    bool operator == (const Point& P) const 
	{return (fX==P.fX && fY==P.fY && fZ == P.fZ); };

    /*! Operator, is this not equal to point P? */
    bool operator != (const Point& P) const 
	{return !(*this == P);};

    /*! Operator, is the magnitude of this less than the magnitude of P? */
    bool operator <  (const Point& P) const 
	{return (this->Magnitude()<P.Magnitude());};

    /*! Operator, is the magnitude of this greater than the magnitude of P? */
    bool operator >  (const Point& P) const 
	{return (this->Magnitude()>P.Magnitude());};

    inline Point operator*(const double &v) const
	{return Point(fX*v, fY*v, fZ*v);};
    inline Point operator /(const double &v) const
	{return Point(fX/v, fY/v, fZ/v);};


//    Point operator*(double v, const Point& a) 
//	{return Point(a.X()*v, a.Y()*v, a.Z()*v);};

    inline Point operator +(const Point& b) const
	{return Point(fX+b.X(), fY+b.Y(), fZ+b.Z());};

    inline Point operator -(const Point& b) const
	{return Point(fX-b.X(), fY-b.Y(), fZ-b.Z());};

    /*! R is the XY magnitude */
    inline double R(void) const {return sqrt(fX*fX + fY*fY);};

    /*! angle in XY plane Radians */
    double Theta(void) const {return atan2(fY,fX);};

    /*! Projection of Z onto XY plane, assume spherical */
    double Phi(void) const;

    /* Helper functions for dealing with multiple points. */
    /*! 
     * Given three points, return the distance of the third point 
     * as projected onto a line defined by the first 2. 
     */
    double DAL( const Point &P2, const Point &P3);

    /*! Given this point and another, return the range and bearing. 
     * Bearing will be returned in radians. 
     * This assumes that the point data is cartesian and in XY. 
     */
    double Range(const Point &P2, double *Bearing);

protected:
    double fX,fY,fZ;
};
inline Point operator*(double v, const Point&a) { 
    return Point(a.X()*v, a.Y()*v, a.Z()*v);};
#endif
