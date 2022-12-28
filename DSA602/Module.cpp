/********************************************************************
 *
 * Module Name : Module.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 30-Nov-14 CBL The 11A33 is a more complex object. Get the system working
 *               and finish this one later. 
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
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
#include "debug.h"
#include "Module.hh"
#include "GParse.hh"
// Mainframe commands supported
// Offset V/div offset for either horizontal or vertical
// Display +/- Ch1, +/-Ch2
// Poarity - Waveform menu
// Trigger selection - Trigger Menu
// A32 - HFLimit 20MHz or 100 MHz



char *ModuleNames[] = {(char*)"11A32 Two channel",
		       (char*)"11A33 Differential input"};

/**
 ******************************************************************
 *
 * Function Name : Module
 *
 * Description : Create a module from the given string
 *
 * Inputs : val - a character string that contains module data. 
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Module::Module(const char *val) : CObject()
{
    SET_DEBUG_STACK;
    SetName("Module");
    ClearError(__LINE__);

    fSlot       = kSLOT_NONE;
    fType       = kTYPE_NONE;
    fNChannel   = 0;
    fChannel[0] = NULL;
    fChannel[1] = NULL;
    if (val)
    {
	Decode(val);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Module destructor
 *
 * Description : Clean up any allocated variables. 
 *
 * Inputs :
 *
 * Returns :
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
Module::~Module()
{
    SET_DEBUG_STACK;
    delete fChannel[0];
    delete fChannel[1];
}
/**
 ******************************************************************
 *
 * Function Name : operator << for Module data
 *
 * Description : stuff all the formatted data about Module and Channel 
 *               into the specified stream. 
 *
 * Inputs :
 *   output - stream to stuff
 *   n      - Module class input
 *
 * Returns :
 *     formatted stream data associated with this module. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const Module &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "Module  ( Position: ";
    switch (n.fSlot)
    {
    case kSLOT_LEFT:
	output << "Left ";
	break;
    case kSLOT_CENTER:
	output << "Center ";
	break;
    case kSLOT_RIGHT:
	output << "Right ";
	break;
    case kSLOT_NONE:
	output << "NONE";
	break;
    }
    output << "Serial: " << n.fUnitSerial << " Type: ";
    switch (n.fType)
    {
    case kTYPE_NONE:
	output << "NONE";
	break;
    case kTYPE_11A32:
	output << "11A32";
	break;
    case kTYPE_11A33:
	output << "11A33";
	break;
    }

    if (n.fDifferential)
	output << " differential ";
    else
	output << " single ended ";
    output << ")" << endl;

    for (uint8_t i=0;i<n.fNChannel;i++)
    {
	output << *n.fChannel[i];
    }
    SET_DEBUG_STACK;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : DOES NOTHING
 *
 * Inputs :
 *
 * Returns :
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
void Module::Decode(const char *val)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
}
/**
 ******************************************************************
 *
 * Function Name : Type
 *
 * Description : Set the module type. Once we do this we know how many
 *               channels we have etc. We can create the channel 
 *               class and start to populate it. 
 *
 * Inputs :  string telling the module what type it is. 
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Module::Type(const string &in)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (in.find("11A32") != string::npos)
    {
	fType = kTYPE_11A32;
	fNChannel = 2; 
	fDifferential = false;
    } 
    else if (in.find("11A33") != string::npos)
    {
	fType = kTYPE_11A33;
	fNChannel = 1;
	fDifferential = true;
    }
    /*
     * Create the channel. Allocating the class will cause
     * the program to query the mainframe to tell us about the
     * channel. 
     */
    for(size_t i=0; i<fNChannel; i++)
    {
	fChannel[i] = new Channel(fSlot, i);
	if (fDifferential) fChannel[i]->SetDifferential();
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : ModuleString
 *
 * Description : return the name of the module. 
 *
 * Inputs : NONE
 *
 * Returns : module name.
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* Module::ModuleString(void)
{
    SET_DEBUG_STACK;
    return  ModuleNames[fType];
}
#ifdef DEBUG_MODULE
/**
 ******************************************************************
 *
 * Function Name : Test
 *
 * Description : Run a series of tests to debug the Channel class. 
 *
 * Inputs : NONE
 *
 * Returns : module name.
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
void Module::Test(void)
{
    SET_DEBUG_STACK;
    double dval = 1.0;
    // Start the tests on the various channel functions. 

    unsigned char ichn = 0;

    cout << "Amplifier offset: ";
    cout << "   Set: " << dval << endl;
    fChannel[ichn]->AmpOffset(dval);
    cout << "   Query" << fChannel[ichn]->AmpOffset(true) << endl;

    // 29-Jan-21 Set BW does not work in Left module. 
    cout << "Bandwidth: " ;
    cout << "   Set: " << fChannel[ichn]->Bandwidth(1.0e5) << endl;
    cout << "   Query" << fChannel[ichn]->Bandwidth(true) << endl;
    fChannel[0]->Bandwidth(2.0e7);
#if 0
    cout << "Bandwidth HI: " ;
    cout << "   Set: " << fChannel[ichn]->Bandwidth_HI(1.0e5) << endl;
    cout << "   Query" << fChannel[ichn]->Bandwidth_HI(true) << endl;
    fChannel[0]->Bandwidth(2.0e7);
#endif
    cout << "Coupling: " ;
    cout << " Set "  << fChannel[ichn]->Coupling(kCOUPLING_OFF) << endl;
    cout << " Query " << fChannel[ichn]->Coupling(true) << endl;

    cout << "Impedence: " ;
    cout << " Set "  << fChannel[ichn]->Impedence(kONE_MEG) << endl;
    cout << " Query " << fChannel[ichn]->Impedence(true) << endl;

    cout << "MNSCoupling: " ;
    cout << " Set "  << fChannel[ichn]->MNS_Coupling(kCOUPLING_AC) << endl;
    cout << " Query " << fChannel[ichn]->MNS_Coupling(true) << endl;

    cout << "MNS offset: ";
    cout << "   Set: " << fChannel[ichn]->MNS_Offset(0.5) << endl;
    cout << "   Query" << fChannel[ichn]->MNS_Offset(true) << endl;

    cout << "offset: ";
    cout << "   Set: " << fChannel[ichn]->Offset(0.5) << endl;
    cout << "   Query" << fChannel[ichn]->Offset(true) << endl;

    cout << "PLS Coupling: " ;
    cout << " Set "  << fChannel[ichn]->PLS_Coupling(kCOUPLING_AC) << endl;
    cout << " Query " << fChannel[ichn]->PLS_Coupling(true) << endl;


    cout << "PLS offset: ";
    cout << "   Set: " << fChannel[ichn]->PLS_Offset(0.5) << endl;
    cout << "   Query" << fChannel[ichn]->PLS_Offset(true) << endl;

    cout << "PROTECT: ";
    cout << "   Set: " << fChannel[ichn]->SetProtect(false) << endl;
    cout << "   Query" << fChannel[ichn]->Protect(true) << endl;

    cout << "Sensitivity: ";
    cout << "   Query" << fChannel[ichn]->Sensitivity(true) << endl;
    cout << "   Set: " << fChannel[ichn]->Sensitivity(1.0e-1) << endl;
    cout << "   Query" << fChannel[ichn]->Sensitivity(true) << endl;

    cout << "UNITS: " << fChannel[ichn]->Units(true);

    cout << "VCOffset: ";
    cout << "   Query" << fChannel[ichn]->VCOffset(true) << endl;
    cout << "   Set: " << fChannel[ichn]->VCOffset(0.0) << endl;
    cout << "   Query" << fChannel[ichn]->VCOffset(true) << endl;

}
#endif
