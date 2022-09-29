/********************************************************************
 *
 * Module Name : SolutionStatus.cpp
 *
 * Author/Date : C.B. Lirakis / 25-Nov-17
 *
 * Description : Moved out of lassen.cpp, This has all the data
 * associated with the fix data packet, 0x6D.
 *
 * Restrictions/Limitations : NONE
 *
 * Change Descriptions : NONE
 *
 * Classification : Unclassified
 *
 * References : NONE
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
#include "SolutionStatus.hh"

/**
 ******************************************************************
 *
 * Function Name : SolutionStatus
 *
 * Description : Constructor, clear out the all the variables. 
 * put a DateTimeStamp on the construction. 
 *
 * Inputs : NONE
 *
 * Returns : 
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
SolutionStatus::SolutionStatus(): DataTimeStamp()
{
    SET_DEBUG_STACK;
    Clear();
}
/**
 ******************************************************************
 *
 * Function Name : SolutionStatus destructor
 *
 * Description : just clears the variables for the moment. 
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
SolutionStatus::~SolutionStatus(void)
{
    // Nothing to do right now. 
    SET_DEBUG_STACK;
    Clear();
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : Set everything to zero
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SolutionStatus::Clear(void)
{
    SET_DEBUG_STACK;
    fNSV      = 0;
    fSolution = 0;
    fPDOP     = fHDOP = fVDOP = fTDOP = 0.0;
    memset(fPRN, 0, sizeof(fPRN));
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Fill
 *
 * Description : Populate the values of the structure. 
 *
 * Inputs : 
 *
 * Returns : NONE
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
void SolutionStatus::Fill(unsigned char solution, unsigned char NSV, 
			  float PDOP, float HDOP, float VDOP, float TDOP)
{
    SET_DEBUG_STACK;
    fSolution = solution;
    fNSV      = NSV;
    fPDOP     = PDOP;
    fHDOP     = HDOP;
    fVDOP     = VDOP;
    fTDOP     = TDOP;
}

/**
 ******************************************************************
 *
 * Function Name : SolutionStatus operator << overload
 *
 * Description :
 *
 * Inputs : output stream to append to
 *          n - solution status class
 *
 * Returns : packed ostream to do with as you please. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 25-Nov-17
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const SolutionStatus &n)
{
    SET_DEBUG_STACK;
    output << " Solution Status NSV: " << (int) n.fNSV << " ";
    /**
     * Solution status - derived from mode, Bit 3: 0 - Auto, 1-Manual
     * Bits 0:2 3 - 2D, 4 - 3D
     */
    unsigned char Mode = n.fSolution&0x07;
    bool Auto = ((n.fSolution&0x08)>0);
    if (Auto)
    {
	output << "Manual ";
    }
    else
    {
	output << "Auto   ";
    }
    switch(Mode)
    {
    case 0:
	output << "Unknown ";
	break;
    case 1:
	output << "Zero-D ";
	break;
    case 2:
	output << "2D Clock Hold ";
	break;
    case 3:
	output << "2D ";
	break;
    case 4:
	output << "3D ";
	break;
    case 5:
	output << "Overdeterminded Clock ";
	break;
    case 6:
	output << "DGPS Ref. ";
	break;
    }
    output << " PDOP: " << n.fPDOP
	   << " HDOP: " << n.fHDOP
	   << " VDOP: " << n.fVDOP
	   << " TDOP: " << n.fTDOP;

    output << " PRNs: ";
    for (int i=0;i<n.fNSV;i++)
    {
	output << " " << (int) n.fPRN[i];
    }
    SET_DEBUG_STACK;
    return output;
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
 * Returns : NONE
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
unsigned char SolutionStatus::Mode(void) const 
{
    SET_DEBUG_STACK;
    return fSolution&0x07;
}
