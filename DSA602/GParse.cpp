/********************************************************************
 *
 * Module Name : GParse.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : Generic module
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *   23-Jan-21     CBL     Modified to remove root dependencies. 
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
#include <sstream>
#include <vector>
#include <locale>
#include <cmath>
#include <string.h>
#include <stdlib.h>

// Local Includes.
#include "GParse.hh"
#include "debug.h"

/**
 ******************************************************************
 *
 * Function Name : GParse constructor
 *
 * Description : Assume format of something title:value,title:value...
 * this should parse out the something then title:value pairs and 
 * put them in a list for subsequent search. 
 *
 * Inputs : instr - input string to find values in. 
 *
 * Returns : ...
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GParse::GParse (const char *instr)
{
    SET_DEBUG_STACK;
    const char sep = ' '; // Seperator for command
    fNarg    = 0;
    fLine    = NULL;
    fCommand = NULL;
    fTmp     = NULL;

    if (instr == NULL)
    {
	SET_DEBUG_STACK;
	return;
    }
    size_t N = strlen(instr);
    if (N<1)
    {
	SET_DEBUG_STACK;
	return;
    }

    // Make a local copy of instr
    fLine = new string(instr);

    // Get the command assocated with this string.
    size_t nCommandPos = fLine->find_first_of(sep);

    fCommand = new string(fLine->substr(0, nCommandPos));
   
    // Tokenize the remaining string. 
    stringstream ToLook(fLine->substr(nCommandPos+1));
    string       intermediate;

    while (getline(ToLook, intermediate, ','))
    {
	fTokenVals.push_back(intermediate);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : GParse destructor
 *
 * Description : clean up any allocated data
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GParse::~GParse (void)
{
    SET_DEBUG_STACK;
    delete fCommand;
    delete fTmp;
    delete fLine;
}

/**
 ******************************************************************
 *
 * Function Name : Command
 *
 * Description : return the command string value from the 
 *               input string
 *
 * Inputs : NONE
 *
 * Returns : string containing the command value. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* GParse::Command(void) const
{
    SET_DEBUG_STACK;
    return fCommand->c_str();
}
/**
 ******************************************************************
 *
 * Function Name : value
 *
 * Description : Find the value assocated with the string 'name' 
 *               This version is assocated with values that are
 *               strings. 
 *
 * Inputs : 
 *    name to find in the input string provide to parse.
 *    Warning, this is not re-entrant!
 *
 * Returns :
 *    string value assocated with this name, NULL if not found. 
 *    this is not a static value and the user should be expected to
 *    copy and store. 
 *
 * Error Conditions : 
 *     Name not found in parse. 
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* GParse::Value(const char *name)
{
    SET_DEBUG_STACK;
    const char *rv = NULL;
    size_t i = 0;
    size_t delim_pos;

    do 
    {
	if (fTokenVals[i].find(name) == 0)
	{
	    if (fTmp)
	    {
		delete fTmp;
		fTmp = NULL;
	    }
	    delim_pos=fTokenVals[i].find(':');
	    if (delim_pos > 0)
	    {
		delim_pos++;
		fTmp = new string(fTokenVals[i].substr(delim_pos));
		rv = fTmp->c_str();
	    }
	    break;
	}
	delim_pos = 0;
	i++;
    } while (i<fTokenVals.size());

    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : IndexedValue
 *
 * Description : Return string value at index. 
 *
 * Inputs : index into vector
 *
 * Returns : string value at that index
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 24-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* GParse::IndexedValue(size_t index) const
{
    SET_DEBUG_STACK;
    const char *rv = NULL;
    if (index<fTokenVals.size())
    {
	rv = fTokenVals[index].c_str();
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : ValueType
 *
 * Description : Determine what the value type is based on the 
 *               format of fTmp. Used directly after the Value
 *               call above
 *
 * Inputs : NONE
 *
 * Returns : 
 *     one of the types found in the enum eTYPES
 *     {TYPE_NONE=0,TYPE_STRING, TYPE_INT, TYPE_FLOAT}
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
int GParse::ValueType(void) const
{
    SET_DEBUG_STACK;
    int rv = TYPE_NONE;
    std::locale loc;
    if (fTmp)
    {
	if (fTmp->find("\"") != string::npos)
	{
	    rv = TYPE_STRING;
	}
	else if (fTmp->find(".") != string::npos)
	{
	    rv = TYPE_FLOAT;
	}
	else if (std::isdigit((string)*fTmp, loc))
	{
	    rv = TYPE_INT;
	}
	else 
	{
	    rv = TYPE_STRING;
	}
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : operator << 
 *
 * Description : insert into the provided stream the values of the
 *               class
 *
 * Inputs :
 *     output - stream to insert data into
 *     n - GParse class
 *
 * Returns : 
 *     modified output stream
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
ostream& operator<<(ostream& output, const GParse &n)
{
    //output << "GParse command: " << n.Command() << end;
    for (size_t i=0; i<n.NArg(); i++)
    {
	output << i << " " << n.IndexedValue(i) << endl;
    }
    return output;
}
