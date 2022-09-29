/**
 ******************************************************************
 *
 * Module Name : emclass.h
 *
 * Author/Date : C.B. Lirakis / 15-May-02
 *
 * Description : Equationsfrom "Handbook of Electromagnetic
 * Propagation in Conducting Media" by Martin Kraichman
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
 * $RCSfile: emclass.h,v $
 * $Author: clirakis $
 * $Date: 2003/11/24 15:23:48 $
 * $Locker: clirakis $
 * $Name:  $
 * $Revision: 1.2 $
 *
 * $Log: emclass.h,v $
 * Revision 1.2  2003/11/24 15:23:48  clirakis
 * Small changes
 *
 * Revision 1.1  2003/09/12 20:04:36  clirakis
 * Initial revision
 *
 *
 *
 * Copyright (C) BBNT Solutions LLC  2001
 *******************************************************************
 */
#ifndef __EMCLASS_h_
#define __EMCLASS_h_
#include "externdef.h"
# include <complex>
# include "emconst.h"      // Constants  for calculation. 
# include "Spherical.h"    // Spherical coordinate system. 
# include "Cylindrical.h"  // Cylindrical coordinate system.

enum DipoleType {DIPOLE_ELECTRIC, DIPOLE_MAGNETIC};
enum FieldType  {FIELD_ELECTRIC,  FIELD_MAGNETIC}; 

/// Module documentation here. 
class EMClass {
 private:
    // Private Data
    ///  frequency of calculation, Hertz
    double Frequency, 
		/// Angular frequency, radians/sec
		Omega;
	/// Conductivity in Simens per meter.
    double Sigma;

 protected:
    // Private Functions

 public:
    /**
     * Default Constructor
     *      Inputs are:
     *          frequency in HZ
     *          sigma - conductivity in simens per meter
     */
    EMClass(const double frequency=500.0, const double sigma=4.0);
    /// Default destructor
    ~EMClass();
    /// Access to complex propagation constant. 
	std::complex<double> emgamma();
    /**
     * Calculate the skindepth where the signal drops to 
     * 55dB of its original value
     */
    double SkinDepth();
    /** 
     * Return the speed of light in the medium described. 
     */
    double Speed();
    /**
     * Get the wavelength.
     */
    double Lambda();
    /**
     * Calculate fields due to short dipole. 
	 * input is the test point from the dipole.
	 * length is the  overall  length of the dipole. 
	 * and current is the current flowing  in  the dipole.
	 *
	 * If phase  is non-null then the phase for the various 
	 * components is  returned. 
     */
    Cylindrical ShortElectricDipole(const Cylindrical &tp, 
		       const double length=100.0, const double current=10.0,
			   const FieldType type=FIELD_ELECTRIC, Cylindrical  *phase=NULL);
    /**
     * A hyberbolic arc sin function. Utility only
     */
    double asinh(const double x);

	/**
	 * This function impliments all of the equations shown
	 * in Kraichman's book on  mpage 3-2. Specifically, 
	 * for a test point in spherical coordinates specified by tp (Meters and radians), and
	 * magnetic moment of MagMoment (ampere-meter^2) this will calculate the
	 * field values. This is specifically for an magnetic dipole.
	 * This returns  the magnitude of the field in spherical coordinates. 
	 * The type  (electric  or  magnetic) is  determined  by the FieldType argument.
	 * Put in non-null pointers to get return values for the phase information
	 * 
	 */
	Spherical MagneticDipole(const Spherical &tp, const double MagMoment, const FieldType type, Spherical *Phase );
	/**
	 * This function impliments all of the equations shown
	 * in Kraichman's book on  mpage 3-2. Specifically, 
	 * for a test point in spherical coordinates specified by tp (Meters and radians), and
	 * dipole moment of dipoleMoment (ampere-meter^2) this will calculate the
	 * field values. This is specifically for an magnetic dipole.
	 * This returns  the magnitude of the field in spherical coordinates. 
	 * The type  (electric  or  magnetic) is  determined  by the FieldType argument.
	 * Put in non-null pointers to get return values for the phase information
	 * 
	 */
	Spherical ElectricDipole(const Spherical &tp, const double dipoleMoment, const FieldType type, Spherical *Phase);

};
#endif
