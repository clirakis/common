/**
 ******************************************************************
 *
 * Module Name : filename.cpp
 *
 * Author/Date : C.B. Lirakis / 06-Jul-03
 *
 * Description : log file 
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
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <csignal>
#include <fstream>
#include <stdlib.h>

/// Local Includes.
#include "debug.h"
#include "filename.hh"

/**
 ******************************************************************
 *
 * Function Name :FileName constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
FileName::FileName(const char *snsr, const char *Suffix, 
                   const struct timespec &update, ostream *o) : CObject()
{
    char *temp, msg[64];
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    sensorName = NULL;
    Filespec   = NULL;
    Path       = NULL;
    suffix     = NULL;
    revision   = 0;
    TimeToChange   = NULL;
    LoggingEnabled = false;
    fOutput        = NULL;

    // CObject initializations
    SetName("FileName");
    SetOutput( o);

    if (snsr != NULL)
    {
        sensorName = strdup(snsr);
        Filespec = new char[FILELEN];
        if ((temp = getenv("DATAPATH")) != NULL)
        {
            Path = strdup(temp);
        }
        if (Suffix != NULL)
        {
            suffix = strdup(Suffix);
        }
	sprintf(msg, "Logfile name set to: %s with suffix: %s",
		Path, suffix);
	LogData(msg);
    }
    Update = new PreciseTime(update);
    if (update.tv_sec > 0)
    {
        TimeToChange = new PreciseTime();
        TimeToChange->Now();
        (*TimeToChange) += (*Update);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name :Filename destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 *******************************************************************
 */
FileName::~FileName()
{
    SET_DEBUG_STACK;

    if (fOutput != NULL)
    {
	fOutput->close();
	delete fOutput;
    }
    if (sensorName != NULL)
    {
        delete sensorName;
    }
    if (Filespec != NULL)
    {
        delete Filespec;
    }
    if (Path != NULL)
    {
        delete Path;
    }
    if (suffix != NULL)
    {
        delete suffix;
    }
    if (TimeToChange != NULL)
    {
        delete TimeToChange;
    }
    if (Update !=NULL)
    {
        delete Update;
    }
}
/**
 ******************************************************************
 *
 * Function Name : ChangeNames
 *
 * Description : returns true if a name change is in order!
 *
 * Inputs : none
 *
 * Returns : true or false only. 
 *
 * Error Conditions : None
 *
 *******************************************************************
 */
bool FileName::ChangeNames()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    double ans;

    if (TimeToChange != NULL)
    {
        if ((ans=TimeToChange->DiffDoubleNow()) >= 0)
        {
            revision = 0;
            return true;
        } 
    }
    return false; // No change is necessary. 
}

/**
 ******************************************************************
 *
 * Function Name : GetName
 *
 * Description : Return a fully constructed name based on the 
 *               characteristics defined byt the FileName class. 
 *               Based on current construction this will return something
 *               like:
 *               PATH/YYYYSensornameMMDD_RR.suf
 *
 *               where RR - is a revision number ranging from 00 to 99
 *                     suf - is a user supplied suffix. 
 *
 * Inputs : none
 *
 * Returns : constructed file name
 *
 * Error Conditions : In all cases a filespec is returned. The user
 * must check the status of LastError to determine if this routine
 * worked fully!
 *
 *******************************************************************
 */
const char *FileName::GetName()
{
    struct tm tmnow;
    time_t    now;
    char      temp[64];
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    // Get the current time. 
    time(&now);
    tmnow = *gmtime(&now);

    // Zero the array we are about to fill. 
    memset(Filespec, 0, FILELEN);

    // Fill the array temp with the current year. 
    sprintf(temp, "%4d", tmnow.tm_year+1900);

    // If a path has been specified, add it first to the 
    // filespec. Otherwise, the filespec remains filled
    // with nulls. 
    if (Path != NULL)
    {
        strcpy(Filespec, Path);
        strcat(Filespec,"/");
    }
    SET_DEBUG_STACK;

    // Regardless of how we got here, add the string for the year. 
    strcat(Filespec, temp);   

    // Check to see that the sensor name is not null and
    // that adding the sensor name does not overwrite the
    // size of the Filespec variable. 
    if ((sensorName!=NULL) && ((strlen(Filespec)+strlen(sensorName)) 
                               < FILELEN-1))
    {
        strcat(Filespec, sensorName);
    }
    else
    {
        SetError(-1, __LINE__);
    }
    SET_DEBUG_STACK;

    // Now check to see we have enough space to add on the
    // month and day qualifiers. 
    if ((strlen(Filespec)+5) < FILELEN)
    {
        sprintf(temp, "%3.3d", tmnow.tm_yday+1);
        strcat(Filespec, temp);
        sprintf(temp,"_%2.2d", revision);
        strcat(Filespec, temp);
        revision++;
    }
    else
    {
        SetError(-1, __LINE__);
    }
    SET_DEBUG_STACK;
    // Finally, check to see that we have enough space for 
    // a suffix. 
    if ((suffix != NULL) && ((strlen(Filespec)+strlen(suffix)+1) < FILELEN-1))
    {
        strcat( Filespec, ".");
        strcat( Filespec, suffix);
    }
    else
    {
         SetError(-1, __LINE__);
    }
    SET_DEBUG_STACK;
    return Filespec;
}
/**
******************************************************************
*
* Function Name : GetUniqueName
*
* Description : When called this routine will return a unique
*               filename associated with the FileName class. It
*               does this by calling the routine GetName
*
* Inputs : none
*
* Returns :
*
* Error Conditions :
*
*******************************************************************
*/
const char *FileName::GetUniqueName()
{
    SET_DEBUG_STACK;
    char msg[64];
    ClearError(__LINE__);

    // Do this until repeat equals false or the 
    // revision count is exhausted. 
    bool repeat = true;

    do {
        GetName(); // Create the file name as show in routine above.
        // Filename should come back as PATH/YYYYSensornameMMDD_RR.suf
        // 
        SET_DEBUG_STACK;
	if (fDebug>=1)
	{
	    sprintf(msg, "module filename: %s", Filespec);
	    LogData(msg);
	}

        // Open this file readonly to see if it exists. 
        ifstream in(Filespec,ifstream::in);
        if (in.fail())
        {
            // file could not be opened, Good to go!
            repeat = false;
        }
        else
        {
            // This constitutes a valid open. 
            if (in.good())
            {
                // file is open and is valid. 
                in.close();
                // The next call to getname will increment the
                // revision number so we don't need to here. 
            }
        }
    } while (repeat && revision<100);
    SET_DEBUG_STACK;
    return Filespec;
}
/**
******************************************************************
*
* Function Name : NewUpdateTime
*
* Description : Once we have passed the update time, we need to
* recalculate when the next one will occur. 
*
* Inputs : none
*
* Returns :
*
* Error Conditions :
*
*******************************************************************
*/
void FileName::NewUpdateTime()
{
    ClearError(__LINE__);
    TimeToChange->Now();
    (*TimeToChange) += (*Update);
}

/**
******************************************************************
*
* Function Name : LogData
*
* Description : A very generic/vanilla way of logging ASCII data.
* this also takes into account the request for logging name changes.
*
* Inputs : character data to be output to file. 
*
* Returns : true on success, false on failure. 
*
* Error Conditions : Fails with a -2 if the file can not be opened.
*
*******************************************************************
*/
bool FileName::LogData(const char *data)
{
    SET_DEBUG_STACK;
    char Fname[FILELEN];
    ClearError(__LINE__);

    if (fOutput != NULL)
    {
        // First check to see if we need to change names. 
        if (ChangeNames())
        {
            fOutput->flush();
            fOutput->close();
            memcpy(Fname, GetUniqueName(), FILELEN);
            NewUpdateTime();
            SET_DEBUG_STACK;

            fOutput->open(Fname,ios_base::out);
            if (fOutput->fail())
            {
                SetError(-2, __LINE__);
                delete fOutput;
                fOutput = NULL;
                return false;
            }
        }

        (*fOutput) << data << flush;
        return true;
    }
    return false;
}
/**
******************************************************************
*
* Function Name : EnableLogging
*
* Description : Part of the vanilla ASCII logging routines. 
* When set to false and the file is open, it will flush, close the file
* and set logging enabled flag to false; When true, it will get a name 
* for the file, open it and set the logging enabled flag to true. 
*
* Inputs : True - Enable, false - Disable. 
*
* Returns : true on success, false on failure. 
*
* Error Conditions : False on failure with an error code of -2
* indicates that the file specified could not be opened. 
*
*******************************************************************
*/
bool FileName::EnableLogging(const bool val) 
{
    SET_DEBUG_STACK;
    char Fname[FILELEN];
    ClearError(__LINE__);

    if ((fOutput != NULL) && !val)
    {
        // Disable logging, close out current system.
        fOutput->flush();
        fOutput->close();
        delete fOutput;
        fOutput = NULL;
    }
    else
    {
        // Logging is not currently enabled
        // Open the output logfile in text mode, without append. 

        memcpy(Fname, GetName(), FILELEN);

        SET_DEBUG_STACK;

        fOutput = new ofstream(Fname, ios_base::out);
        if (fOutput->fail())
        {
            SetError(-2, __LINE__);
            LogData("Error opening output datastream");
            return false;
        }
    }

    LoggingEnabled=val;
    return true;
}
