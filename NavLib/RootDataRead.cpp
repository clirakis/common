/**
 ******************************************************************
 *
 * Module Name : RootDataRead.cpp
 *
 * Author/Date : C.B. Lirakis / 05-Mar-19
 *
 * Description : Lassen control entry points. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions : 
 *
 * Classification : Unclassified
 *
 * References : https://root.cern.ch
 *
 *
 *******************************************************************
 */  
// System includes.
#include <iostream>
using namespace std;

/// Root includes
#include <TFile.h>
#include <TTree.h>

/// Local Includes.
#include "RootDataRead.hh"
#include "CLogger.hh"
#include "tools.h"
#include "debug.h"

RootDataRead* RootDataRead::fRootDataRead;

/**
 ******************************************************************
 *
 * Function Name : RootDataRead constructor
 *
 * Description : initialize CObject variables
 *
 * Inputs : currently none. 
 *
 * Returns : none
 *
 * Error Conditions :
 * 
 * Unit Tested on: 12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RootDataRead::RootDataRead(const char* File) 
{
    CLogger *Logger = CLogger::GetThis();

    /* Store the this pointer. */
    fRootDataRead = this;
    fFile         = NULL;
    fTree         = NULL;
    fNentries     = 0;
    fCurrent      = 0;
    fError        = kFALSE;

    Zero();

    if (OpenFile(File))
    {
	Logger->Log("# RootDataRead constructed.\n");
    }
    else
    {
	Logger->Log("# RootDataRead open error.\n");
	fError = kTRUE;
    }

    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : RootDataRead Destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RootDataRead::~RootDataRead(void)
{
    SET_DEBUG_STACK;
    CLogger *Logger = CLogger::GetThis();
    delete fTree;
    fTree = NULL;
    delete fFile;
    fFile = NULL;
    fRootDataRead = NULL;
    // Make sure all file streams are closed
    Logger->Log("# RootDataRead closed.\n");
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Do
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void RootDataRead::Do(void)
{
    SET_DEBUG_STACK;
    for (Int_t i=0;i<100;i++)
    {
	ReadPoint();
	cout << *this << endl;
    }
    SET_DEBUG_STACK;
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

/**
 ******************************************************************
 *
 * Function Name : ReadPoint
 *
 * Description : Read a point from the ntuple. If the supplied index <0
 * then use the internal pointer and update it. Otherwise use the 
 * pointer specifed.
 *
 * Inputs : index - index for point to read. 
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const TVector3 &RootDataRead::ReadPoint(Long64_t index) 
{
    SET_DEBUG_STACK;
    fError = kFALSE;

    if (index >= 0)
    {
	if (index < fNentries)
	{
	    fCurrent = index;
	    fTree->GetEntry(fCurrent);
	}
	else
	{
	    fError = kTRUE;
	}
    }
    else
    {
	fTree->GetEntry(fCurrent);
	fCurrent++;
	if (fCurrent > fNentries) fCurrent = 0;
    }
    fPoint.SetXYZ(fLon, fLat, fZ);
    SET_DEBUG_STACK;
    return fPoint;
}

/**
 ******************************************************************
 *
 * Function Name : OpenFile
 *
 * Description : Open and manage the root file
 *
 * Inputs : none
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on:  12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool RootDataRead::OpenFile(const char *filename)
{
    SET_DEBUG_STACK;

    CLogger *pLogger = CLogger::GetThis();

    fFile = new TFile(filename, "READ");
    if (fFile->IsZombie())
    {
	pLogger->LogError(__FILE__, __LINE__, 1, filename);
	delete fFile;
	fFile = NULL;
	SET_DEBUG_STACK;
	return false;
    }
    else
    {
	pLogger->Log("# RootDataRead, open file: %s\n", filename);
    }
    fTree = (TTree *)fFile->Get("GPS");

    fNentries = fTree->GetEntries();
    fTree->SetBranchAddress("Time", &fTime);
    fTree->SetBranchAddress("Lat",  &fLat);
    fTree->SetBranchAddress("Lon",  &fLon);
    fTree->SetBranchAddress("Z",    &fZ);
    fTree->SetBranchAddress("VE",   &fVE);
    fTree->SetBranchAddress("VN",   &fVN);
    fTree->SetBranchAddress("VZ",   &fVZ);
    fTree->SetBranchAddress("NSV",  &fNSV);
    fTree->SetBranchAddress("HDOP", &fHDOP);

    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : operator<< 
 *
 * Description :
 *     Formatted output of RootDataRead class data. 
 * 
 * Inputs :
 *    output - ostream data
 *    in     - RootDataRead data.
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 12-Mar-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<( ostream& output, const RootDataRead &in)
{
    output.setf(ios::floatfield,ios::fixed);
    output.precision(8);
    output << in.fCurrent 
	   << " " << in.fLat
	   << " " << in.fLon;
    output.precision(3);
    output << " " << in.fZ
	   << " " << in.fVE
	   << " " << in.fVN
	   << " " << in.fVZ;
    output.precision(2);
    output << " " << (int) in.fNSV
	   << " " << in.fHDOP;
    return output;
}
