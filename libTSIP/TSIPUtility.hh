/**
 ******************************************************************
 *
 * Module Name : TSIPUtility.hh
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Utility functions that are helpers to the TSIP
 * decoder. 
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
 * References : NONE
 *
 *******************************************************************
 */
#ifndef __LASSENUTILITY_hh_
#define __LASSENUTILITY_hh_
#include "GPSTime.hh"
/// Utility functions
/*! Make a formatted string of Position options. */
void OutputPositionOptions(ostream& output, unsigned char n); 
/*! Make a formatted string of Velocity options. */
void OutputVelocityOptions(ostream& output, unsigned char n); 
/*! Make a formatted string of Timing options. */
void OutputTimingOptions(ostream& output, unsigned char n); 
/*! Make a formatted string of Velocity options. */
void OutputAuxOptions(ostream& output, unsigned char n); 

#endif
