/********************************************************************
 *
 * Module Name : SimpleCommand.cpp
 *
 * Author/Date : C.B. Lirakis / 26-Feb-24
 *
 * Description : Manage global Issuing of commands. 
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
#include "SimpleCommand.hh"

/**
 ******************************************************************
 *
 * Function Name : SimpleCommand constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
SimpleCommand::SimpleCommand (bool Source, const char *Name) : 
    SharedMem2("ChangeFileName", sizeof(struct CommandStructure), Source, 0)
{
    fSource = Source;
    memset( fMyName, 0, sizeof(fMyName));
    memset( (void*)&fMyCommand, 0, sizeof(struct CommandStructure));
    if (Name)
    {
	strncpy( fMyName, Name, sizeof(fMyName));
    }
    else
    {
	sprintf( fMyName, "NONE");
    }
    Command("ALL",kINVALID);
}

/**
 ******************************************************************
 *
 * Function Name : SimpleCommand destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
SimpleCommand::~SimpleCommand (void)
{
}


/**
 ******************************************************************
 *
 * Function Name : Command function
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
struct CommandStructure *SimpleCommand::Command(const char *Name, 
						uint32_t Command)
{
    SET_DEBUG_STACK;
    static const char *kALL = "ALL";
    struct CommandStructure MyCommand;
    
    memset( (void *) &fMyCommand, 0, sizeof(struct CommandStructure));
    if (fSource)
    {
	strncpy(fMyCommand.ProcessName, Name, kNAMELENGTH);
	fMyCommand.Command = Command;
	// Send a message to change the name
	PutData(&fMyCommand);
    }
    else
    {
	// receive a message to change the name
	GetData(&MyCommand);
	if ((memcmp(MyCommand.ProcessName, fMyName, strlen(fMyName)) == 0) ||
	    (memcmp(MyCommand.ProcessName, kALL, strlen(kALL)) == 0))
	{
	    // This command was meant for me.
	    fMyCommand = MyCommand;
	}
	else
	{
	    fMyCommand.Command = kINVALID;
	}
    }
    SET_DEBUG_STACK;
    return &fMyCommand;
}
/**
 ******************************************************************
 *
 * Function Name : Reset function
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Feb-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SimpleCommand::Reset(void)
{
    SET_DEBUG_STACK;
    static struct CommandStructure MyCommand;

    memset( (void*) &MyCommand, 0, sizeof(struct CommandStructure));
    if (!fSource)
    {
	strncpy(MyCommand.ProcessName, fMyName, kNAMELENGTH);
	MyCommand.Command = kINVALID;
    }
    
    // Send a message to change the name
    PutData(&MyCommand);
    SET_DEBUG_STACK;
}

