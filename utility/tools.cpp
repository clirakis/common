/********************************************************************
 *
 * Module Name :  tools.cpp
 *
 * Author/Date : C.B. Lirakis / 23-Oct-98
 *
 * Description :
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <execinfo.h>

// Local Includes.
#include "tools.h"


/* Obtain a backtrace and print it to stdout. */
void print_trace (void)
{
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size    = backtrace (array, 10);
  strings = backtrace_symbols (array, size);

  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
     printf ("%s\n", strings[i]);

  free (strings);
}


/********************************************************************
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
 ********************************************************************/
void rtodm(double radians, int *degrees, double *minutes)
{
    double          x;

    // Convert to degrees.
    x = fabs(radians * 180.0/M_PI);
    // Normalize.
    while (x > 360.0)
    {
        x -= 360.0;
    }

    *degrees = (int) x;
    *minutes = 60.0 * (x-(double)(*degrees));
}

/********************************************************************
 *
 * Function Name : HeadingToAngle
 *
 * Description : convert heading to normal right hand rule angle
 *
 * Inputs : Heading in radians. 
 *
 * Returns : theta in radians. 
 *
 * Error Conditions :
 *
 ********************************************************************/
double HeadingToAngle(double Heading)
{
    double theta = M_PI/2.0 - Heading;

    while (theta>(2.0*M_PI)) theta-=(2.0*M_PI);
    while (theta<0.0) theta += (2.0*M_PI);
    return theta;
}
/**
 ******************************************************************
 *
 * Function Name : str_lat
 *
 * Description : convert a double to a formatted string
 *
 * Inputs : 
 *         latitude in radians
 *         string to fill
 *
 * Returns : filled string
 *
 * Error Conditions : Only if string is NULL
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
char *str_lat(double latitude, char *instr)
{

    if (instr != NULL)
    {
	double degrees = fabs(latitude * RadToDeg);
	double minutes =  60.0 * modf(degrees, &degrees);
	double fracs   = 10000.0 * modf(minutes, &minutes);
	if (latitude < 0.0)
	{
	    sprintf(instr, "S  %2.2d %2.2d.%4.4d",
		    (int)degrees, (int)minutes, (int)fracs);
	}
	else
	{
	    sprintf(instr, "N  %2.2d %2.2d.%4.4d",
		    (int)degrees, (int)minutes, (int)fracs);
	}
    }
    return instr;
}

/**
 ******************************************************************
 *
 * Function Name : str_lon
 *
 * Description : convert a double to a formatted string
 *
 * Inputs :
 *         latitude in radians
 *         string to fill
 *
 * Returns : formatted string
 *
 * Error Conditions :  Only if string is NULL
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
char *str_lon(double longitude, char *instr)
{
    if (instr != NULL)
    {
	double degrees = fabs(longitude * RadToDeg);
	double minutes =   60.0 * modf(degrees, &degrees);
	double fracs   = 10000.0 * modf(minutes, &minutes);
	if (longitude < 0.0)
	{
	    sprintf(instr, "W %3.3d %2.2d.%4.4d",
		    (int)degrees, (int)minutes, (int)fracs);
	}
	else
	{
	    sprintf(instr, "E %3.3d %2.2d.%4.4d",
		    (int)degrees, (int)minutes, (int)fracs);
	}
    }
    return instr;
}
