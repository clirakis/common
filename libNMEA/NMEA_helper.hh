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
#include <string> 
#include <cstdint>
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

/*!
 *
 */
std::string EncodeUTCTime(const time_t& now, uint32_t ms);
std::string EncodeUTCSeconds(const time_t& now, float ms);
std::string EncodeLatitude(const double &L);
std::string EncodeLongitude(const double &L);
uint8_t Checksum(const string &val);

#endif
