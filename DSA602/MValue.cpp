/********************************************************************
 *
 * Module Name : MValue.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic MValue
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
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <string>

// Local Includes.
#include "debug.h"
#include "MValue.hh"
#include "DSA602_Types.hh"
/**
 ******************************************************************
 *
 * Function Name : MValue Constructor
 *
 * Description : Decode a response into a MValue pair
 *
 * Inputs : the result of the MEAS? command
 *
 * Returns : none
 *
 * Error Conditions : Onluy if the Command doesn't equal the response
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
MValue::MValue(void)
{
    SET_DEBUG_STACK;
    fValue     = 0.0;
    fQualifier = kMNONE;
}

/**
 ******************************************************************
 *
 * Function Name : MValue Constructor
 *
 * Description : Decode a response into a MValue pair
 *
 * Inputs : the result of the MEAS? command
 *
 * Returns : none
 *
 * Error Conditions : Onluy if the Command doesn't equal the response
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
MValue::MValue(const char *Response)
{
    SET_DEBUG_STACK;
    fValue     = 0.0;
    fQualifier = kMNONE;
    Decode(Response);
}
/**
 ******************************************************************
 *
 * Function Name : MValue Constructor
 *
 * Description : Decode a response into a MValue pair
 *     an example of a response is: "-9.942585E-7,EQ"
 *     The command echo has been stripped out. 
 *
 * Inputs : the result of the MEAS? command
 *
 * Returns : none
 *
 * Error Conditions : Only if the Command doesn't equal the response
 * 
 * Unit Tested on: 26-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool MValue::Decode(const char *Response)
{
    SET_DEBUG_STACK;
    string resp(Response);
    size_t end;
    if (!Response)
    {
	return false;
    }
    /*
     * By the time we have gotten here we have the correct command
     * checked and the command is stripped it out by finding the 
     * first space delimter. 
     *
     *
     * We are now positioned to just before the value. 
     * Parse until the , delimeter that denotes the qualifier data. 
     */
    end   = resp.find_first_of(',');
    fValue = stof(resp.substr(0, end));
    string qual(resp.substr(end+1));
    if (qual.compare("EQ") == 0)
    {
	fQualifier = kEQUAL;
    }
    else if(qual.compare("LT") == 0)
    {
	fQualifier = kLESS_THAN;
    }
    else if(qual.compare("GT") == 0)
    {
	fQualifier =  kGREATER_THAN;
    }
    else if(qual.compare("UN") == 0)
    {
	fQualifier = kUNKNOWN;
    }
    else if(qual.compare("ER") == 0)
    {
	fQualifier = kERROR;
    }
// 	cout << "TO PARSE: " << fValue << " qual " << qual 
// 	     << " qual num: " << (int) fQualifier
// 	     << endl;

    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : MValue operator << 
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
ostream& operator<<(ostream& output, const MValue &n)
{
    output << "Value: " << n.fValue 
	   << " Qualifier: ";

    switch(n.fQualifier)
    {
    case kEQUAL:
	output << " Equal.";
	break;
    case kLESS_THAN:
	output << " Less Than.";
	break;
    case kGREATER_THAN:
	output << " Greater Than.";
	break;
    case kUNKNOWN:
	output << " Unknown.";
	break;
    case kERROR:
	output << " Error.";
	break;
    case kMNONE:
	output << " NOPARSE.";
	break;
    default:
	break;
    }
    
    return output;
}
