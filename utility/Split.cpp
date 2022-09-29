/********************************************************************
 *
 * Module Name : Split.cpp
 *
 * Author/Date : C.B. Lirakis / 02-May-20
 *
 * Description : Split - Take a string and tokenize it. 
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
#include <cstring>
#include <cmath>

// Local Includes.
#include "debug.h"
#include "Split.hh"

/**
 ******************************************************************
 *
 * Function Name : Split constructor
 *
 * Description : Given an input string and a delimiter, make
 *               an array of tokenized elements. 
 *
 * Inputs : Names - the string that the user desires to be parsed.
 *          delim - the character delimiter in the string. 
 *
 * Returns : NONE
 *
 * Error Conditions : Names is NULL
 *                    delimiter is zero
 *                    memory allocation fails 
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Split::Split (const char *Names, const char delim)
{
    SET_DEBUG_STACK;

    if (Names == NULL) return;
    if (delim == 0) return;

    size_t i;
    char *Input = strdup(Names);     // Local copy
    char *ptr;                       // Pointer used in strchr operation
    char *begin = Input;             // Beginning of string

    /* It could be a single delimiter therefore make it a big buffer */
    size_t MaxChar = strlen(Names);
    char *tmp      = (char *) calloc(sizeof(char), MaxChar);

    // Parse into sub lists
    // 1) Count the number of tokens. 
    fNTokens = 0;
    for (i=0;i<strlen(Input);i++)
    {
	if (Input[i] == delim)
	{
	    fNTokens++;
	}
    }
    // Advance this one more time to go to the end of the string if necessary
    fNTokens++;

    // Allocate space for the tags. 
    fTokens = (char **) calloc(sizeof(char *), fNTokens);
    if (fTokens)
    {
	// Space allocation was successful. 
	i = 0;  // index on token pointer

	// Now fill the allocated space.
	// First instance of the delimiter. 
	ptr = strchr( begin, delim);
	while (ptr != NULL)
	{
	    // Copy over to tmp the string between the current pointer value
	    // and delimiter
	    memset( tmp, 0, MaxChar); // Is this too time consuming?
	    memcpy( tmp, begin, ptr-begin);

	    fTokens[i] = strdup(tmp);
	    i++;
	    begin = ptr+1;
	    ptr = strchr(begin, delim);
	}
	if (strlen(begin) > 0)
	{
	    // A litte more to copy
	    fTokens[i] = strdup(begin);
	}
	else
	{
	    fNTokens--; // There was nothing else. 
	}
    }
    free(Input);
    free(tmp);
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Split destructor
 *
 * Description : Clean up internal elements when done. 
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: NONE
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Split::~Split ()
{
    SET_DEBUG_STACK;
    size_t i;
    for (i=0;i<fNTokens;i++)
    {
	free(fTokens[i]);
	fTokens[i] = 0;
    }
    free (fTokens);
    fTokens = 0;
    fNTokens = 0;
    SET_DEBUG_STACK;
}


/**
 ******************************************************************
 *
 * Function Name : Split::Token
 *
 * Description :  Return the index to token from the tokenized array 
 *                internal to the class. 
 *
 * Inputs : Index - user specified index into the array of tokenized
 *          elements. 
 *
 * Returns : a const char string of the token at that index OR
 *           NULL if the index is out of bounds. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* Split::Token(size_t Index) const
{
    SET_DEBUG_STACK;
    if (Index < fNTokens)
    {
	return fTokens[Index];
    }
    return NULL;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Split::FindToken
 *
 * Description : Given a needle, return the index in the array of 
 *               tokens in the class that it matches. 
 *
 * Inputs : needle - search string
 *
 * Returns : index into array of tokens if it exists. 
 *           -1 if not found. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 02-May-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
int Split::FindToken(const char *needle) const
{
    SET_DEBUG_STACK;
    size_t i = 0;
    size_t n = strlen(needle);
    int    rc;

    while(i<fNTokens)
    {
	rc = strncmp(fTokens[i], needle, n);
 	if (rc==0)
 	{
 	    return i;
 	}
	i++;
    }
    SET_DEBUG_STACK;
    return -1;
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
ostream& operator<<(ostream& output, const Split &n)
{
    SET_DEBUG_STACK;
    size_t i;

    output << "Split: ";

    for (i=0;i<n.NTokens();i++)
    {
	output << i << ") " 
	       << n.Token(i) << ",";
    }
    output << endl;
    SET_DEBUG_STACK;
    return output;
}
