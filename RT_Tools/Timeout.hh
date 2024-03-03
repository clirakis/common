/**
 ******************************************************************
 *
 * Module Name : Timeout.hh
 *
 * Author/Date : C.B. Lirakis / 29-Feb-24
 *
 * Description : A set of routines to setup a timeout. 
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
 *******************************************************************
 */
#ifndef __TIMEOUT_hh_
#define __TIMEOUT_hh_

bool Timeout(bool *RunFlag, const struct timespec &TimeToSleep);

#endif
