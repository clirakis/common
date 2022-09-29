/**
 ******************************************************************
 *
 * Module Name : Point.cpp
 *
 * Author/Date : C.B. Lirakis / 13-Feb-16
 * Original: 06-Sep-02
 *
 * Description : Impliment some rotation methods for the cartesian class. 
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
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>

/// Local Includes.
#include "debug.h"
#include "Point.hh"

/**
 ******************************************************************
 *
 * Function Name : Polar
 *
 * Description : Make an XY from a polar representaton
 *               equivalent to a cylindrical coordinate system. 
 *
 * Inputs : r   - radius
 *          phi - polar angle in radian
 *          Z   - distance along Z axis
 *
 * Returns : NONE - fills the values in place. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Point::Polar(double r, double phi, double Z)
{
    SET_DEBUG_STACK;    
    fX = r * cos(phi);
    fY = r * sin(phi);
    fZ = Z;
    SET_DEBUG_STACK;    
}

/**
 ******************************************************************
 *
 * Function Name : RotateX
 *
 * Description : Rotate about  x axis by angle specified. 
 * Changes state of this pointer, and returns value of this pointer. 
 *
 * Inputs : angle in radians to rotate around
 *
 * Returns : *this rotated
 *
 * Error Conditions : NONE
 *
 * Unit Tested  on:  
 * 
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
Point &Point::RotateX(const double angle)
{
    SET_DEBUG_STACK;    
    double Y;
    /**
     * Only Y and Z change.
     * Positive Rotation is Y to Z.
     */
    Y  = (fY * cos(angle) - fZ * sin(angle));
    fZ = (fY * sin(angle) + fZ * cos(angle));
    fY = Y;
    SET_DEBUG_STACK;    
    return *this;
}
/**
 ******************************************************************
 *
 * Function Name : RotateY
 *
 * Description : Rotate about y axis by angle specified. 
 * Changes state of this pointer, and returns value of this pointer. 
 *
 * Inputs : angle in radians to rotate around
 *
 * Returns : Modified this pointer
 *
 * Error Conditions : NONE
 *
 * Unit Tested  on:  
 * 
 * Unit Tested by:
 *
 *******************************************************************
 */
Point &Point::RotateY(const double angle)
{
    SET_DEBUG_STACK;
    double X;
    /**
     * Only X and Z change.
     * Positive Rotation is X to Z.
     */
    Point rc(*this);
    X  = (fX * cos(angle) - fZ * sin(angle));
    fZ = (fX * sin(angle) + fZ * cos(angle));
    fX = X;
    SET_DEBUG_STACK;    
    return *this;
}
/**
 ******************************************************************
 *
 * Function Name : RotateZ
 *
 * Description : Rotate about z axis by angle specified. 
 * Changes state of this pointer, and returns value of this pointer. 
 *
 * Inputs : angle in radians to rotate around
 *
 * Returns : Modified This pointer
 *
 * Error Conditions : NONE
 *
 * Unit Tested  on:  
 * 
 * Unit Tested by: CBL
 *
 *******************************************************************
 */
Point &Point::RotateZ(const double angle)
{
    SET_DEBUG_STACK;  
    double X;
    /**
     * Only X and Y change.
     * Positive Rotation is X to Y.
     */
    Point rc(*this);
    X  = (fX * cos(angle) - fY * sin(angle));
    fY = (fX * sin(angle) + fY * cos(angle));
    fX = X;
    SET_DEBUG_STACK;    
    return *this;
}
/**
 ******************************************************************
 *
 * Function Name : DAL
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
double Point::DAL(const Point &P2, const Point &P3)
{
    SET_DEBUG_STACK;  
    double rv = 0.0;
    /*
     * Point 1 is this.  
     * Create a vector from point 1 to point 2. 
     * Make another vector from point1 to point 3
     * project the length of the vector of 13 onto 12. 
     */
    Point P12(P2);
    P12 -= *this;
    Point P13(P3);
    P13 -= *this;

    rv  = P13.Dot(P12);
    rv /= P12.Magnitude();

    SET_DEBUG_STACK; 
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : Dot
 *
 * Description : Perform Dot product between this and P. 
 *
 * Inputs : P
 *
 * Returns : projection along P. 
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
double Point::Dot(const Point &P) const
{ 
    SET_DEBUG_STACK;
    return (fX*P.fX + fY*P.fY + fZ*P.fZ); 
    SET_DEBUG_STACK;
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
double Point::Phi(void) const
{
    SET_DEBUG_STACK;
    return acos(R()/Magnitude());
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Range 
 *
 * Description : Calculate range and bearing between two points. 
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
double Point::Range(const Point &P2, double *Bearing)
{
    SET_DEBUG_STACK;
    Point delta(P2);
    delta -= *this;

    if (Bearing!=NULL)
    {
	*Bearing = M_PI/2.0 - delta.Theta();
	if (*Bearing<0.0) *Bearing+=(2.0*M_PI);
    }
    SET_DEBUG_STACK;
    return delta.Magnitude();
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
ostream& operator<<(ostream& output, const Point &in)
{
    output << in.fX << " " << in.fY << " " << in.fZ;
    return output;
}
