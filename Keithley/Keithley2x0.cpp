/********************************************************************
 *
 * Module Name : Keithley2x0.cpp
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
#include "Keithley2x0.hh"

/**
 ******************************************************************
 *
 * Function Name : Keithley2x0 constructor
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
Keithley2x0::Keithley2x0 (int gpib_address, char Type, bool verbose) : 
    Keithley(gpib_address, verbose)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if(!DefaultSetup())
    {
	CLogger::GetThis()->Log("# Keithley 2x0 - Error setting defaults.\n");
	SetError(-1, __LINE__);
    }
    switch (Type)
    {
    case 'V':
    case 'v':
	fUnitType=230;
	break;
    case 'I':
    case 'i':
	fUnitType=220;
	break;
    default:
	fUnitType = 0;
	CLogger::GetThis()->Log("# Keithley 2x0 - unit type wrong.\n");
	SetError(-1,__LINE__);
	break;
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Keithley2x0 destructor
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
Keithley2x0::~Keithley2x0 ()
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
double Keithley2x0::Get(void)
{
    const char seps[] = ",\t\n";
    SET_DEBUG_STACK;
    double value = 0.0;
    // Typical return NDCV+2.0000E-2,I+2.0000E-3,W+3.0000E-3,L+1.0000E+0
    // Read a value
    char buffer[128], *token;
    memset( buffer, 0, sizeof(buffer));
    if (Read(buffer, sizeof(buffer)))
    {
	fVoltage = 0.0;
	fCurrent = 0.0;
	fDwell   = 0.0;
	fDisplay = 0;
	fBuffer  = 0;

	token = strtok( buffer, seps);
	while (token!=NULL)
	{
	    if (strncmp( token, "NDCV", 4) == 0)
	    {
		token += 4;
		fVoltage = atof(token);
	    }
	    else if (strncmp( token, "NDCI", 4) == 0)
	    {
		token+=4;
		fCurrent = atof(token);
	    }
	    else if (*token == 'V')
	    {
		token +=1;
		fVoltage = atof(token);
	    }
	    else if (*token == 'I')
	    {
		token +=1;
		fCurrent = atof(token);
	    }
	    else if (*token == 'W')
	    {
		token +=1;
		fDwell = atof(token);
	    }
	    else if (*token == 'L')
	    {
		token +=1;
		fDisplay = (int) atof(token);
	    }
	    else if (*token == 'B')
	    {
		token +=1;
		fBuffer = (int) atof(token);
	    }

	    //cout << token << endl;
	    token = strtok( NULL, seps);
	}
#if 1
	cout << " V "<< fVoltage 
	     << " I " << fCurrent
	     << " D " << fDwell
	     << " L " << (int) fDisplay 
	     << " B " << (int) fBuffer
	     << endl;
#endif
    }
    SET_DEBUG_STACK;
    return value;
}
/**
 ******************************************************************
 *
 * Function Name : Set
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
bool Keithley2x0::Set(double Voltage, double Current, double Dwell, 
		   int Memory,  int Buffer)
{
    SET_DEBUG_STACK;
    char cstring[128];
    double  V,I;

    // Basic checks.
    if ((fUnitType != 220) && (fUnitType != 230))
    {
	return false;
    }

    if ((Buffer<0) || (Buffer>99) || (Memory<0) || (Memory>99))
    {
	SET_DEBUG_STACK;
	return false;
    }
    if ((Dwell<0.0) || (Dwell> 999.9))
    {
	SET_DEBUG_STACK;
	return false;
    }

    memset(cstring, 0, sizeof(cstring));

    switch(fUnitType)
    {
    case CurrentSource:
	// Voltage Range 1-105 in 1 volt steps. 
	if ((Voltage<1.0) || (Voltage>105))
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	V = floor(Voltage); // Integer part only. 

	// Current Range 0-101mA in 500fA steps
	if (fabs(Current) > 101.0e-3)
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	I = Current;
	sprintf(cstring, " B%d L%d V%d I%4.3f W%4.3f X", Buffer, Memory, 
		(int)V, I, Dwell);
	break;
    case VoltageSource:
	// Range 0-101V in 50uV steps
	if(fabs(Voltage)>101.0)
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	V = Voltage;
	// 
	// Current limit Ranges: 
        //     0 -   2 mA
        //     1 -  20 mA
        //     2 - 100 mA
	if ((Current<0.0) || (Current>2.0))
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	I = Current;
	sprintf(cstring, "B%d L%d V%4.3f I%d W%4.3f X", Buffer, Memory, 
		V, (int) I, Dwell);
	break;
    }
    //cout << "SET: " << cstring << endl;
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : Keithley2x0 function
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
bool Keithley2x0::DefaultSetup(void)
{
    // D0 - Display value (default on power up)
    // F1 - Operate
    // G0 - Transmit display memory location with prefixes.
    // K0 - Send EOI
    // P2 - Step
    // R0 - Auto range
    // T6 - Trigger, start on external
    // M1 - If the instrument receives an Illegal Device DependentCommand 
    //      (IDDC) or Illegal Device Dependent CommandOption (IDDCO), 
    //      or if the instrument is not in remotewhen programmed.

    SET_DEBUG_STACK;
    return Command("D0G0K0P2R0T6M1X", NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetVoltage
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
bool Keithley2x0::SetVoltage( double Value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    double x;

    memset(cstring, 0, sizeof(cstring));
    switch(fUnitType)
    {
    case CurrentSource:
	// Range 1-105 in 1 volt steps. 
	if ((Value<1.0) || (Value>105))
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	x = floor(Value);
	sprintf(cstring, "V%4.3eX", x);
	break;
    case VoltageSource:
	// Range 0-101V in 50uV steps
	if(fabs(Value)>101.0)
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	sprintf(cstring, "V%4.3eX", Value);
	break;
    case Multimeter:
	//sprintf(Command, "");
	break;
    }
    Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetCurrent
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
bool Keithley2x0::SetCurrent( double Value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    int ix = 0;
    memset(cstring, 0, sizeof(cstring));
    switch(fUnitType)
    {
    case CurrentSource:
	// Range 0-101mA in 500fA steps
	if (fabs(Value) > 101.0e-3)
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	sprintf(cstring, "I%4.3eX", Value);
	break;
    case VoltageSource:
	// Ranges: 
        //     0 -   2 mA
        //     1 -  20 mA
        //     2 - 100 mA
	if ((Value<0.0) || (Value>2.0))
	{
	    SET_DEBUG_STACK;
	    return false;
	}
	ix = (int) floor(Value);
	sprintf(cstring, "I%dX", ix);
	break;
    case Multimeter:
	//sprintf(Command, "");
	break;
    }
    cout << cstring << endl;
    Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetDwell
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
bool Keithley2x0::SetDwell( double Value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    // Range 3ms to 999.9s in 1ms steps
    // Value 0 is permissable to turn off the value
    if (((Value<0.003) || (Value> 999.9)) && (Value!=0.0))
    {
	SET_DEBUG_STACK;
	return false;
    }

    memset(cstring, 0, sizeof(cstring));
    sprintf(cstring, "W%4.3fX", Value);
    Command(cstring, NULL, 0);
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetBuffer
 *
 * Description : Set buffer pointer alone. 
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
bool Keithley2x0::SetBuffer (unsigned char Value)
{
    SET_DEBUG_STACK;
    char cstring[128];
    if (Value<=99)
    {
	memset(cstring, 0, sizeof(cstring));
	sprintf(cstring, "B%dX", Value);
	Command(cstring, NULL, 0);
	SET_DEBUG_STACK;
	return true;
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : Set Memory
 *
 * Description : Set memory location pointer. 
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
bool Keithley2x0::SetMemory (unsigned char Value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    if (Value<=99)
    {
	memset(cstring, 0, sizeof(cstring));
	sprintf(cstring, "L%dX", Value);
	Command(cstring, NULL, 0);
	SET_DEBUG_STACK;
	return true;
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetOutput
 *
 * Description : 4 bit general purpose output port, 
 *
 * Inputs : Value ranging from 0-15. The bit pattern is reflected
 *          on the output port. 
 *
 * Returns : true
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
bool Keithley2x0::SetOutput (unsigned char Value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    int iv = Value%16;
    memset(cstring, 0, sizeof(cstring));

    sprintf(cstring, "O%dX", iv);
    Command(cstring, NULL, 0);
    SET_DEBUG_STACK;

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetTrigger
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
bool Keithley2x0::SetTrigger(TriggerType t)
{
    SET_DEBUG_STACK;
    char cstring[32];
    
    if ((t>=StartOnTalk) && (t<=StopOnExternal))
    {
	memset(cstring, 0, sizeof(cstring));
	sprintf(cstring, "T%dX", t);
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : DumpBuffer
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
void Keithley2x0::DumpBuffer(void)
{
    SET_DEBUG_STACK;
    const char seps[] = ",\t\n";
    char *token;
    char Buffer[52*101];  // typically 52 characters, 100 locations max.
    SetTimeout(15);
    if (Command("G4X", NULL, 0))
    {
	SetBuffer(0);
	memset(Buffer, 0, sizeof(Buffer));
	Read( Buffer, sizeof(Buffer));
	token = strtok( Buffer, seps);

	cout << "Buffer contents:"
	     << endl;
	while (token!=NULL)
	{
	    if(token[0]=='N') cout << endl;
	    cout << token << " ";
	    token = strtok( NULL, seps);
	}
	cout << endl;
    }
    else
    {
	cout << "Failed to dump buffer contents." << endl;
    }
    SetTimeout(11);
    Command("G0X", NULL, 0);
    SET_DEBUG_STACK;
}
