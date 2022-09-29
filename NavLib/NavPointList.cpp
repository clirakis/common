/********************************************************************
 *
 * Module Name : NavPointList.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : A collection of NavPoints using TList. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 02-Apr-22  CBL Cleaned up a bit. 
 *
 * Classification : Unclassified
 *
 * References :
 * https://root.cern.ch/doc/master/classTList.html
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;

// Root includes
#include <TList.h>

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
NavPointList::NavPointList (const char *f, Int_t type)
{
    char *SensorName, *suffix;
    const char *filename;
    SET_DEBUG_STACK;

    // Create a new list for the points to live in. 
    fNavPoints = new TList();
    // Tell the list to execute a delete when Clear is called. 
    fNavPoints->SetOwner();
    fType = type;
    fError = kNPL_NONE;

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
    case NavPoint::kTRACK:
	SensorName   = (char *)"TRK";
	suffix       = (char *)"trk";
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
    fFilename = new TString(filename);
    fSave     = kFALSE; // We are up to date in writing. 
    fFirst    = kTRUE;  // The file has not yet been written to.
    fNavPointCount = 0;
    fTitle    = NULL;
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : NavPointList destructor
 *
 * Description : Clean up all the points, title of the list and 
 * filename if allocated. 
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
NavPointList::~NavPointList (void)
{
    SET_DEBUG_STACK;
    Close();
    delete fFileName;
    delete fNavPoints;
    delete fFilename;
    delete fTitle;
}

/**
 ******************************************************************
 *
 * Function Name : Load
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
Bool_t NavPointList::Load(const char *filename)
{
    Char_t        line[256], *p;
    NavPoint*     m;
    Int_t         i;
    SET_DEBUG_STACK;

    fError = kNPL_NONE;

    // Return if there is no filename. 
    if (filename == NULL)
    {
	fError = kNPL_FILENAME_ERROR;
	return kFALSE;
    }

    // Open a file for read. 
    ifstream  indata(filename);
    if (indata.fail())
    {
	// Also a bad thing.
	fError = kNPL_READ_ERROR;
	return kFALSE;
    }

    // If we got this far, we have a file!
    // Clear current list. 
    fNavPoints->Clear();
    fNavPointCount = 0;

    // Set new current filename. 
    delete fFilename;
    fFilename = new TString(filename);
    fIsXY = kFALSE;
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
	    //cout << "TYPE: " << fType << endl;
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
	    fTitle = new TString(p);
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
	    fNavPoints->Add(m);
	    fSave = kFALSE;
	}
    }
    indata.close();
    if (fNavPointCount == 0)
    {
	fError = kNPL_EMPTY;
	SET_DEBUG_STACK;
	return kFALSE;
    }
    SET_DEBUG_STACK;
    return kTRUE;
}


/**
 ******************************************************************
 *
 * Function Name : Write
 *
 * Description : Save all events in the list to the specified file. 
 *               This creates the file and writes the header information. 
 *               I points are available they are flushed to the file. 
 *               The Save variable is set to false after this operation. 
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
Bool_t NavPointList::Write(const char *filename, bool Append)
{
    const char *file;
    TListIter  next(fNavPoints);
    NavPoint*     m;
    ofstream output;
    SET_DEBUG_STACK;
    
    if (filename == NULL)
    {
	file = fFilename->Data();
    }
    else
    {
	file = filename;
	delete fFilename;
	fFilename = new TString(filename);
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
	return kFALSE;
    }

    WriteHeader(output);

    while ((m = (NavPoint*) next()))
    {
	output << *m << endl;
    }
    output.close();
    /* File is up to date with all data */
    fSave  = kFALSE;
    /* The header has been written      */
    fFirst = kFALSE; 

    return kTRUE;
}

/**
 ******************************************************************
 *
 * Function Name : Operator << 
 *
 * Description : overload to output data in file or screen stream
 *
 * Inputs : 
 *      output - ostream to format into
 *      in - NavPointList structure to format
 *
 * Returns : formatted ostream
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
ostream& operator<<( ostream& output, const NavPointList &in)
{
    SET_DEBUG_STACK;
    TListIter  next(in.fNavPoints);
    NavPoint*     m;

    while ((m = (NavPoint*) next()))
    {
	output << *m << endl;
    }

    SET_DEBUG_STACK;
    return output;
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
Bool_t NavPointList::WriteHeader(ofstream &output)
{
    time_t    now;
    char      msg[128];
    SET_DEBUG_STACK;
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
	       << "# Type: 0  " << endl;
	break;
    case NavPoint::kEVENT:
	output << "# File type is Event." << endl
	       << "# Type: 1 " << endl;
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
    return kTRUE;
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
Bool_t NavPointList::Close(void)
{
    time_t    now;
    char      msg[128];
    SET_DEBUG_STACK;

    /* Return if there are no records. */
    if (fNavPoints->GetSize() == 0)
    {
	return kFALSE;
    }
    if (fSave)
    {
	Write(NULL, false);
    }
    ofstream  output(fFilename->Data(), ios::app);
    if (output.fail())
    {
	return kFALSE;
    }
    time(&now);
    strftime (msg, sizeof(msg), "%m-%d-%y %H:%M:%S", gmtime(&now));
    output << "# ***************************************************" << endl;
    output << "# File closed on: " 
	   << msg << endl;
    output << "# Number records written: " << fNavPoints->GetSize() << endl;
    output << "# ***************************************************" << endl;
    output.close();
    SET_DEBUG_STACK;
    return kTRUE;
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
Bool_t NavPointList::AppendCurrent(void)
{
    SET_DEBUG_STACK;
    if (fFirst)
    {
	return Write(NULL, true);
    }
    else
    {
	NavPoint*    m;
	ofstream  output(fFilename->Data());
	if (output.fail(), ios::app)
	{
	    return kFALSE;
	}
	m = (NavPoint *) fNavPoints->Last();
	output << *m << endl;
	output.close();
	fSave = kFALSE;
    }
    SET_DEBUG_STACK;
    return kTRUE;
}
/**
 ******************************************************************
 *
 * Function Name : Add 
 *
 * Description : Add a waypoint
 *
 * Inputs : X,Y,Z, time, and some title
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
void NavPointList::Add(Double_t Lon, Double_t Lat, Double_t Z, 
		    Double_t dt, const char* title)
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
    else if ((fType == NavPoint::kTRACK) && (title==NULL))
    {
	sprintf(name, "TRK-%2.2d", fNavPointCount);
	p = name;
    }
    else
    {
	p = title;
    }
    // Format X then Y, Height, point type, dt and title. 
    NavPoint* m = new NavPoint( Lon, Lat, Z, fType, dt, p);
    m->SetUniqueID(fNavPointCount);
    fNavPointCount++;
    fNavPoints->Add(m);
    fSave = kTRUE;
    AppendCurrent(); // Write to file. 
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Point
 *
 * Description : Access a particular point in the list. 
 *
 * Inputs : An index between 0 and Size-1
 *
 * Returns : NavPoint at the desired index. 
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
NavPoint* NavPointList::Point(UInt_t index)
{
    SET_DEBUG_STACK;
    NavPoint* rv = NULL;
    if ((index>=0) && (index<fNavPoints->GetSize()))
    {
	rv = (NavPoint*)fNavPoints->At(index);
    }
    SET_DEBUG_STACK;
    return rv;
}

UInt_t NavPointList::Size(void)
{
    return fNavPoints->GetSize();
}
