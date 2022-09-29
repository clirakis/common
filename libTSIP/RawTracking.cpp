/********************************************************************
 *
 * Module Name : module.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Stores the data assocated with the Tracking data,
 * message 0x5C. This is either raw or filtered depending on the 
 * setup. 
 * 
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
 * References : NONE
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>

// Local Includes.
#include "debug.h"
#include "RawTracking.hh"

/**
 ******************************************************************
 *
 * Function Name : RawTracking
 *
 * Description : NONE
 *
 * Inputs : Clear the internal data structure. 
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RawTracking::RawTracking() : DataTimeStamp()
{
    SET_DEBUG_STACK;
    Clear();
}
/**
 ******************************************************************
 *
 * Function Name : RawTracking
 *
 * Description : Constructor, using another RawTracking class
 *
 * Inputs : RawTracking structure
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RawTracking::RawTracking(const RawTracking &Fp)
{
    SET_DEBUG_STACK;
    fPRN                       = Fp.fPRN;
    fSignalLevel               = Fp.fSignalLevel;
    fElevation                 = Fp.fElevation;
    fAzimuth                   = Fp.fAzimuth;
    fChannelCode               = Fp.fChannelCode;
    fAcquisitionflag           = Fp.fAcquisitionflag;
    fEphemerisFlag             = Fp.fEphemerisFlag;
    fGPS_TimeofLastMeasurement = Fp.fGPS_TimeofLastMeasurement;
    fOldMeasurementFlag        = Fp.fOldMeasurementFlag;
    fMSecFlag                  = Fp.fMSecFlag;
    fBadDataFlag               = Fp.fBadDataFlag;
    fDataCollectionFlag        = Fp.fDataCollectionFlag;
    Stamp();     // Set the timestamp to now. 
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : Clear the data in the structure
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void RawTracking::Clear(void)
{
    SET_DEBUG_STACK;
    fChannelCode = fAcquisitionflag =  fEphemerisFlag = 0;
    fSignalLevel = fGPS_TimeofLastMeasurement =  fElevation =0.0;
    fAzimuth = 0.0;              // Radians
    fOldMeasurementFlag = false;
    fMSecFlag = fBadDataFlag = 0;
    fDataCollectionFlag = false;
    fValid = false;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Operator=
 *
 * Description : Overload on the operator = for Raw Tracking. Set
 * one class to another. 
 *
 * Inputs : RawTracking class as input
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
RawTracking &RawTracking::operator =(const RawTracking &Fp)
{
    SET_DEBUG_STACK;
    fPRN                       = Fp.fPRN;
    fSignalLevel               = Fp.fSignalLevel;
    fElevation                 = Fp.fElevation;
    fAzimuth                   = Fp.fAzimuth;
    fChannelCode               = Fp.fChannelCode;
    fAcquisitionflag           = Fp.fAcquisitionflag;
    fEphemerisFlag             = Fp.fEphemerisFlag;
    fGPS_TimeofLastMeasurement = Fp.fGPS_TimeofLastMeasurement;
    fOldMeasurementFlag        = Fp.fOldMeasurementFlag;
    fMSecFlag                  = Fp.fMSecFlag;
    fBadDataFlag               = Fp.fBadDataFlag;
    fDataCollectionFlag        = Fp.fDataCollectionFlag;
    Stamp();  // Set the timestamp to now!
    SET_DEBUG_STACK;
    return *this;
}

/**
 ******************************************************************
 *
 * Function Name : operator<<
 *
 * Description : ostream operator overload to format the RawTracking
 * data into a stream. 
 *
 * Inputs : RawTracking class. 
 *
 * Returns : ostream
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const RawTracking &n)
{
    SET_DEBUG_STACK;
    int channel, slot;
    channel = n.fChannelCode >> 3;
    slot    = n.fChannelCode&0x07;

    output << " PRN: " << (int) n.fPRN
	   << " Channel:"   << channel
	   << " Slot: "     << slot
	   << " Signal: "   << n.fSignalLevel
	   << " Az: "       << n.fAzimuth * 180.0/3.1415
	   << " El:"        << n.fElevation * 180.0/3.1415
	   << endl
	   << " Acq: "      << (int) n.fAcquisitionflag
	   << " Ephem: "    << (int) n.fEphemerisFlag
	   << " GPSTime: "  << n.fGPS_TimeofLastMeasurement
	   << " Old: "      << (int) n.fOldMeasurementFlag
	   << " MSecFlag: " << (int) n.fMSecFlag
	   << " Bad: "      << (int) n.fBadDataFlag
	   << " Collect: "  << (int) n.fDataCollectionFlag;
    SET_DEBUG_STACK;
    return output;
}
