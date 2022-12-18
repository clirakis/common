/********************************************************************
 *
 * Module Name : Units.cpp
 *
 * Author/Date : C.B. Lirakis / 16-dec-14
 *
 * Description : Generic module
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
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

// Local Includes.
#include "Units.hh"

static const char* UnitTitle[9] = { "Amps", "Db", "Degrees", "Divs", "Hertz", 
				    "Ohms", "Seconds", "Volts", "Watts" };
const char* UnitString(UNITS p) 
{
    return UnitTitle[p];
}
UNITS DecodeUnits(const char* p)
{
    UNITS rv = kUNITSNONE;
    if (strncasecmp( p, "AMPS", 4) == 0)
    {
	rv = kAMPS;
    }
    else if (strncasecmp( p, "DB", 2) == 0)
    {
	rv = kDB;
    }
    else if (strncasecmp( p, "DEGR", 4) == 0)
    {
	rv = kDEGREES;
    }
    else if (strncasecmp( p, "DIVS", 4) == 0)
    {
	rv = kDIVS;
    }
    else if (strncasecmp( p, "HERT", 4) == 0)
    {
	rv = kHERTZ;
    }
    else if (strncasecmp( p, "OHMS", 4) == 0)
    {
	rv = kOHMS;
    }
    else if (strncasecmp( p, "SECO", 4) == 0)
    {
	rv = kSECONDS;
    }
    else if (strncasecmp( p, "VOLT", 4) == 0)
    {
	rv = kVOLT;
    }
    else if (strncasecmp( p, "WATT", 4) == 0)
    {
	rv = kWATT;
    }
    return rv;
} 


