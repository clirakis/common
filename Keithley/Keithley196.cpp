/********************************************************************
 *
 * Module Name : Keithley196.cpp
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
#include "Keithley196.hh"

/**
 ******************************************************************
 *
 * Function Name : Keithley196 constructor
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
Keithley196::Keithley196 (int gpib_address,bool verbose) : 
    Keithley(gpib_address, verbose)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    SetUnitType(Multimeter);
    if (!Command("REN F0X", NULL, 0))
    {
	// Command enable failed. 
	SetError(-1, __LINE__);
    }

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Keithley196 destructor
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
Keithley196::~Keithley196 ()
{
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GetData
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
double Keithley196::GetData(void)
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
 * Function Name : SetFunction
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : true on success
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
bool Keithley196::SetFunction(Function f)
{
    SET_DEBUG_STACK;
    char cstring[16];
    sprintf( cstring,"F%dX",f);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetZero
 *
 * Description : Set zero offset value
 *
 * Inputs : none
 *
 * Returns : true on success
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
bool Keithley196::SetZero(double value)
{
    SET_DEBUG_STACK;
    char cstring[32];
    sprintf( cstring,"Z2%fX",value);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetFilter
 *
 * Description : Set Filter value, 0-off
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetFilter(int value)
{
    SET_DEBUG_STACK;
    char cstring[16];
    if ((value>=0)&& (value<100))
    {
	sprintf( cstring,"P%dX",value);
	SET_DEBUG_STACK;
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetRate
 *
 * Description : Set average rate
 * 3.5 digits = 318uS
 * 4.5 digits = 2.59ms
 * 5.5 digits = line cycle
 * 6.5 digits = line cycle
 * line cycle = 20ms for 50Hz and 16.6ms for 60Hz.
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetRate(Rate value)
{
    SET_DEBUG_STACK;
    char cstring[16];
    if ((value>=0)&& (value<100))
    {
	sprintf( cstring,"S%dX",value);
	SET_DEBUG_STACK;
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetTrigger
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetTrigger(TriggerType value)
{
    SET_DEBUG_STACK;
    char cstring[16];
    sprintf( cstring,"T%dX",value);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetDataStore
 *
 * Description : interval in milliseconds from 1 to 999999msec
 *               size from 1-500
 * Setting interval to 0 is a one shot into the buffer.
 * Setting size to 0 is continuous storage mode. 
 * see page 3-22 for more information. 
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetDataStore(unsigned int interval, unsigned int size)
{
    SET_DEBUG_STACK;
    char cstring[32];
    if ((interval<999999) && (size>=0) && (size<500))
    {
	sprintf( cstring,"Q%d I%d X",interval,size);
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetFormat
 *
 * Description : Set Data format output
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetFormat(DataFormat value)
{
    SET_DEBUG_STACK;
    char cstring[16];
    sprintf( cstring,"G%dX",value);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
}
/**
 ******************************************************************
 *
 * Function Name : SetSRQ
 *
 * Description : Set SRQ mask
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetSRQ(SRQ value)
{
    SET_DEBUG_STACK;
    char cstring[16];
    sprintf( cstring,"M%dX",value);
    SET_DEBUG_STACK;
    return Command(cstring, NULL, 0);
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
 * Returns : machine status word.
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
const char* Keithley196::GetStatus(StatusCommand c)
{
    SET_DEBUG_STACK;
    char cstring[16];
    sprintf(cstring,"U%d X",c);
    memset(fStatus, 0, sizeof(fStatus));
    Command(cstring, fStatus, sizeof(fStatus));
    SET_DEBUG_STACK;
    return fStatus;
}
/**
 ******************************************************************
 *
 * Function Name : GetMachineStatus
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : machine status word.
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
struct t_MachineStatus& Keithley196::GetMachineStatus(void)
{
    SET_DEBUG_STACK;
    const char *s = GetStatus(MachineStatus);
    int  offset;
    char str[16];

    offset = 0;
    // Now decode it. 
    if (strncmp(s, "196", 3) == 0)
    {
	offset = 3;
    }
    fMStatus.AUTOCAL_Mult = (s[AUTOCAL_MULIPLEX+offset] == '0');
    fMStatus.ReadAD       = (s[READ_MODE+offset] == '0');

    memset(str,0,sizeof(str));
    memcpy(str, &s[FUNCTION_MODE+offset], 1);
    fMStatus.Function = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[DATA_FORMAT+offset], 1);
    fMStatus.DataFormat = atoi(str);

    fMStatus.SelfTest = (s[SELF_TEST+offset] == '0');

    memset(str,0,sizeof(str));
    memcpy(str, &s[BUS_HOLDOFF+offset], 1);
    fMStatus.EOI_Holdoff = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[SRQ_SETUP+offset], 2);
    fMStatus.SRQ = atoi(str);

    fMStatus.Exp_Filter = (s[FILTER_EPONENTIAL+offset] == '0');

    memset(str,0,sizeof(str));
    memcpy(str, &s[FILTER_SAMPLES+offset], 2);
    fMStatus.Filter_Average = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[STORE_RATE+offset], 6);
    fMStatus.DataStoreRate = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[RANGE_VALUE+offset], 1);
    fMStatus.Range = atoi(str);


    memset(str,0,sizeof(str));
    memcpy(str, &s[DISPLAY_RATE+offset], 1);
    fMStatus.DisplayRate = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[TRIGGER_TYPE+offset], 1);
    fMStatus.Trigger = atoi(str);

    memset(str,0,sizeof(str));
    memcpy(str, &s[DELAY_VALUE+offset], 5);
    fMStatus.Delay = atoi(str);

    fMStatus.Terminator = s[TERMINATOR+offset];

    memset(str,0,sizeof(str));
    memcpy(str, &s[ZERO_ENABLED+offset], 1);
    fMStatus.Zero = atoi(str);

    fMStatus.Calibration = (s[CAL_SW] == '0');

    SET_DEBUG_STACK;
    return fMStatus;
}
/**
 ******************************************************************
 *
 * Function Name : GetMachineStatus
 *
 * Description : 
 *
 * Inputs : none
 *
 * Returns : machine status word.
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Keithley196::DumpMachineStatus(void)
{
    const char *FunctionString[] = {"DCV","ACV","DCA","ACA","OHMS","ACV dB",
				    "ACA dB", "OFF_OHMS"};
    const char* DisplayMode[] = {"Value with prefix", "Value without prefix",
				 "Buffer with prefix and location",
				 "Buffer without prefix but with location",
				 "Buffer with prefix",
				 "Buffer without prefix"};
    const char* EOI_HOLD[] = {"EOI and HOLD-OFF", "NO EOI and HOLD-OFF",
			      "EOI and no HOLD-OFF", "NO EOI and NO HOLD-OFF"};
    const char* TRIGGER_STR[] = {"Continuous on talk", "One-shot on talk",
				 "Continuous on GET", "One-shot on GET",
				 "Continuous on X", "One-shot on X",
				 "Continuous on External Trigger", 
				 "One-shot on External Trigger"};
    const char* ZERO_STR[] = {"DISABLED","ENABLED","ENABLED with value"};

    SET_DEBUG_STACK;

    cout << "====================================================" << endl;
    cout << "Machine Status:  " << fStatus << endl
	 << "    AutoCal: ";
    if (fMStatus.AUTOCAL_Mult)
	cout << " Disabled";
    else
	cout << " Enabled";

    cout << endl << "    Read Mode: ";
    if (fMStatus.ReadAD)
	cout << " A/D converter";
    else
	cout << " Data buffer";

    cout << endl << "    Function mode: " << FunctionString[fMStatus.Function];
    cout << endl << "    Data format: " << DisplayMode[fMStatus.DataFormat];

    cout << endl << "    Self test: ";
    if (fMStatus.SelfTest)
	cout << " Disabled";
    else
	cout << " Enabled";

    cout << endl << "    EOI/Hold off: " << EOI_HOLD[fMStatus.EOI_Holdoff];

    cout << endl << "    SRQ: ";
    switch(fMStatus.SRQ)
    {
    case 0: 
	cout << "DISABLED";
	break;
    case 1:
	cout << "Reading overflow";
	break;
    case 2:
	cout << "Data store full";
	break;
    case 4:
	cout << "Data store half full";
	break;
    case 8:
	cout << "Reading done";
	break;
    case 16:
	cout << "Ready";
	break;
    case 32:
	cout << "ERROR";
    }
	
    cout << endl << "    Exponential filter: ";
    if (fMStatus.Exp_Filter)
	cout << " Disabled";
    else
	cout << " Enabled";

    cout << endl << "    FILTER P:     " << (int) fMStatus.Filter_Average;
    cout << endl << "    STORE RATE:   " << fMStatus.DataStoreRate << " ms";
    cout << endl << "    Range value:  " << (int) fMStatus.Range ;
    cout << endl << "    Display Rate: " << fMStatus.DisplayRate;
    cout << endl << "    Trigger:      " << TRIGGER_STR[fMStatus.Trigger];
    cout << endl << "    Delay:        " << fMStatus.Delay << " ms";
    cout << endl << "    Terminator:   " << fMStatus.Terminator;
    cout << endl << "    Zero:         " << ZERO_STR[fMStatus.Zero];
    cout << endl << "    Exponential filter: ";
    if (fMStatus.Calibration)
	cout << " Disabled";
    else
	cout << " Enabled";
    cout << endl
	 << "====================================================" << endl;

    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SetDelay
 *
 * Description : Set Delay value between trigger and sample in ms. 
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Keithley196::SetDelay(unsigned int val)
{
    SET_DEBUG_STACK;
    char cstring[16];
    if (val<60000)
    {
	sprintf( cstring,"W%dX",val);
	SET_DEBUG_STACK;
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : DisplayCharacter
 *
 * Description : Display a character on the display.
 *
 * Inputs : none
 *
 * Returns : true on success
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
bool Keithley196::DisplayCharacter(char *c)
{
    SET_DEBUG_STACK;
    char cstring[16];
    if (strlen(c) < 10)
    {
	sprintf( cstring,"D%sX",c);
	SET_DEBUG_STACK;
	return Command(cstring, NULL, 0);
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : GetBufferOfData
 *
 * Description : Go through the whole sequence of setup and
 * get a buffer of data where the size < 500
 *
 * Inputs : buffer
 *          size of buffer
 *          interval between samples. 
 *
 * Returns : true on success
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
bool Keithley196::GetBufferOfData(double *data, size_t s,unsigned int interval)
{
    SET_DEBUG_STACK;
    struct timespec sleeptime = {0L,0L};
    bool            rv = false;
    unsigned int    i,ix;
    char            buffer[32], *p, cstring[64];
    double          dt;

    if ((interval>999999) || (s>500))
    {
	return false;
    }

    // Determine time to hang out before looking at buffer. 
    dt = 1.0e-3 * (double)(interval * (s+1));
    ix = (int) floor(dt);
    dt -= (double)ix;
    sleeptime.tv_sec  = ix;
    sleeptime.tv_nsec = (int) floor(dt*1.0e9);
    //cout << "Delay: " << sleeptime.tv_sec << " " << sleeptime.tv_nsec << endl;

    // I assume that someone has previously specified what type of data
    // to acquire and set the range values. 
    // T4 - trigger on X
    // G0 - Send all buffer readings without prefix or buffer number
    // B1 - Get data from buffer


    sprintf( cstring,"G0 B1 T4 Q%d I%ld X",interval,s);
    //cout << cstring << endl;
    rv = Command(cstring, NULL, 0);

    nanosleep( &sleeptime, NULL);
    //GetMachineStatus();
    //DumpMachineStatus();

    for (i=0;i<s;i++)
    {
	memset( buffer, 0, sizeof(buffer));
	if (Read(buffer, sizeof(buffer)))
	{
	    //cout << buffer << endl;
	    p = buffer;
	    DecodePrefix(p); p+=4;
	    data[i] = atof(p);
	}
    }
    // Set back to normal immediate read. 
    Command("R0 T4 B0 G0 X", NULL, 0);
    SET_DEBUG_STACK;
    return rv;
}
