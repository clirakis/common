/********************************************************************
 *
 * Module Name : Keithley197.cpp
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
#include "Keithley197.hh"

/**
 ******************************************************************
 *
 * Function Name : Keithley197 constructor
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
Keithley197::Keithley197 (int gpib_address, bool verbose) : 
    Keithley(gpib_address, verbose)
{
    SET_DEBUG_STACK;
    SetUnitType(Multimeter2);
    Command("M35X",NULL,0);
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Keithley197 destructor
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
Keithley197::~Keithley197 ()
{
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Get
 *
 * Description : Perform a read and parse the data. 
 *
 * Inputs : none
 *
 * Returns : current value
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
double Keithley197::Get(void)
{
    SET_DEBUG_STACK;
    double value = 0.0;
    // Typical return 'OOHM+9.99999E+9
    // Read a value
    char buffer[32], *p;
    memset( buffer, 0, sizeof(buffer));
    if (Read(buffer, sizeof(buffer)))
    {
	p = buffer;
	DecodePrefix(p); p+=4;
	value = atof(p);
    }
    SET_DEBUG_STACK;
    return value;
}
/**
 ******************************************************************
 *
 * Function Name : GetStatus
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
bool Keithley197::GetStatus(void)
{
    SET_DEBUG_STACK;
    char  *p, tmp[4];
    memset(fKStatus, 0, sizeof(fKStatus));

    if (Command("U0X", fKStatus, sizeof(fKStatus)))
    {
	if (strncmp(fKStatus, "197", 3) == 0)
	{
	    p = fKStatus;
	    p+=3;
	    fFunction   = *p - '0'; p++;
	    fRange      = *p - '0'; p++;
	    fRelative   = *p - '0'; p++;
	    fEOI        = *p - '0'; p++;
	    fTrigger    = *p - '0'; p++;
	    fDataBuffer = *p - '0'; p++;

	    memset( tmp, 0, sizeof(tmp));
	    memcpy( tmp, p, 2); p+=2;
	    fSRQDataMask = atoi(tmp); 

	    memset( tmp, 0, sizeof(tmp));
	    memcpy( tmp, p, 2); p+=2;
	    fSRQErrorMask = atoi(tmp); 

	    fTerminator = *p;
	    SET_DEBUG_STACK;
	    return true;
	}
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : GetStatus
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : current value
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
void Keithley197::DumpStatus(void)
{
    const char *strFunction[]  = {"DCV","ACV","OHM","N/A","ACA","DC dB", 
				  "AC dB"};
    const char *strRangeV[]    = {"AUTO", "200mV", "2V", "20V", "200V", 
				  "1000V"};
    const char *strRangeOHM[]  = {"AUTO","200","2k","20k","200k","M"};
    const char *strRangeAMPS[] = {"200uA","2mA","20mA", "200mA", "2000mA", 
				  "10A"};
    const char *strTrigger[]   = {"Continuous on Talk", "One-shot on Talk",
				  "Continuous on Get", "One-shot on Get",
				  "Continuous on X", "One-shot on X"};
    SET_DEBUG_STACK;

    cout << "==================================================" << endl;
    cout << "Status: " << fKStatus << strFunction[fFunction]<< " ";
    switch (fFunction)
    {
    case 0:
    case 1:
    case 6:
    case 7:
	cout << strRangeV[fRange];
	break;
    case 2:
	cout << strRangeOHM[fRange];
	break;
    case 3:
	break;
    case 4:
    case 5:
	cout<< strRangeAMPS[fRange];
	break;
    }
    cout << " Relative: ";
    if (fRelative==0)
	cout << "Off";
    else
	cout << " On";
    cout << ", EOI: ";
    if (fEOI==0)
	cout << "Off";
    else
	cout << " On";

    cout << ", Trigger: " <<  strTrigger[fTrigger];
    cout << endl << "SRQ Data Mask: ";
    if (fSRQDataMask == 0) 
    {
	cout << "NONE";
    }
    else
    {
	if ((fSRQDataMask & 0x01)>0) cout << "Reading Overflow,";
	if ((fSRQDataMask & 0x08)>0) cout << "Reading Done,";
	if ((fSRQDataMask & 0x10)>0) cout << "Busy,";
    }
    cout << endl << "SRQ Error Mask: ";
    if (fSRQErrorMask == 0) 
    {
	cout << "NONE";
    }
    else
    {
	if ((fSRQErrorMask & 0x01)>0) cout << "IDDCO,";
	if ((fSRQErrorMask & 0x02)>0) cout << "IDDC,";
	if ((fSRQErrorMask & 0x04)>0) cout << "Not in Remote,";
    }
    cout << endl << "Terminator: " << fTerminator << endl;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : GetBuffer
 *
 * Description : Full buffer read
 *
 * Inputs : none
 *
 * Returns : current value
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
bool Keithley197::GetBuffer(double *data, size_t n)
{
    const char seps[] = ",\t\n";
    SET_DEBUG_STACK;
    // 100 maximum readings. 
    char Buffer[16*101], *token;
    int  i;
    unsigned int index;

    // Supress prefix and put in full buffer read. 
    if (Command("G1 B1 X",NULL,0))
    {
	memset( data, 0, n*sizeof(double));
	Read( Buffer, sizeof(Buffer));

	token = strtok( Buffer, seps);
	i = 0;
	while (token!=NULL)
	{
	    if (i%2 == 0)
	    {
		index = atoi(token);
	    }
	    else
	    {
		if (index<n)
		{
		    data[index] = atof(token);
		}
		else
		{
		    return true;
		}
	    }
	    token = strtok(NULL, seps);
	    i++;
	}
	return true;
    }
    Command("G0 B0 X", NULL, 0);
    SET_DEBUG_STACK;
    return false;
}

