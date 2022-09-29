/**
 ******************************************************************
 *
 * Module Name : CLogger
 *
 * Author/Date : C.B. Lirakis / 08-Jun-11
 *
 * Description : Program logging control
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : none
 *
 *
 *******************************************************************
 */
#ifndef __CLOGGER_hh_
#define __CLOGGER_hh_
#include <fstream>
#include "debug.h"

class CLogger {
public:
    CLogger (const char *logfile, const char *pgmname=NULL, double Version=0.0);
    ~CLogger();
    /*! Return the pointer assocated with the log stream */
    ofstream*   LogPtr(void) {return fLogptr;};
    /*! Control logging */
    inline void Toggle(bool onoff=true) {fOnOff = onoff;};
    /*! Insert CR/LF */
    inline void CR(void)     {if(fOnOff) *fLogptr << endl;};
    /*! Insert space in stream. */
    inline void Space(void)  {if(fOnOff) *fLogptr << " ";};
    /*! Set the verbosity level for the logging. */
    void SetVerbose(unsigned int i);
    /*! Access the current verbosity level */
    inline unsigned int  GetVerbose(void) const {return fVerbose;};

    /*! Log a string, kinda raw. */
    void LogData(const char *msg);
    /*! Log a double, just puts the number in the stream, no endl. 
     * this does flush the stream. */ 
    void LogData(double );
    /*! Log a float, just puts the number in the stream, no endl. 
     * this does flush the stream. */ 
    void LogData(float );

    /*! Put a comment in the log stream, this starts with a #, 
     * no endl is provided. */
    void LogComment(const char *msg);
    /*!
     * A more complex log comment. Puts a timestamp on the comment
     * and terminates with endl. 
     */
    void LogCommentTimestamp(const char *msg);

    /*! Log data using a variable argument list. flush but no endl. */
    void Log( const char *fmt, ...);
    /*! Log a variable format with a timestamp, no endl. */
    void LogTime(const char *fmt, ...);
    /*! Full up log error. Includes endl. */
    void LogError( const char *File, int Line, char Level, const char *text);

    /*! Access the This pointer. */
    static CLogger* GetThis(void) {return fLogger;};

private:
    /*! Control ther verbosity level of output. */
    unsigned int       fVerbose;
    /*! Turn logging on/off, this isn't used everywhere. */
    bool      fOnOff;
    /*! Maintain a copy of the stream pointer */
    ofstream* fLogptr;

    /*! The static 'this' pointer. */
    static CLogger *fLogger;
};
#endif
