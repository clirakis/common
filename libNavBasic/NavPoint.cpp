/********************************************************************
 *
 * Module Name : NavPoint.cpp
 *
 * Author/Date : C.B. Lirakis / 26-Nov-15
 *
 * Description : Navigation point, builds on the base class Point.
 * This adds in features that are specific to navigation. 
 * All internal data is either specified as Radians, if storing a LL,
 * or meters, for storing XY data. 
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
#include <fstream>
#include <ostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <stdlib.h>

// Local Includes.
#include "debug.h"
#include "NavPoint.hh"

unsigned int NavPoint::fCount = 0;

/**
 ******************************************************************
 *
 * Function Name : NavPoint constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 20-Feb-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
NavPoint::NavPoint (double X, double Y, double Z, int Type,
		    double dt, const char *n, PointFormat fmt) : Point(X,Y,Z)
{
    SET_DEBUG_STACK;
    fFormat  = fmt;
    fComment = NULL;
    fType    = Type;
    if (n)
    {
	fName = new string(n);
    }
    else
    {
	fName = NULL;
    }
    fdt = dt;
    SetUniqueID(fCount);
    fCount++;
    SET_DEBUG_STACK;
}
NavPoint::NavPoint(const NavPoint& p) : Point(p.X(), p.Y(),p.Z())
{
    SET_DEBUG_STACK;
    fFormat  = p.Format();
    fComment = NULL;
    fType    = p.Type();
    fdt      = p.GetDt();
    const string *name = p.GetName();
    if (name) 
    {
	fName    = new string(*name);
    }
    else
    {
	fName = NULL;
    }

    if (p.CommentData())
    {
	fComment = new string(p.CommentData());
    }
    SetUniqueID(fCount);
    fCount++;
    SET_DEBUG_STACK;
}


NavPoint::NavPoint(const char *s) : Point()
{
    SET_DEBUG_STACK;
    fComment = NULL;
    fName    = NULL;
    SetData(s);
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : SetName
 *
 * Description : Set the name of the NavPoint
 *
 * Inputs : n - name to use. 
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
void NavPoint::SetName(const char *n)
{
    SET_DEBUG_STACK;
    if (fName==NULL)
    {
	fName = new string(n);
    }
    else
    {
	delete fName;
	fName = new string(n);
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : SetData
 *
 * Description : Typically called when loading a file. Given
 * that, this method parses a formatted string, preferabley in the
 * form of the overloaded operatator <<, and then fills the appropriate 
 * fields
 *
 * Inputs : n - line to parse out
 *
 * Returns : NONE
 *
 * Error Conditions :
 * 
 * Unit Tested on: 20-Feb-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NavPoint::SetData(const char *n)
{
    SET_DEBUG_STACK;
    std::string      input(n);
    std::string::size_type prev_pos=0, pos=0;
    double           XX;

    fType   = NavPoint::kPOINT_NONE;
    fName   = NULL;
    Zero();
    fdt = 0.0;

    XX = 0.0;
    // determine type
    while((pos = input.find(NAV_SEPARATOR, pos)) != std::string::npos)
    {
	std::string substring( input.substr(prev_pos, pos-prev_pos));
	if (substring.compare(0,3,"WPT")==0)
	{
	    fType = NavPoint::kWAYPOINT;
	}
	else if (substring.compare(0,3,"EVT")==0)
	{
	    fType = NavPoint::kEVENT;
	}
	else if (substring.compare(0,4,"LON=")==0)
	{
	    XX = atof(substring.substr(4,substring.length()).c_str());
	    // Convert to radians. 
	    XX *= DegToRad;
	    X(XX);
	}
	else if (substring.compare(0,4,"LAT=")==0)
	{
	    XX = atof(substring.substr(4,substring.length()).c_str());
	    // Convert to radians. 
	    XX *= DegToRad;
	    Y(XX);
	}
	else if (substring.compare(0,2,"X=")==0)
	{
	    XX = atof(substring.substr(2,substring.length()).c_str());
	    X(XX);
	}
	else if (substring.compare(0,2,"Y=")==0)
	{
	    XX = atof(substring.substr(2,substring.length()).c_str());
	    Y(XX);
	}
	else if (substring.compare(0,2,"Z=") == 0)
	{
	    XX = atof(substring.substr(2,substring.length()).c_str());
	    Z(XX);
	}
	else if (substring.compare(0,2,"DT=") == 0)
	{
	    fdt = atof(substring.substr(2,substring.length()).c_str());
	}
	else if (substring.compare(0,6,"TITLE=")==0)
	{
	    if (fName!=NULL)
	    {
		delete fName;
	    }
	    fName = new string(substring.substr(6,substring.length()));
	}
	else if (substring.compare(0,8,"COMMENT=")==0)
	{
	    if (fComment!=NULL)
	    {
		delete fComment;
	    }
	    fComment = new string(substring.substr(8,substring.length()));
	}

	prev_pos = ++pos;
    }
    SetUniqueID(fCount);
    fCount++;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : NavPoint destructor
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
NavPoint::~NavPoint()
{
    SET_DEBUG_STACK;
    delete fName;
}
/**
 ******************************************************************
 *
 * Function Name : SameName
 *
 * Description : Used to help in searches in the NavPointList. 
 * This will return true if the provided name equals that of the 
 * stored name. 
 *
 * Inputs : name to compare
 *
 * Returns : true if the input name and the stored name are the same. 
 *
 * Error Conditions : if fName or name is NULL. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NavPoint::SameName(const char *name)
{
    SET_DEBUG_STACK;
    bool rc =false;
    if (fName!=NULL)
    {
	if (fName->compare(name) == 0)
	{
	    rc = true;
	}
    }
    SET_DEBUG_STACK;
    return rc;
}

/**
 ******************************************************************
 *
 * Function Name : operator<< 
 *
 * Description : Overload for printing and saving the NavPoint
 * structure. 
 * A couple of fine points. 
 * IF the stored data format is specified as kFORMAT_RADIANS then the data
 * will be multiplied by 180/pi prior to output for human readablity. 
 * otherwise, this is left alone. 
 *
 * Inputs : ostream - stream to output to.
 *          in - the NavPoint to save.  
 *
 * Returns : constructed string
 *
 * Error Conditions :
 * 
 * Unit Tested on: 20-Feb-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<( ostream& output, const NavPoint &in)
{
    SET_DEBUG_STACK;
    const char *prefix = (const char *)"NONE";
    
    // It is 1852 m per arc minute of latitude. 
    // This allows us to calculate our digit precision
    // so 1e-8 in latitude is 1.11mm 3 digits for degree and 8 for 
    // radix, 11 total. 
    switch(in.fType)
    {
    case NavPoint::kWAYPOINT:
	prefix = (const char *) "WPT";
	break;
    case NavPoint::kEVENT:
	prefix = (const char *) "EVT";
	break;
    case NavPoint::kTRACK:
	prefix = (const char *) "TRK";
	break;
    }
    output.setf(ios::floatfield,ios::fixed);
    output.precision(8);
    if (in.fFormat == NavPoint::kFORMAT_RADIANS)
    {
	/* Convert prior to output. */
	output << "NavPoint, Prefix: " 
	       << prefix << NAV_SEPARATOR << "LON=" << in.X() * RadToDeg 
	       << NAV_SEPARATOR << "LAT=" << in.Y() * RadToDeg;
    }
    else
    {
	output << prefix << NAV_SEPARATOR << "X=" << in.X() 
	       << NAV_SEPARATOR << "Y=" << in.Y();
    }
    if ((in.fType == NavPoint::kEVENT) || (in.fType == NavPoint::kTRACK))
    {
	output.precision(2);
	output << NAV_SEPARATOR << "Z="
	       << in.Z()
	       << NAV_SEPARATOR << "dt=" 
	       << in.GetDt();
    }
    if (in.GetName())
    {
	output << NAV_SEPARATOR << "TITLE=" << in.GetName();
    }
    else
    {
	output << NAV_SEPARATOR << "TITLE=Event " << in.GetUniqueID(); 
    }

    if (in.CommentData() != NULL)
    {
	output << NAV_SEPARATOR << "COMMENT=" << in.CommentData();
    }
    else
    {
	output << NAV_SEPARATOR << "COMMENT= "; 
    }

    output << NAV_SEPARATOR;
    SET_DEBUG_STACK;
    return output;
}

void NavPoint::SetComment(const char *s)
{
    SET_DEBUG_STACK;
    if (fComment)
    {
	*fComment = s;
    }
    else
    {
	fComment = new std::string(s);
    }
    SET_DEBUG_STACK;
}
const char* NavPoint::CommentData(void) const
{
    SET_DEBUG_STACK;
    if(fComment)
    {
	return fComment->c_str();
    }
    else
    {
	return NULL;
    }
    SET_DEBUG_STACK;
}


#ifdef USE_MAIN
int main(int argc, char **argv)
{
    const char *testme = "WPT,X=0.00000000,Y=1.00000000,TITLE=One,";
    NavPoint np(0.0, 1.0, 2.0, NavPoint::kWAYPOINT, 34.0, "One");
    cout << np << endl;
    
    NavPoint np2(testme);
    cout << np2 << endl;
    exit(0);
}

#endif
