/********************************************************************
 *
 * Module Name : NavPointList.cpp
 *
 * Author/Date : C.B. Lirakis / 20-Feb-17
 *
 * Description : Generic NavPointList, rewrote using stl.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
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
#include <fstream>
#include <iterator>
#include <stdlib.h>

// Local Includes.
#include "debug.h"
#include "NavPoint.hh"
#include "NavPointList.hh"
#include "filename.hh"
#include "LibVersion.hh"

/**
 ******************************************************************
 *
 * Function Name : NavPointList constructor
 *
 * Description : if the filename is provided, use it. 
 * otherwise open a new one that is unique. 
 *
 * Inputs : filename to write to. If the filename is NULL, a unique filename
 * will be created in the form of YearXXXJJJ_RR.XXX
 * where XXX is either wpt or evt and JJJ is julian day of year and RR is
 * file revision number. 
 *
 * Returns : fully constructed class
 *
 * Error Conditions : 
 * 
 * Unit Tested on: 8-Nov-15
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
NavPointList::NavPointList (const char *f, int type)
{
    SET_DEBUG_STACK;
    char *SensorName, *suffix;
    const char *filename;

    fType = type;
    switch( fType)
    {
    case NavPoint::kWAYPOINT:
	SensorName   = (char *)"WPT";
	suffix       = (char *)"wpt";
	break;
    case NavPoint::kEVENT:
	SensorName   = (char *)"EVT";
	suffix       = (char *)"evt";
	break;
    }
    fFileName = new FileName(SensorName, suffix, One_Hour);
    if (f)
    {
	filename = f;
    }
    else
    {
	filename = (char *) fFileName->GetUniqueName();
    }

    fFilename = new string(filename);
    fSave     = false; // We are up to date in writing. 
    fFirst    = true;  // The file has not yet been written to.
    fNavPointCount = 0;
    fTitle    = NULL;
    fIsXY     = false;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : NavPointList destructor
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
NavPointList::~NavPointList ()
{
    SET_DEBUG_STACK;
    //NavPoint *pNP;
    for(std::list<NavPoint*>::iterator it = fNavPoints.begin();
	it != fNavPoints.end(); it++)
    {
	delete (*it);
    }
    fNavPoints.clear();

    Close();
    delete fFileName;
    delete fFilename;
    delete fTitle;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Read
 *
 * Description : Load data from a file
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
bool NavPointList::Read(const char *filename)
{
    SET_DEBUG_STACK;
    char        line[256], *p;
    NavPoint*   m;
    int         i;

    // Return if there is no filename. 
    if (filename == NULL)
    {
	return false;
    }

    // Open a file for read. 
    ifstream  indata(filename);
    if (indata.fail())
    {
	// Also a bad thing. 
	return false;
    }

    // If we got this far, we have a file!
    // Clear current list. 
    fNavPoints.clear();
    fNavPointCount = 0;

    // Set new current filename. 
    delete fFilename;
    fFilename = new string(filename);
    fIsXY = false;
    while(!indata.eof())
    {
	memset(line, 0, sizeof(line));
	indata.getline(line, sizeof(line));
	if (strlen(line) == 0)
	{
	    // do nothing.
	}
	else if ((p = strstr(line, "Type:")) != NULL)
	{
	    p += 5;
	    fType = atoi(p);
	}
	else if((p = strstr( line, "IsXY:")) != NULL)
	{
	    p += 5;
	    i = atoi(p);
	    fIsXY = (i>0);
	}
	else if ((p = strstr( line, "Title:")) != NULL)
	{
	    p+=sizeof("Title:");
	    fTitle = new string(p);
	}
	else if (line[0] == '#')
	{
	    // skip
	}
	else
	{
	    m = new NavPoint(line);
	    //cout << *m << endl;
	    fNavPointCount++;
	    fNavPoints.push_back(m);
	    fSave = false;
	}
    }
    indata.close();
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Write
 *
 * Description : Save all events in the list to the specified file. 
 * This creates the file and writes the header information. I points
 * are available they are flushed to the file. The Save
 * variable is set to false after this operation. 
 *
 * Inputs : filename to use, if NULL, use the last filename provided. 
 *
 * Returns : True on success
 *
 * Error Conditions : Could not open file. 
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NavPointList::Write(const char *filename, bool Append)
{
    SET_DEBUG_STACK;
    const char *file;
    ofstream output;
    
    if (filename == NULL)
    {
	file = fFilename->c_str();
    }
    else
    {
	file = filename;
	delete fFilename;
	fFilename = new string(filename);
    }

    if (Append)
    {
	output.open(file, ios::app);
    }
    else
    {
	output.open(file);
    }

    if (output.fail())
    {
	return false;
    }
    if (!Append) 
    {
	/* Only write header if we are not in an append mode. */
	WriteHeader(output);
    }
    for(std::list<NavPoint*>::iterator pNP = fNavPoints.begin();
	pNP != fNavPoints.end(); pNP++)
    {
	output << *(*pNP) << endl;
    }
    output.flush();
    output.close();
    /* File is up to date with all data */
    fSave  = false;
    /* The header has been written      */
    fFirst = false; 
    SET_DEBUG_STACK;

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : WriteHeader
 *
 * Description : write the header for the NavPoint file. 
 *
 * Inputs : none
 *
 * Returns : True on success
 *
 * Error Conditions : file not open, error in writing. 
 * 
 * Unit Tested on: 8-Nov-15
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NavPointList::WriteHeader(ofstream &output)
{
    SET_DEBUG_STACK;
    time_t    now;
    char      msg[128];
    time(&now);
    strftime (msg, sizeof(msg), "%m-%d-%y %H:%M:%S", gmtime(&now));
    output << "# ***************************************************" << endl;
    output << "# GPS NavPoint log file opened on: "
	   << msg << endl;
    output << "# Version:  " << MAJOR_VERSION << "." << MINOR_VERSION << endl;
    output << "# Lat and Lon are in degrees, Z is in meters relative" << endl
	   << "# to mean geoid." << endl;
    switch( fType)
    {
    case NavPoint::kWAYPOINT:
	output << "# File type is Waypoint." << endl
	       << "# Type: " << NavPoint::kWAYPOINT << endl;
	break;
    case NavPoint::kEVENT:
	output << "# File type is Event." << endl
	       << "# Type: " << NavPoint::kEVENT << endl;
	break;
    }
    if (fIsXY)
    {
	output << "# IsXY: 1" << endl;
    }
    else
    {
	output << "# IsXY: 0" << endl;
    }
    if (fTitle)
    {
	output << "# Title: " << *fTitle << endl;
    }
    else
    {
	output << "# Title: None" << endl;
    }
    output << "# ***************************************************" << endl;
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : Close 
 *
 * Description : Close the file and put something at the end. 
 *
 * Inputs : none
 *
 * Returns : True on success
 *
 * Error Conditions : file not open, error in writing. 
 * 
 * Unit Tested on: 8-Nov-15
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NavPointList::Close(void)
{
    SET_DEBUG_STACK;
    time_t    now;
    char      msg[128];

    /* Return if there are no records. */
    if (fNavPoints.size() == 0)
    {
	return false;
    }
    if (fSave)
    {
	Write(NULL, false);
    }
    ofstream  output(fFilename->c_str(), ios::app);
    if (output.fail())
    {
	return false;
    }
    time(&now);
    strftime (msg, sizeof(msg), "%m-%d-%y %H:%M:%S", gmtime(&now));
    output << "# ***************************************************" << endl;
    output << "# File closed on: " 
	   << msg << endl;
    output << "# Number records written: " << fNavPoints.size() << endl;
    output << "# ***************************************************" << endl;
    output.close();
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : AppendCurrent
 *
 * Description : Append Current navpoint to file. If the file has never
 * been written to, the file is created and filled. If the file has 
 * been previously created, then it is opened, the current navpoint 
 * data is written in and it is closed. 
 *
 * Inputs : none
 *
 * Returns : True on success. 
 *
 * Error Conditions :
 * 
 * Unit Tested on: 8-Nov-15
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NavPointList::AppendCurrent(void)
{
    SET_DEBUG_STACK;
    if (fFirst)
    {
	return Write(NULL, false);
	fFirst = false;
    }
    else
    {
	NavPoint*    m;
	ofstream  output(fFilename->c_str(), ios_base::app);
	if (output.fail())
	{
	    return false;
	}
	m = (NavPoint *) fNavPoints.back();
	output << *m << endl;
	output.flush();
	output.close();
	fSave = false;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Add 
 *
 * Description : Add a waypoint
 *
 * Inputs : X,Y,Z, time, and some title. While Lon, Lat are called
 * out, this doesn't really mean that it couldn't be XY. 
 * Note, there is no conversion done here. It is upto the calling 
 * method to provide the appropriate conversion. 
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NavPointList::Add(double Lon, double Lat, double Z, 
		    double dt, const char* title)
{
    SET_DEBUG_STACK;
    const char *p;
    char name[32];

    if ((fType == NavPoint::kWAYPOINT) && (title==NULL))
    {
	sprintf(name, "WPT-%2.2d", fNavPointCount);
	p = name;
    }
    else if ((fType == NavPoint::kEVENT) && (title==NULL))
    {
	sprintf(name, "EVT-%2.2d", fNavPointCount);
	p = name;
    }
    else
    {
	p = title;
    }
    // Format X then Y, Height, point type, dt and title. 
    NavPoint* m = new NavPoint( Lon, Lat, Z, fType, dt, p);
    /* By default, NavPoint assumes that the user will be 
     * using radians to store the data. 
     * If the NavPointList was created as XY, we should store 
     * the data as XY. This means that this NavPoint creation needs
     * to be set as such. 
     */
    if(fIsXY)
    {
	m->SetFormat(NavPoint::kFORMAT_NONE);
    }
    m->SetUniqueID(fNavPointCount);
    fNavPointCount++;
    fNavPoints.push_back(m);
    fSave = true;
    AppendCurrent(); // Write to file. 
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Add 
 *
 * Description : Add a waypoint, but with an existing NavPoint. 
 * Have to be a little careful here about mixing input formats!!
 *
 * Inputs : pre-existing NavPoint
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NavPointList::Add (const NavPoint &p)
{
    SET_DEBUG_STACK;
    NavPoint* m = new NavPoint( p);
    //cout << "NavPointList:Add " << p
    //<< " " << *m << endl;
    m->SetUniqueID(fNavPointCount);
    fNavPointCount++;
    fNavPoints.push_back(m);
    fSave = true;
    AppendCurrent(); // Write to file. 
    SET_DEBUG_STACK;
}

int NavPointList::Size(void)
{
    SET_DEBUG_STACK;
    return fNavPoints.size();
}
#ifdef USE_MAIN
int main(int argc, char **argv)
{
    NavPointList npl("test.wpt");

    npl.Add(0.0,2.0,0.0, 0.0, "WPT-1");
    npl.Add(0.0,3.0,0.0, 0.0, "WPT-2");
    npl.Add(0.0,4.0,0.0, 0.0, "WPT-3");
    npl.Add(0.0,5.0,0.0, 0.0, "WPT-4");
    npl.Write("test2.wpt",false);

    NavPointList npl2;
    npl2.Read("test2.wpt");
    npl2.Write("test3.wpt",false);

    exit(0);
}

#endif
