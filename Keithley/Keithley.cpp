/********************************************************************
 *
 * Module Name : Keithley.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic Keithley
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
#include <cstdio>
#include <cstring>
#include <cstdlib>



// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "Keithley.hh"

/**
 ******************************************************************
 *
 * Function Name : Keithley constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Keithley::Keithley (int gpib_address, bool verbose) : GPIB(gpib_address, verbose)
{
    SET_DEBUG_STACK;
    SetError();
    if(!RemoteEnable())
    {
	CLogger::GetThis()->Log("# ERROR %s %d, open and enable failed.\n",
				__FILE__,__LINE__);
	SetError(-1);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Keithley destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Keithley::~Keithley ()
{
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Keithley destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
 void Keithley::DecodePrefix(const char *v)
{
    SET_DEBUG_STACK;
    fReadStatus = *v;
    memset(fPrefix, 0, sizeof(fPrefix));
    memcpy(fPrefix, &v[1], 3);
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Keithley destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 26-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Keithley::SetUnitType(UnitType c) 
{
    SET_DEBUG_STACK;
    char MyString[128],*p;
    fUnitType = c;

    switch (fUnitType)
    {
    case CurrentSource:
	p = (char *) "220 Current source.";
	break;
    case VoltageSource:
	p = (char *)"230 Voltage source.";
	break;
    case Multimeter:
	p = (char *)"196 Multimeter.";
	break;
    case Multimeter2:
	p = (char *)"197 Multimeter.";
	break;
    }
    sprintf( MyString, "# Keithly Unit type = %s \n", p);
    CLogger::GetThis()->LogData(MyString);
    SET_DEBUG_STACK;
}
