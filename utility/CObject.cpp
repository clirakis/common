/********************************************************************
 *
 * Module Name : CObject.cpp
 *
 * Author/Date : C.B. Lirakis / 26-Dec-10
 *
 * Description : Base module for CBL library set.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.
#include <iostream>
#include <ostream>
using namespace std;
#include <string>
#include <cstring>
#include <cmath>
#include <ctime>

// Local Includes.
#include "CObject.hh"
#include "debug.h"

/**
******************************************************************
*
* Function Name : CObject module constructor
*
* Description : Allows the inheriting class to get some base 
* properties that all classes should have. 
*
* Inputs : none
*
* Returns : constructed CObject 
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
CObject::CObject ()
{
    SET_DEBUG_STACK;
    fBits  = 0;
    fDebug = ENONE;
    fError = DNONE;
    fELine = 0;
    fVersionHigh = 0;
    fVersionLow  = 0;
    fName  = NULL;
    SetName("CObject");
    SetUniqueID();
    fOutput = &cout;
}
/**
 ******************************************************************
 *
 * Function Name : CObject module constructor
 *
 * Description : Construct one CObject from another. 
 *
 * Inputs : none
 *
 * Returns : constructed CObject 
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
CObject::CObject (CObject &a)
{
    SET_DEBUG_STACK;
    fBits  = a.fBits;
    fDebug = a.fDebug;
    fError = a.fError;
    fELine = 0;
    SetName(a.fName);
}

/**
******************************************************************
*
* Function Name : CObject destructor
*
* Description : Nothing to do right now. 
*
* Inputs :
*
* Returns :
*
* Error Conditions :
* 
* Unit Tested on: 
*
* Unit Tested by: CBL
*
*
*******************************************************************
*/
CObject::~CObject ()
{
    SET_DEBUG_STACK;
    delete fName;
}

/**
******************************************************************
*
* Function Name : CObject Print
*
* Description : Print out diagnostic information about the object.
*
* Inputs :
*
* Returns :
*
* Error Conditions :
* 
* Unit Tested on: 
*
* Unit Tested by: CBL
*
*
*******************************************************************
*/
void CObject::Print(void)
{
    SET_DEBUG_STACK;
    cout << "CObject: ";
    if ( fName) 
        cout << fName << " ";
    else
        cout << "NO NAME ";
    cout << "Version: " << fVersionHigh << "." << fVersionLow << endl;
    cout << "Bits: 0x" << hex << fBits << " "
         << "Debug: " << dec << fDebug << " "
         << "Error: " << fError << " "
         << "Line:  " << fELine << " "
         << endl;
}
/**
******************************************************************
*
* Function Name : SetName
*
* Description : Permit the class to set the name. 
*
* Inputs : character name to assign to the class. 
*
* Returns : NONE
*
* Error Conditions : NONE
* 
* Unit Tested on: 
*
* Unit Tested by: CBL
*
*
*******************************************************************
*/
void CObject::SetName(const char *name)
{
    SET_DEBUG_STACK;
    delete fName;
    fName = strdup (name);
}

/**
******************************************************************
*
* Function Name : CObject operator =
*
* Description :
*
* Inputs :
*
* Returns :
*
* Error Conditions :
* 
* Unit Tested on: 
*
* Unit Tested by: CBL
*
*
*******************************************************************
*/
CObject& CObject::operator= (const CObject rhs)
{
    SET_DEBUG_STACK;
    if (this != &rhs)
    {
        fBits  = rhs.fBits;
        fDebug = rhs.fDebug;
        fError = rhs.fError;
        fELine = 0;
        SetName(rhs.fName);
    }
    return *this;
}
/**
******************************************************************
*
* Function Name : CObject SetUniqueID
*
* Description : 
*
* Inputs : Desired ID if 0, set to current UNIX time since epoch.
*
* Returns :
*
* Error Conditions :
* 
* Unit Tested on: 
*
* Unit Tested by: CBL
*
*
*******************************************************************
*/
void CObject::SetUniqueID(unsigned long i)
{
    SET_DEBUG_STACK;
    if (i==0)
    {
        time((time_t *)&fUniqueID);
    }
    else
    {
        fUniqueID = i;
    }
}
/**
 ******************************************************************
 *
 * Function Name : CObject SetUniqueID
 *
 * Description : 
 *
 * Inputs : Desired ID if 0, set to current UNIX time since epoch.
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* CObject::GetErrorString(int error)
{
    SET_DEBUG_STACK;
    char *str;
    if (error == 0)
    {
        str = (char *) "No Error";
    }
    else
    {
        str = (char *) "UNDEFINED";
    }   
    return str;
}
const char* CObject::GetErrorString()
{
    return GetErrorString(fError);
}
/**
 ******************************************************************
 *
 * Function Name : CObject SetUniqueID
 *
 * Description : 
 *
 * Inputs : Desired ID if 0, set to current UNIX time since epoch.
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void CObject::DebugInfo(const char* val, unsigned int level)
{
    SET_DEBUG_STACK;
    if (fDebug >= level)
    {
        cout << val << endl;
    }
}
bool CObject::CheckError(void) 
{
    SET_DEBUG_STACK;
    if(fError == CObject::ENONE) 
	return false; 
    return true;
}
int CObject::SetError( int code, int Line) 
{
    SET_DEBUG_STACK;
    fError=code;
    fELine=Line; 
    return code;
}
/**
 ******************************************************************
 *
 * Function Name : GetVersion
 *
 * Description : return a double that represents the current version
 * of the class. 
 *
 * Inputs : NONE
 *
 * Returns : double with version number
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double CObject::GetVersion(void)
{
    SET_DEBUG_STACK;
    char msg[32];
    double version;
    /* startup a message log. String messages go here. */
    sprintf(msg, "%d.%d", fVersionHigh, fVersionLow);
    version = atof( msg);
    return version;
}

/**
 ******************************************************************
 *
 * Function Name : 
 *
 * Description : 
 *
 * Inputs : 
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const CObject &n)
{
    output << "CObject: ";
    if ( n.fName) 
        output << n.fName << " ";
    else
        output << "NO NAME ";
    output << "Version: " << n.fVersionHigh << "." << n.fVersionLow << endl;
    output << "Bits: 0x"  << hex << n.fBits << " "
           << "Debug: " << dec << n.fDebug << " "
           << "Error: " << n.fError << " "
           << "Line:  " << n.fELine << " "
           << endl;
    return output;
}
void CObject::SetOutput(std::ostream *v)
{
    if(v)
    {
	fOutput=v;
    }
    else
    {
	fOutput = &std::cout;
    }
}
/**
 ******************************************************************
 *
 * Function Name : LogData
 *
 * Description : log a message to the file with an appropriate timestamp.
 *
 * Inputs : character message.
 *
 * Returns : none
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void CObject::LogData(const char *mymsg)
{
    time_t        now;
    char          msg[128];
    if (fOutput == NULL)
	return;
    time(&now);
    strftime (msg, sizeof(msg), "%F %T", gmtime(&now));
    *fOutput << "# "
	     << fName
	     << " " << msg
	     << " " << mymsg
	     << endl;
}
