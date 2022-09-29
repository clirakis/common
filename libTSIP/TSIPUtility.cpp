/********************************************************************
 *
 * Module Name : TSIPUtility.cpp
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Utility functions for TSIP decoding
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
#include "TSIPUtility.hh"
#include "GPSTime.hh"
#include "lassen.hh"


/**
 ******************************************************************
 *
 * Function Name : OutputPositionOptions
 *
 * Description : Output to provided stream a formatted 
 * string of the position options input
 *
 * Inputs : output stream
 *          n - the packed character code for position options. 
 *
 * Returns : NONE
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
void OutputPositionOptions(ostream& output, unsigned char n)
{
    output << "Position: ";
    if (n&kXYZ_ECEF)
	output << "XYZ_ECEF ";
    if (n&kLLA)
	output << "LLA ";
    if (n&kALT)
	output << "Alt:MSL ";
    else
	output << "Alt:HAE ";

    if (n&kALT_IN)
	output << "Alt IN:MSL ";
    else
	output << "Alt IN:HAE ";

    if (n&kPREC)
	output << "Prec:Double ";
    else
	output << "Prec:Single ";
    if (n&kSUPER)
	output << "Superpackets ";
	
}


/**
 ******************************************************************
 *
 * Function Name : OutputVelocityOptions
 *
 * Description : format a string into output stream for velocity options
 *
 * Inputs : output stream
 *          n bit packed data for setting options. 
 *
 * Returns : NONE
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
void OutputVelocityOptions(ostream& output, unsigned char n)
{
    output << "Velocity: ";
    if (n&kXYZ_ECEF)
	output << "XYZ ECEF ";
    if (n&kENU)
	output << "ENU ";
} 


/**
 ******************************************************************
 *
 * Function Name : OutputTimingOptions
 *
 * Description : format a string into output stream for timing options
 *
 * Inputs : output stream
 *          n bit packed data for setting options. 
 *
 *
 * Returns : NONE
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
void OutputTimingOptions(ostream& output, unsigned char n)
{
    output << "Timing: ";
    if (n&kUTC)
	output << "UTC ";
    else
	output << "GPS ";
}
/**
 ******************************************************************
 *
 * Function Name : OutputAuxOptions
 *
 * Description : format a string into output stream for AUX options
 *
 * Inputs : output stream
 *          n bit packed data for setting options. 
 *
 *
 * Returns : NONE
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
void OutputAuxOptions(ostream& output, unsigned char n)
{
    output << "AUX: ";
    if (n&kFilter)
	output << "Filter PRs ";
    else
	output << "Raw PRs ";

    output << "Signal Level:";
    if (n&kAMU)
	output << "AMU ";
    else
	output << "dBHz ";
}

