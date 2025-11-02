/********************************************************************
 *
 * Module Name : GSA.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic GSA
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

// Local Includes.
#include "debug.h"
#include "GSA.hh"

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
GSA::GSA(void)
{
    SET_DEBUG_STACK;
    fMode1 = fMode2 = 'N';
    memset( fSatellite, 0, sizeof(fSatellite));
    fPDOP=fHDOP=fVDOP = 0.0;
    SET_DEBUG_STACK;
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
bool GSA::Decode(const char *line)
{
    SET_DEBUG_STACK;
    int  i;
    char *p = (char *) line;

    p = strchr(p, ',')+1;
    fMode1 = *p;

    p = strchr(p, ',')+1;
    fMode2 = atoi(p);

    for (i=0;i<12;i++)
    {
	p = strchr(p, ',')+1;
	fSatellite[i] = atoi(p);
    } 
    p = strchr(p, ',')+1;
    fPDOP = atof(p);

    p = strchr(p, ',')+1;
    fHDOP = atof(p);

    p = strchr(p, ',')+1;
    fVDOP = atof(p);
    SET_DEBUG_STACK;
    return true;
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
ostream& operator<<(ostream& output, const GSA &n)
{
    output << "GSA data ================================= " << endl
	   << "  PDOP: " << n.fPDOP << endl
	   << "  HDOP: " << n.fHDOP << endl
	   << "  VDOP: " << n.fVDOP << endl
	   << " Mode1: " << n.fMode1 << endl
	   << " Mode2: " << n.fMode2 << endl;
    for (int i=0;i<12;i++)
    {
	output << (unsigned int) n.fSatellite[i] << " ";
    }
    output << endl
	   << " ========================================= " << endl;
    return output;
}
