/********************************************************************
 *\file
 * Module Name : Constants.h
 *
 * Author/Date : C.B. Lirakis / 27-Feb-22
 *
 * Description :
 *   A variety of physical constants that I use. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/

#ifndef __CONSTANTS_h_
#define __CONSTANTS_h_
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

    /* Conversion factors.*/
    /** Convert from map minutes or nautical miles to meters */
    static const double MetersPerMinute=1852.0;
    /** Oh boy, the old typesetters are here, number of points in 1 inch. */
    static const double PointsPerInch = 72.0;
    /** Conversion from Knots (nautical miles per hour) to Meters per second */
    static const double KnotsTOMetersPerSecond = MetersPerMinute/3600.0;
    static const double RadToDeg = 180.0/M_PI;
    static const double DegToRad = M_PI/180.0;
    static const double KPH2MPS  = 1000.0/3600.0;

    static const double Tesla2Gauss  = 10000.0; 
    static const double SpeedOfLight = 299792458.0; /* meters per second. */
    /** Convert from Inches to Centimeters */
    static const double CMperInch    = 2.54;
    static const double BoltzmanK    = 1.3806503e-23;
    static const double PlankH       = 6.62606876e-34;
    static const double PlankH_bar   = 1.054571596e-34;
    static const double Na           = 6.02214199e+23;   // avrogado's number
    static const double Coulomb      = 1.602176462e-19;  // electron charge
    static const double G2mps        = 9.8;
    static const double G_gravity    = 6.67430e-11; // N*m^2/kg^2
    static const double gal2mps2     = 0.01;        // 1 gal = 0.01m/s^2
    static const double e0           = 8.85e-12;    // Farads/m
    static const double u0           = 4.0*M_PI*1.0e-7; // N/m^2
#endif
