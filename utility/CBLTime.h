/**
 ******************************************************************
 *
 * Module Name : CBLTime.h
 *
 * Author/Date : C.B. Lirakis / 21-Mar-16
 *
 * Description : Generic timepec implementation.
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
#ifndef __CBLTime_h_
#define __CBLTime_h_
# ifdef __APPLE__
#  include <sys/time.h>
#  define CLOCK_GETTIME(x) \
    struct timeval tv; \
    struct timezone tz; \
    struct tm *tm; \
    gettimeofday (&tv, &tz); \
    tm = gmtime(&tv.tv_sec); \
    x.tv_sec  = tv.tv_sec; \
    x.tv_nsec = tv.tv_usec * 1000; 
# else
#  include <time.h> 
#  define CLOCK_GETTIME(x) clock_gettime(CLOCK_REALTIME,&x)
# endif
#endif
