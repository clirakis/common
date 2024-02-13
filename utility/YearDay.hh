/**
 ******************************************************************
 *
 * Module Name : YearDay.hh
 *
 * Author/Date : C.B. Lirakis / 13-Feb-24
 *
 * Description :  input year, month, day and return day into year. 
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
#ifndef __YEARDAY_hh_
#define __YEARDAY_hh_
#   include <stdint.h>
/*!
 * Year - 2024 is 2024. 
 * Month - 0:11
 * Day   - 1:31
 */
uint32_t YearDay(uint32_t Year, uint8_t Month, uint8_t Day);

#endif
