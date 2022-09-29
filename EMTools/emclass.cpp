/**
 ******************************************************************
 *
 * Module Name : 
 *
 * Author/Date : C.B. Lirakis / 02-Feb-01
 *
 * Description :
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
 * RCS header info.
 * ----------------
 * $RCSfile: emclass.cpp,v $
 * $Author: clirakis $
 * $Date: 2003/11/24 15:23:38 $
 * $Locker: clirakis $
 * $Name:  $
 * $Revision: 1.2 $
 *
 * $Log: emclass.cpp,v $
 * Revision 1.2  2003/11/24 15:23:38  clirakis
 * Small changes
 *
 * Revision 1.1  2003/09/12 20:04:36  clirakis
 * Initial revision
 *
 *
 *
 * Copyright (C) BBNT Solutions LLC  2000
 *******************************************************************
 */

#ifndef lint
/// RCS Information
static char rcsid[]="$Header: /home/clirakis/common/EMTools/RCS/emclass.cpp,v 1.2 2003/11/24 15:23:38 clirakis Exp clirakis $";
#endif

// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <list>


/// Local Includes.
#include "emclass.h"

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
 *******************************************************************
 */
EMClass::EMClass(const double frequency, const double sigma)
{
    Frequency = frequency;
    Omega     = 2.0 * pi * Frequency;
    Sigma     = sigma;
}
  //***************************************************************** 
  // Function: EMClass Destructor
  //
  // Purpose: Clean up after ourselves
  // 
  // Inputs:
  //
  // Return values:
  // 
  // Modified   By  Reason
  // --------   --  ------
  // 
  //
  //***************************************************************** 
EMClass::~EMClass()
{
}
  //***************************************************************** 
  // Function: emgamma 
  //
  // Purpose: calculate the complex propagation constant
  // 
  // Inputs:
  //
  // Return values:
  // 
  // Modified   By  Reason
  // --------   --  ------
  // 
  //
  //***************************************************************** 
complex<double> EMClass::emgamma()
{
    double s = Sigma/(Omega * e0);
    double s2 = s*s;
    double a = Omega * sqrt ( 0.5 * mu0 * e0 *(sqrt(1+s2)-1.0));
    double b = Omega * sqrt ( 0.5 * mu0 * e0 *(sqrt(1+s2)+1.0));
    return complex<double>(a,b);

}
  //***************************************************************** 
  // Function: SkinDepth
  //
  // Purpose: calculate the skin depth where the signal drops
  // to 55dB of its original value.
  // 
  // Inputs:
  //
  // Return values:
  // 
  // Modified   By  Reason
  // --------   --  ------
  // 
  //
  //***************************************************************** 
double EMClass::SkinDepth()
{
    double delta = 0.0;
    double omega2 = Omega * Omega;
    double e02    = e0 * e0;
    double sigma2 = Sigma * Sigma;
    //
    double numerator = sqrt(2.0/(Omega * mu0 * Sigma));
    double denominator = sqrt(sqrt((omega2*e02/sigma2)+1.0)-Omega*e0/Sigma); 
    delta = numerator/denominator;
    /**
     * Using a value of 1Hz and conductivity of 4 Simens per Meter this
     * comes out to 251.646 which is in agreement with NUSC Technical
     * report 5807, page 3. 
     */

    return delta;
}
  //***************************************************************** 
  // Function: Speed
  //
  // Purpose: calculate the speed of light in a particular medium. 
  // Taken from page 219 of Schaum's outline on EM
  // 
  // Inputs:
  //
  // Return values:
  // 
  // Modified   By  Reason
  // --------   --  ------
  // 
  //
  //***************************************************************** 
double EMClass::Speed()
{
    double s     = Sigma/(Omega*e0);
    return 1.0/sqrt(0.5*mu0*e0*(1.0 + sqrt(1.0+s*s)));
}
  //***************************************************************** 
  // Function: 
  //
  // Purpose:
  // 
  // Inputs:
  //
  // Return values:
  // 
  // Modified   By  Reason
  // --------   --  ------
  // 
  //
  //***************************************************************** 
double EMClass::Lambda()
{
    complex<double> r = emgamma();
    return 2.0*pi/r.real();
}
/**
 ******************************************************************
 *
 * Function Name : ShortDipole
 *
 * Description : Field equations for an electrically short dipole 
 * antenna in a conducting medium. Kraichman, page 4-3
 *
 * The inputs must be thought of as a test point in space from the
 * center of the dipole under study, in cylindrical coordinates. 
 *
 * Inputs :  rho    - cylindral radius from dipole, meters
 *           phi    - angle around cylinder, radians
 *           z      - distance from center of dipole
 *                    along the z axis in meters
 *           length - length of dipole in meters. 
 *           current - in amps.
 *
 * Returns : just the magnetic field component. 
 *           Hphi - Magnetic field component around dipole in phi 
 *
 * Error Conditions :
 *
 *******************************************************************
 */
Cylindrical EMClass::ShortElectricDipole(const Cylindrical &tp, 
					 const double length,
					 const double Current,
					 const FieldType type, 
					 Cylindrical  *Phase)
{
  Cylindrical rc;
  complex<double> Ez, Erho, Hphi;
  complex<double> temp; // A working variable.
  complex<double> Gamma = emgamma();
  if (tp.Radius()<=0.0)
    {
      return rc;
    }
  /**
   * Orient dipole along z axis in cylindrical coordinates with the
   * center at z=0.
   */
  double A = length/2.0;
  double B = -A;
  // Eq 4.3
  double r1 = sqrt((tp.Z()-A)*(tp.Z()-A) + tp.Radius()*tp.Radius());
  // Eq 4.4
  double r2 = sqrt((tp.Z()-B)*(tp.Z()-B) + tp.Radius()*tp.Radius());
  //
  // Eq 4.5
  //
  double r13 = r1*r1*r1;
  double r23 = r2*r2*r2;

  // Electric field is measured in Volts per Meter
  Ez = Current/(4.0*pi*Sigma) *( Gamma*Gamma*( asinh((A-tp.Z())/tp.Radius()) - asinh((B-tp.Z())/tp.Radius()) ) +
				 ( (tp.Z()-A)/r13 - (tp.Z()-B)/r23 ) );
 
  Erho = Current * tp.Radius()/(4.0*pi*Sigma) * (1.0/r13 + 1.0/r23);

  // Magnetic Field is measured in amps per Meter
  // To get to Tesla from amps per Meter multipy by mu0.
  // To convert from Gauss to Tesla mutlitpy by 1.0E-4
  Hphi = Current/(4.0 * pi * tp.Radius()) * ( (tp.Z()-A)/r1 - (tp.Z()-B)/r2 );
	
  switch (type)
  {
    case FIELD_ELECTRIC:
      // Get  the  magnitude, must mutiply by complex conjugate. 
      temp = Ez * conj(Ez);

      rc.Z(sqrt(temp.real()));   // Should only be a real component at this point. 
      rc.PHI(0.0);

      temp  =  Erho * conj(Erho);
      rc.Radius(sqrt(temp.real()));
      if (Phase != NULL)
	{
	  // Calculate  the phase element.
	  Phase->Radius(atan2(Erho.imag(), Erho.real()));
	  Phase->PHI(0.0);
	  Phase->Z(atan2(Ez.real(),Ez.imag()));
	} 
      break;
    case  FIELD_MAGNETIC:
      temp = Hphi * conj(Hphi);
      rc.PHI(sqrt(temp.real()));

      rc.Z(0.0);
      rc.Radius(0.0);

      if (Phase != NULL)
	{ 
	  // Calculate  the phase element.
	  Phase->PHI(atan2(Hphi.imag(),Hphi.real()));
	  Phase->Radius(0.0);
	  Phase->Z(0.0);
	}
      break;
    }
  return rc;   
}

double EMClass::asinh(const double x)
{
  // From Abramowitz and Stegun, page 87.
  return log(x+sqrt((x*x)+1.0));
}      
/**
 ******************************************************************
 *
 * Function Name : MagneticDipole
 *
 * Description : Complilation of equations 3.10-3.11 in Kraichman. 
 * For these equations, this represents a magnetic dipole oriented along the z
 * axis in spherical coordinate system. 
 *
 * The inputs are in a spherical coordinate system with the dipole at the 
 * origin. 
 *
 * Inputs :  tp - Location of test point in spherical coordinates. (Meters and Radians)
 *          MagMoment - the magnetic moment of the dipole in ampere-meters^2
 *          FieldType - Field type for return value,  electric or  magnetic.
 *          Phase     - if this is non-null going in, it will return the phase angle
 *                      of each component of the returned field.
 *
 * NOTE this is in the approximation that the conductivity >> omega*e0
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
Spherical EMClass::MagneticDipole(const Spherical &tp, const double MagMoment, const FieldType  type, Spherical *Phase)
{
    Spherical rc;
    complex <double> EPhi, Hr, Htheta;

    /**
     * Radius - Radial distance from test point to source. (Meters)
     * Theta  - The angle in radians source polar axis and test point.
     * MagMoment - Magnetic moment of source in Amps/Meter^2
     */
   
    complex<double> Gamma = emgamma();
    complex<double> GammaR = Gamma * tp.Radius();
    complex<double> val, temp;  //  A  temporary  variable.
    complex<double> eye(0.0,-1.0); // Sqrt(-1)

    // Equation 3.4
    val  = (1.0 + 1.0/GammaR) * exp(-GammaR);
    EPhi  = eye*(Omega * mu0 * Gamma * MagMoment * sin(tp.THETA())/(4.0*pi*tp.Radius())) * val;

    // Equation 3.5
    Hr   = Gamma * MagMoment * cos(tp.THETA())/(2.0*pi*tp.Radius()*tp.Radius()) * val;

    //Equation 3.6
    Htheta = Gamma * Gamma * MagMoment * sin(tp.THETA())/(4.0*pi*tp.Radius()) *
      (1.0 + 1.0/GammaR + 1.0/(GammaR*GammaR))  * exp(-GammaR);


    switch (type)
      {
      case FIELD_ELECTRIC:
	// Get  the  magnitude, must mutiply by complex conjugate. 
	temp = EPhi * conj(EPhi);
	rc.PHI(sqrt(temp.real()));   // Should only be a real component at this point. 
	rc.Radius(0.0);
	rc.THETA(0.0);
    	if (Phase != NULL)
	  {
	    // Calculate  the phase element.
	    Phase->Radius(0.0);
	    Phase->PHI(atan2(EPhi.imag(),EPhi.real()));
	    Phase->THETA(0.0);
	  } 
	break;
      case  FIELD_MAGNETIC:
	temp = Hr * conj(Hr);
	rc.Radius(sqrt(temp.real()));

	rc.PHI(0.0);

	temp = Htheta * conj(Htheta);
	rc.THETA(sqrt(temp.real()));

	if (Phase != NULL)
	  { 
	    // Calculate  the phase element.
	    Phase->Radius(atan2(Hr.imag(),Hr.real()));
	    Phase->PHI(0.0);
	    Phase->THETA(atan2(Htheta.imag(),Htheta.real()));
	  }
	break;
      }
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : ElectricDipole
 *
 * Description : Complilation of equations 3.1 - 3.3 in Kraichman. 
 * For these equations, this represents a electric dipole oriented along the z
 * axis in spherical coordinate system. 
 *
 * The inputs are in a spherical coordinate system with the dipole at the 
 * origin. 
 *
 * Inputs :  tp - Location of test point in spherical coordinates. (Meters and Radians)
 *          DipoleMoment - the electric dipole moment of the dipole in ??
 *          FieldType - Field type for return value,  electric or  magnetic.
 *          Phase     - if this is non-null going in, it will return the phase angle
 *                      of each component of the returned field.
 *
 * NOTE this is in the approximation that the conductivity >> omega*e0
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
Spherical EMClass::ElectricDipole(const Spherical &tp, 
				  const double DipoleMoment, 
				  const FieldType  type, 
				  Spherical *Phase)
{
  Spherical rc;
  complex <double> Er,  Etheta,  Hphi;

  /**
   * Radius - Radial distance from test point to source. (Meters)
   * Theta  - The angle in radians source polar axis and test point.
   * MagMoment - Magnetic moment of source in Amps/Meter^2
   */
   
  complex<double> Gamma = emgamma();
  complex<double> GammaR = Gamma * tp.Radius();
  complex<double> val, temp;  //  A  temporary  variable.
  complex<double> eye(0.0,-1.0); // Sqrt(-1)

  // Equation 3.1
  val  = (1.0 + 1.0/GammaR) * exp(-GammaR);
  Er  = eye*(Omega * mu0/Gamma * DipoleMoment * cos(tp.THETA())/(2.0*pi*tp.Radius())*tp.Radius()) * val;

  // Equation 3.2
  Etheta = eye * (Omega * mu0 * DipoleMoment * sin(tp.THETA())/(4.0*pi*tp.Radius()) *
		  (1.0 + 1.0/GammaR + 1.0/(GammaR*GammaR)) *  exp(-GammaR));

  //Equation 3.3
  Hphi = Gamma * DipoleMoment * sin(tp.THETA())/(4.0*pi*tp.Radius()) * val;

  switch (type)
    {
    case FIELD_ELECTRIC:
      // Get  the  magnitude, must mutiply by complex conjugate. 
      temp = Er * conj(Er);
      rc.Radius(sqrt(temp.real()));   // Should only be a real component at this point. 
      rc.PHI(0.0);

      temp = Etheta * conj(Etheta);
      rc.THETA(sqrt(temp.real()));
      if (Phase != NULL)
	{
	  // Calculate  the phase element.
	  Phase->Radius(atan2(Er.imag(), Er.real()));
	  Phase->THETA(atan2(Etheta.imag(),Etheta.real()));
	  Phase->PHI(0.0);
	} 
      break;
    case  FIELD_MAGNETIC:

      temp = Hphi * conj(Hphi);
      rc.PHI(temp.real());

      rc.Radius(0.0);
      rc.THETA(0.0);

      if (Phase != NULL)
	{ 
	  // Calculate  the phase element.
	  Phase->PHI(atan2(Hphi.imag(),Hphi.real()));
	  Phase->Radius(0.0);
	  Phase->THETA(0.0);
	}
      break;
    }
  return rc;
}
