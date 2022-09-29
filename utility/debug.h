/**
 ******************************************************************
 *
 * Module Name : debug.h
 *
 * Author/Date : C.B. Lirakis / 05-Feb-03
 *
 * Description : 
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

#ifndef __DEBUG_h_
#define __DEBUG_h_

#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif
/* CBL 23-Jul-02 Debugging tools. */
extern int LastLine;
extern char *LastFile;
#define SET_DEBUG_STACK LastLine=__LINE__; LastFile=(char *)__FILE__;

#ifdef DEBUG
# define DBG(x) printf("%s: %s\n",__FUNCTION__,(x))
# define DBGD(x,y) printf("%s: %s %d\n",__FUNCTION__,(x),(y))
# define DBGX(x,y) printf("%s: %s 0x%X\n",__FUNCTION__,(x),(y))
#else
# define DBG(x)
# define DBGD(x,y)
# define DBGX(x,y)
#endif
#if 1
# define ERROR(x) printf("ERROR %s %d: %s\n", __FUNCTION__, __LINE__, x);
# define ERRORI(x,y) printf("ERROR %s %d: %s %d\n", __FUNCTION__, __LINE__, x, y);
# define INFO(x) printf("%s %d %s\n",  __FUNCTION__, __LINE__, x);
#else
# define ERROR(x) syslog(LOG_ERR, "ERROR %s %d: %s\n", __FUNCTION__, __LINE__, x);
# define ERRORI(x,y) syslog(LOG_ERR, "ERROR %s %d: %s %d\n", __FUNCTION__, __LINE__, x, y);
# define INFO(x) syslog(LOG_INFO, "%s %d %s\n",  __FUNCTION__, __LINE__, x);
#endif
#define DATE_TIME_TAG printf("File: %s built on %s %s\n", __FILE__, __DATE__, __TIME__);
#define LOG_MESSAGE(x) log_message((x),__FILE__,__LINE__)
void log_message(char *message, char *file, int line);

#ifdef __cplusplus
}
#endif


#endif

