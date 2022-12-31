/********************************************************************
 *
 * Module Name : Input.cpp
 *
 * Author/Date : C.B. Lirakis / 04-Jan-15
 *
 * Input : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Inputs :
 *
 * Classification : Unclassified
 *
 * References :
 * DSA602 Programming reference manual page 294. This is part of the trace
 * command. 
 * DefTrace.cpp
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cstring>

// Local Includes.
#include "debug.h"
#include "Input.hh"
#include "GParse.hh"
#include "DSA602_Utility.hh"
#include "DSA602.hh"

const char* Input::FString[kFUNCTION_NONE+1] = {
    "ABS", "AVG", "CONVOLUTION", "CORRELATION", "DEJITTER", 
    "DELAY", "DIFF", "ENV", "EXP", "FFTIMAG", "FFTMAG", "FFTPHASE", 
    "FFTREAL", "FILTER", "IFFT", "INTG", "INTP", "LN", "LOG", 
    "PIADD", "PISUB", "PULSE", "SIGNUM", "SMOOTH", "SQRT", 
    "NONE"};


const char* Input::OString[kOPERATOR_NONE+1] = {
    "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "NONE"};

/**
 ******************************************************************
 *
 * Function Name : Input Constructor
 *
 * Input : A description can contain a list of inputs. The inputs
 * described here are an operation or not on a module with an operator. 
 * The permissable operators and functions are given at the bottom of 
 * this file. 
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
Input::Input(const char *val)
{
    SET_DEBUG_STACK;
    fFunction = kFUNCTION_NONE;
    fOperator = kOPERATOR_NONE;

    if (val!=NULL)
    {
	DecodeString(val);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Input destructor
 *
 * Input :
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
Input::~Input(void)
{
    SET_DEBUG_STACK;

}
/**
 ******************************************************************
 *
 * Function Name : Input operator <<
 *
 * Input :
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
ostream& operator<<(ostream& output, const Input &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "Input: " ;
#if 0
    if (n.fFunction != Input::kFUNCTION_NONE)
    {
	output << n.sFunction(false)<< "(" 
	       << n.fModule->ChannelString() << ")";
    }
    else
    {
	output << n.fModule->ChannelString();
    }
    if (n.fOperator != Input::kOPERATOR_NONE)
    {
	output << " " << n.sOperator();
    }
#endif
    output << endl << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}

/**
 ******************************************************************
 *
 * Function Name : DecodeString
 *
 * Input : Assume that pre-parsing has occured and the form of the input 
 * string looks something like ABS(L1) +
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
void Input::DecodeString(const char *val)
{
    SET_DEBUG_STACK;
    char *p = NULL;
    char *s, tmp[16], *local;
    int   n;

    /*
     * A Descriptor string looks something like this:
     *
     * TRACE1 DESCRIPTION:"R1 ON MAIN"
     * 
     */

    // Make a local copy of the data and then remove components as 
    // we parse them. 
    local = strdup(val);

    /*
     * Typical channel query data. page 272
     * 'TRACE1 DESCRIPTION:"R1 ON MAIN"
     */
    // Is there an operator of any type?
    for (n=0;n<4;n++)
    {
	if ((p=strchr(local, cop[n]))!= NULL)
	{
	    fOperator = (OPERATORS) n;
	    *p = ' ';
	    n = 4;
	}
    }

    // Determine if there is a function involved. 
    if ((p=strchr(local, '('))!= NULL)
    {
	n = p-local;
	memset(tmp, 0, sizeof(tmp));
	memcpy(tmp, local, n);
	DecodeFunction(tmp);

	// Extract module and channel
	s = strchr(local, ')');
	if (s!=NULL)
	{
	    n = s-p-1;
	    p++;
	    memset(tmp, 0, sizeof(tmp));
	    memcpy( tmp, p, n);
#if 0 // FIXME
	    fModule = new ModuleChannel(tmp);
#endif
	}
    }
    else
    {
#if 0 // FIXME
	fModule = new ModuleChannel(local);
#endif
    }
    free(local);
    SET_DEBUG_STACK;
}

const char* Input::sFunction(bool Y) const
{
    SET_DEBUG_STACK;
    //int i = 0;
    //if (Y) i = 1;

    SET_DEBUG_STACK;
    return FString[fFunction];
}
const char* Input::sOperator(void) const
{
    SET_DEBUG_STACK;
    return OString[fOperator];
}
void Input::DecodeFunction(const char* str)
{
    SET_DEBUG_STACK;
    // set fFunction to best match. 
    const char *p;
    int i = 0;
    fFunction = kFUNCTION_NONE;

    while ((p=FString[i])!= NULL)
    {
	if (strcasestr(str, p) != NULL)
	{
	    fFunction = (FUNCTIONS) i;
	    return;
	}
	i++;
    }
    SET_DEBUG_STACK;
}
void Input::DecodeOperator(const char* str)
{
    SET_DEBUG_STACK;
    // set fFunction to best match. 
    const char *p;
    int i = 0;
    fOperator = kOPERATOR_NONE;

    while ((p=OString[i])!= NULL)
    {
	if (strcasestr(str, p) != NULL)
	{
	    fOperator = (OPERATORS)i;
	    return;
	}
	i++;
    }
    SET_DEBUG_STACK;
}
const char* Input::sString(void) const
{
    SET_DEBUG_STACK;
    static char s[128];

#if 0 // FIXME
    memset(s, 0, sizeof(s));
    if (fFunction == kFUNCTION_NONE)
    {
	sprintf( s, "%s", fModule->ChannelString());
    }
    else
    {
	sprintf( s, "%s(%s)", sFunction(), fModule->ChannelString());
    }
    if (fOperator != kOPERATOR_NONE)
    {
	strcat( s, sOperator());
    }
#endif
    SET_DEBUG_STACK;
    return s;
}
