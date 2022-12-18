/********************************************************************
 *
 * Module Name : StatusAndEvent.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : ERROR - Fix the duplication of module data here. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
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
#include <cstring>

// Local Includes.
#include "debug.h"
#include "StatusAndEvent.hh"
#include "DSA602.hh"
#include "DSA602_Utility.hh"

/**
 ******************************************************************
 *
 * Function Name : StatusAndEvent constructor
 *
 * Description : As a part of constructing this class the modules
 * are created as well as deteriming how the mainframe is configured. 
 *
 * Inputs : NONE
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
StatusAndEvent::StatusAndEvent (void) : CObject()
{
    SetName("StatusAndEvent");
    ClearError(__LINE__);

    for (int i=0;i<3;i++) fModule[i] = new Module(); // Empty modules
    fNModule    = 0;

    // Initialize the module data, Serial and position
    UID();  
    // Deterimine module types.
    Configuration(); 
    // Finally, fill in the channel configuration. 
}

/**
 ******************************************************************
 *
 * Function Name : StatusAndEvent destructor
 *
 * Description : clean up anything we have allocated, 
 *               fModule
 *
 * Inputs : NONE
 *
 * Returns : NONE
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
StatusAndEvent::~StatusAndEvent (void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    for (int i=0;i<3;i++) delete fModule[i];
}

/**
 ******************************************************************
 *
 * Function Name : UID
 *
 * Description : Get the serial numbers of the items in the mainframe. 
 *
 * Inputs : none
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 28-Nov-14
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool StatusAndEvent::UID(void)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    char    response[128];
    size_t  start, end;
    DSA602 *pDSA602 = DSA602::GetThis();

    /*
     * How many modules are there?
     * result from UID? query:
     * 'UID MAIN:"B050259",LEFT:"B032303",CENTER:"B021516",RIGHT:"B084569"'
     */
    if (pDSA602->Command("UID?", response, sizeof(response)))
    {
	/*
	 * success. 
	 * Parse out the serial numbers of the modules. 
	 * create a string to manipulate.
	 */
	string uid(response);
	string LR, Serial;
	start = end = 0;

	// See if there is mainframe ID information. 
	start = uid.find("MAIN:");

	/*
	 * I'm going to assume we found that without error checking. 
	 * increment to start of information.
	 * Open question, should I move this decode
	 * into the module class?
	 */
	start += sizeof("MAIN:");
	end = uid.find(',',start);
	fUnitSerial = uid.substr(start,end-start);
	start = end + 1; 

	fNModule = 0;
	do {
	    // Find the next module position
	    end = uid.find(':',start);
	    LR = uid.substr(start, end-start);
	    start = end + 1;
	    end   = uid.find(',',start);
	    Serial = uid.substr(start, end-start);
	    start = end + 1;
	    //cout << "L/R: " << LR << " SERIAL: " << Serial << endl;
	    if (LR.find("LEFT") == 0)
	    {
		fModule[fNModule]->Slot(kSLOT_LEFT);
	    }
	    else if (LR.find("CENTER") == 0)
	    {
		fModule[fNModule]->Slot(kSLOT_CENTER);
	    }
	    else if (LR.find("RIGHT") == 0)
	    {
		fModule[fNModule]->Slot(kSLOT_RIGHT);
	    }
	    fModule[fNModule]->Serial(Serial);
	    fNModule++;

	} while((fNModule<3) && (end<uid.size()));
    }
    else
    {
	SET_DEBUG_STACK;
	SetError(-1, __LINE__);
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Configuration
 *
 * Description : Get all the other information about the modules 
 *               using the CONFIG? command
 *
 *          "CONFIG LEFT:"11A33",CENTER:"11A33",RIGHT:"11A32"
 *
 * Inputs : none
 *
 * Returns : true on success
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 28-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool StatusAndEvent::Configuration(void)
{
    SET_DEBUG_STACK;
    char   response[128];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    // Get type..
    if (pDSA602->Command("CONFIG?", response, sizeof(response)))
    {
	/*
	 * success. 
	 * Parse out the type of the modules. 
	 * create a string to manipulate.
	 */
	string config(response);
	string LR, Type;
	size_t start, end;
	start = end = 0;

	// See if there is mainframe ID information. 
	start = config.find(' ') + 1;
	size_t n = 0;   // Which module are we addressing?
	do {
	    // Find the next module position
	    end   = config.find(':', start);
	    LR    = config.substr(start, end-start);
	    start = end+1;
	    end   = config.find(',',start);
	    Type  = config.substr(start, end-start);
	    start = end + 1;
	    // Store the type
	    fModule[n]->Type(Type);
	    n++;
	} while ( (n<3) && (end<config.size()));
	// Copy the above method, it is redundant but
        // ensures that the calling order is unimportant. 
    }
    else
    {
	SET_DEBUG_STACK;
	SetError(-1, __LINE__);
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : operator << 
 *
 * Description : specifically for StatusAndEvent
 *
 * Inputs : 
 *    output stream to insert data into. 
 *    n - StatusAndEvent information to parse into output. 
 *
 * Returns :
 *    formatted output stream
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
ostream& operator<<(ostream& output, const StatusAndEvent &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "StatusAndEvent: "  << endl
	   << "    Unit Serial:"  << n.fUnitSerial           << endl
           << "    Number Modules: " << (int) n.fNModule << endl;
//           << "    ========================================" << endl;
    for (int i=0;i<n.fNModule;i++)
    {
	output << *n.fModule[i];
    }

    SET_DEBUG_STACK;
    return output;
}
/**
 ******************************************************************
 *
 * Function Name : RQS
 *
 * Description : Query the status
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
bool StatusAndEvent::RQS(void)
{
    SET_DEBUG_STACK;
    char   response[16];
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);

    if (pDSA602->Command("RQS?", response, sizeof(response)))
    {
	//cout << "RQS? :   " << response << endl;
	if (strstr(response,"ON"))
	{
	    return true;
	}
	return false;
    }
    SetError(-1,__LINE__);
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : SetRQS
 *
 * Description : set the RQS ON/OFF
 *
 * Inputs : true for ON
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
bool StatusAndEvent::SetRQS(bool ON)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    const char *p;

    if(ON)
    {
	p ="RQS:ON";
    }
    else
    {
	p = "RQS:OFF";
    }

    return pDSA602->Command(p, NULL, 0);
}

unsigned char StatusAndEvent::GetStatus(void)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    cout <<  "GPIB iberr: " << pDSA602->GetError() << endl
	 <<  "GPIB ibsta: 0x" << hex << pDSA602->GetStation() << dec 
	 << pDSA602->str_Status() <<endl;
    return 0;
}

/**
 ******************************************************************
 *
 * Function Name : Event
 *
 * Description : Query Event codes
 *
 * Inputs : 
 *    NONE
 *
 * Returns : 
 *    EVENT 
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
bool StatusAndEvent::Event(void)
{
    SET_DEBUG_STACK;
    DSA602 *pDSA602 = DSA602::GetThis();
    ClearError(__LINE__);
    char response[128];

    if (pDSA602->Command("EVENT?", response, sizeof(response)))
    {
	cout << "EVENT response: " << response << endl;
	/* 
	 * This response is somewhat complex to parse. 
	 * 
	 * "RQS is ON...status byte pending"
	 * 
	 */
	return true;
    }
    return false;
}
#ifdef DEBUG_SAE
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
void StatusAndEvent::Test(void)
{
    SET_DEBUG_STACK;
    cout << "Status and Event: " << endl << *this;
    cout << RQS() << endl;
    GetStatus();
    Event();
    //fModule[0]->Test();
}
#endif
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
