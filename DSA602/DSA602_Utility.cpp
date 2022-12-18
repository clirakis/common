/********************************************************************
 *
 * Module Name : DSA602_Utility.cpp
 *
 * Author/Date : C.B. Lirakis / 29-Nov-14
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
#include "debug.h"
#include "DSA602_Utility.hh"

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
void RemoveQuotes(char *s)
{
    unsigned int i,j;
    char *p = strdup(s);
    int   n = strlen(s);

    memset(s, 0, n);

    j = 0;
    for (i=0;i<strlen(p);i++)
    {
	if (p[i]!='\"')
	{
	    s[j] = p[i];
	    j++;
	}
    }
    free(p);
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
void RemoveWhiteSpace(char *s)
{
    unsigned int i,j;
    char *p = strdup(s);
    int   n = strlen(s);

    memset(s, 0, n);

    j = 0;
    for (i=0;i<strlen(p);i++)
    {
	if (p[i]!=' ')
	{
	    s[j] = p[i];
	    j++;
	}
    }
    free(p);
}
const char* PT_String(PT_TYPES t)
{
    const char *p = "NONE";
    switch(t)
    {
    case kPT_Y:
	p = (const char *) "Y";
	break;
    case kPT_XY:
	p = (const char *) "XY";
	break;
    case kPT_ENV:
	p = (const char *) "ENV";
	break;
    case kPT_NONE:
	p = (const char *) "NONE";
	break;
    }
    return p;
}
char SlotChar(SLOT s)
{
    static const char rv[4] = {'L','C','R','N'};
    return rv[s];
}
/**
 ******************************************************************
 *
 * Function Name : Parse
 *
 * Description : parse a double value 
 *
 * Inputs : command and instruction
 *
 * Returns : double value
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
double Parse(const char *command, const char *instr)
{
    SET_DEBUG_STACK;
    double rc = 0.0;
    char *p = strcasestr( (char *)instr, (char *)command);
    if (p==NULL)
    {
	return rc;
    }
    p += strlen(command) + 1;
    rc = atof(p);
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : SParse
 *
 * Description : do a string parse given a command and instruction 
 *               return the result
 *
 * Inputs : 
 *   command - the command issued, performs a check
 *   instr   - input string to parse
 *
 * Returns :
 * string value from parse. 
 *    
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
const char* SParse(const char *command, const char *instr)
{
    SET_DEBUG_STACK;
    static char result[32];
    size_t len;
    char   *p = strcasestr( (char *)instr, (char *)command);

    memset(result, 0, sizeof(result));
    if (p!=NULL)
    {
	p += strlen(command) + 1;
	len = strlen(instr) - strlen(command) - 1;
	if (len > 0)
	    memcpy(result, p, len);
    }
    SET_DEBUG_STACK;
    return result;
}
