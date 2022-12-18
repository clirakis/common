/**
 ******************************************************************
 *
 * Module Name : Units.hh
 *
 * Author/Date : C.B. Lirakis / 16-Dec-14
 *
 * Description : Units for parsing, maintaining pointers and returning
 * strings. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 *
 * 
 *******************************************************************
 */
#ifndef __UNITS_h_
#define __UNITS_h_
#  include "DSA602_Types.hh"
const char *UnitString(UNITS p);
UNITS DecodeUnits(const char* p);

#endif
