/**
 ******************************************************************
 *
 * Module Name : NMEA_helper.hh
 *
 * Author/Date : C.B. Lirakis / 02-Nov-25
 *
 * Description :  Helper functions for NMEA class
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
#ifndef __NMEA_HELPER_hh_
#define __NMEA_HELPER_hh_
#include <ctime>
/*!
 * p   - character string containing time in UTC with ms. 
 * ms  - return value in milliseconds
 * now - return value in struct tm
 * returns seconds since epoch in UTC. 
 */
time_t DecodeUTCFixTime(const char *p, float *ms, struct tm *now);
/*!
 *
 */
time_t DecodeDate(const char *p, struct tm *now);

/*!
 *
 */
double DecodeDegMin(const char *p);

#endif
