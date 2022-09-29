/********************************************************************
 *
 * Module Name : LSM9DS1.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Jan-11
 *
 * Description : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 02-Apr-22  CBL    renamed from IMU to LSM9DS1
 *
 * Classification : Unclassified
 *
 * References :
 * This is specific for LSM9DS1 chip
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <cstring>

// Local Includes.
#include "LSM9DS1.hh"

/* 
 * This is used for the LSM9DS1 
 */
/* 
 * Linear acceleration, units g 
 * The values are 16 bit bi-polar, -32767:32768
 * {0.061, 0.122, 0.244, 0.732} mg/LSB 
 */
const double AccScale[4]={2.0/32768.0, 4.0/32768.0, 8.0/32768.0, 16.0/32768.0};

/* 
 * Magnetic measurement, units gauss
 * The values are 16 bit bi-polar, -32767:32768
 * The truncated values are: {0.00014, 0.00029, 0.00043, 0.00058}; mgauss/lsb
 */
//const double MagScale[4]={4.0/32768.0, 8.0/32768.0,12.0/32768.0,16.0/32768.0};
const double MagScale[4] = {0.00014, 0.00029, 0.00043, 0.00058};
/* 
 * Gyro, units dps 
 * The values are 16 bit bi-polar, -32767:32768
 * {8.75, 17.50, 70.0} mdps
 */
const double GyroScale[3]={245.0/32768.0, 500.0/32768.0, 2000.0/32768.0};
// Earth's magnetic field varies by location. Add or subtract 
// a declination to get a more accurate heading. Calculate 
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
// 41.3084 N 73.893W
// 2016-02-21  13.16° W  ± 0.36°  changing by  0.03° E per year

const double kDECLINATION = -13.16 * M_PI/180.0;
/**
 ******************************************************************
 *
 * Function Name : LSM9DS1 constructor
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
LSM9DS1::LSM9DS1(void) 
{
    /* Make sure these match the real time system. */
    fAccScale  = AccScale[0]; 
    fMagScale  = MagScale[0]; 
    fGyroScale = GyroScale[0];

    fScale     = true;

    fX0 = -0.30091;
    fY0 =  0.289897;

    /*
     * Raw Data from system
     * Axis    Mean      STD
     * GX      59.18     65.05
     * GY      5.915     63.61
     * GZ      -365.4    15.46
     * AX      -4837     62.65     -0.3g
     * AY      1945      150.2      0.1g
     * AZ     1.575e4    1196.0     0.96g
     * facing south
     * MX      -1147     48.17
     * MY      971.3     120.2
     * MZ     -2541       26.4
     */
    fLP_A = fLP_G = 0.9;  // Hz for a 20 Hz sample rate
    fLP_M = 0.9;
}

/**
 ******************************************************************
 *
 * Function Name : LSM9DS1 destructor
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
LSM9DS1::~LSM9DS1 (void)
{
}


/**
 ******************************************************************
 *
 * Function Name : LSM9DS1 function
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
void LSM9DS1::Decode(const char* line)
{
    /* Line
     * IMU G: 32, -26, -365, A: -4704, 1724, 15788, M: -3207, 1601, -1991 
     * G - Gyro
     * A - Acceleration
     * M - Magnetic
     */
    char *p;
    char *ptrG = strstr( (char *) line, "IMU G:");
    char *ptrA = strstr( (char *) line, "A:");
    char *ptrM = strstr( (char *) line, "M:");

    fG.Clear();
    fA.Clear();
    fM.Clear();

    if (ptrG != NULL)
    {
	ptrG += 6;
	p = strtok(  ptrG, ",");
	if (p == NULL) return;
	fG.X(atof(p));
	if (p == NULL) return;
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fG.Y(atof(p));
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fG.Z(atof(p));
    }

    if (ptrA != NULL)
    {
	ptrA += 2;
	p = strtok(  ptrA, ",");
	if (p == NULL) return;
	fA.X(atof(p));
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fA.Y(atof(p));
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fA.Z(atof(p));
    }

    if (ptrM != NULL)
    {
	ptrM += 2;
	p = strtok(  ptrM, ",");
	if (p == NULL) return;
	fM.X(atof(p));
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fM.Y(atof(p));
	p = strtok( NULL, ",");
	if (p == NULL) return;
	fM.Z(atof(p));
    }
    Filter();
    CalculateAttitude();
    if (fScale)
    {
	ApplyScale();
    }
}
void LSM9DS1::Filter(void)
{
    double a = (1.0-fLP_G);
    // New setting is a fraction of the new setting (a) with a fraction of the
    // old setting (fLP)
    fLG.X(a*fG.X()+fLG.X()*fLP_G);
    fLG.Y(a*fG.Y()+fLG.Y()*fLP_G);
    fLG.Z(a*fG.Z()+fLG.Z()*fLP_G);

    a = (1.0-fLP_A);
    fLA.X(a*fA.X()+fLA.X()*fLP_A);
    fLA.Y(a*fA.Y()+fLA.Y()*fLP_A);
    fLA.Z(a*fA.Z()+fLA.Z()*fLP_A);

    a = (1.0-fLP_M);
    fLM.X(a*fM.X()+fLM.X()*fLP_M);
    fLM.Y(a*fM.Y()+fLM.Y()*fLP_M);
    fLM.Z(a*fM.Z()+fLM.Z()*fLP_M);
}
void LSM9DS1::CalculateAttitude(void)
{
    fRoll    = atan2(fLA.Y(), fLA.Z()) * 180.0/M_PI;
    fPitch   = atan2( -fLA.X(), fLA.Z()) * 180.0/M_PI; 
    fHeading = CalculateHeading();
}
double LSM9DS1::CalculateHeading(void)
{
    double Heading = 0.0;

    // H = 180.0+TMath::ATan2((*Mx-X0),(*My-Y0))*TMath::RadToDeg() - 13.16;
    // if (H<0.0) H+=360.0;

    // Normalize
    double y = fLM.X();
    double x = fLM.Y();

    //Heading  = M_PI + atan2((x-fX0),(y-fY0));
    Heading  = M_PI + atan2(x,y);
    Heading += kDECLINATION;

    while (Heading<0.0) Heading += (2.0*M_PI);

    Heading *= 180.0/M_PI;
    return Heading;
}
void LSM9DS1::ApplyScale(void)
{
    fM *= fMagScale;
    fA *= fAccScale;
    fG *= fGyroScale;
}

/**
 ******************************************************************
 *
 * Function Name : LSM9DS1 function
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
ostream& operator<<(ostream& output, const LSM9DS1 &in)
{
    output << "Gyro: " << in.fG << ""
	   << " Acc: " << in.fA << " "
	   << " Mag: " << in.fM << endl
	   << "Heading: " << in.fHeading
	   << " Roll: "   << in.fRoll 
	   << " Pitch: "  << in.fPitch
	   << endl;
    return output;
}
