
/********************************************************************
 *
 * Module Name : CLogger.cpp
 *
 * Author/Date : C.B. Lirakis / 08-Jun-11
 *
 * Description : Base logger class
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions :
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
#include <cstring>
#include <cmath>
#include <ctime>
#include <cstdarg>

// Local Includes.
#include "CLogger.hh"

CLogger* CLogger::fLogger;

/**
 ******************************************************************
 *
 * Function Name : CLogger constructor
 *
 * Description :
 *
 * Inputs : none
 *
 * Returns : constructed CLogger 
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
CLogger::CLogger (const char *filename, const char *pgmname, double Version)
{
    time_t now;
    char   msg[128];

    if (fLogger)
    {
	cerr << __FILE__ 
	     << " CLogger should only be called once."
	     << endl;
	return;
    }

    fLogger = this;

    fOnOff   = true;
    fVerbose = 0;

    time(&now);
    strftime (msg, sizeof(msg), "%F %T", gmtime(&now));

    fLogptr = new ofstream(filename);

    *fLogptr << "# ########################################## " << endl;
    *fLogptr << "# Program ";
    if (pgmname != NULL) *fLogptr << pgmname << endl;
    *fLogptr << "# Version: " << Version << endl;
    *fLogptr << "# Program Begins: " << msg << endl;
    *fLogptr << "# ########################################## " << endl;
}
/**
 ******************************************************************
 *
 * Function Name : CLogger Destructor
 *
 * Description :
 *
 * Inputs : none
 *
 * Returns :
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
CLogger::~CLogger ()
{
    time_t now;
    char   msg[128];
    time(&now);

    strftime (msg, sizeof(msg), "%F %T", gmtime(&now));
    *fLogptr << "# " << endl;
    *fLogptr << "# Program Ends: " << msg << endl;
    *fLogptr << "# ########################################## " << endl;
    fLogptr->close();
    delete fLogptr;
}
void CLogger::LogCommentTimestamp(const char *msg)
{
    time_t now;
    char   tmsg[128];
    time(&now);
    strftime (tmsg, sizeof(tmsg), "%F %T", gmtime(&now));

    *fLogptr << "# "
	     << tmsg << " : "
	     << msg
	     << endl;
}

void CLogger::LogComment(const char *msg)
{
    if (fOnOff) 
    {
	*fLogptr << "# "<< msg;
	fLogptr->flush();
    }
}
void CLogger::LogData(const char *msg) 
{
    if (fOnOff) 
    {
	*fLogptr << msg;
	fLogptr->flush();
    }
}
void CLogger::LogData(double f)
{
    if(fOnOff)
    {
	*fLogptr << f;
	fLogptr->flush();
    }
}
void CLogger::LogData(float f)
{
    if(fOnOff)
    {
	*fLogptr << f;
	fLogptr->flush();
    }
}
void CLogger::Log( const char *fmt, ...)
{
    va_list args;
    char buffer[BUFSIZ];

    va_start (args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    *fLogptr << buffer;
    fLogptr->flush();

}
void CLogger::LogTime(const char *fmt, ...)
{
    time_t  now;
    va_list args;
    char    buffer[BUFSIZ], tmsg[128];

    va_start (args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    time(&now);
    strftime (tmsg, sizeof(tmsg), "# %F %T", gmtime(&now));
    *fLogptr << tmsg << " " << buffer;
    fLogptr->flush();
}

void CLogger::SetVerbose(unsigned int i)
{
    Log("# Ser verbose level to: %d\n", i);
    fVerbose = i;
}
void CLogger::LogError( const char *File, int Line, char Level, 
			const char *text)
{
    time_t now;
    char   tmsg[128];
    time(&now);
    strftime (tmsg, sizeof(tmsg), "%y%m%d %T", gmtime(&now));

    *fLogptr << "# Error-" << Level
	     << " " << File
	     << "," << Line << ","
	     << tmsg << ","
	     << text
	     << endl;
}
