/********************************************************************
 *
 * Module Name : Measurement.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
 *
 * Description : This is the base class for the measurement grouping. 
 * Measurement applies to all the potential measurements that could be
 * made on a waveform. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * DSA602 Programming manual, 
 *  MS <meas>    page 203  Query only the measurement in question. 
 *  MSLIST       page 201  Set up to 6 possible measurements to be performed 
 *                         on the waveform. 
 *  MEAS?        page 192  Executes the commands.  This is complex 
 *  MSN          page 205  Number of measurements currently programmed into
 *                         the unit. 
 *
 * Other files/classes. 
 *  MValue - a way of storing measurements with a qualifier. 
 * 
 *
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <cstring>
#include <string>

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "DSA602.hh"
#include "Measurement.hh"

/**
 ******************************************************************
 *
 * Function Name : Measurement constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Measurement::Measurement (void) : CObject()
{
    SET_DEBUG_STACK;
    SetName("Measurement");
    ClearError(__LINE__);

    /*
     * This is kind of handy to keep around. I may remove it later. 
     * but for the active measurements, it tells me where to look
     * using one of the enums. 
     */
    memset(fActive, -1, 6*sizeof(int8_t));
    /*
     * Initialize all the fMeasurementA values. 
     * I.E. Shove the names into the classes. 
     */
    for(uint32_t i=0;i<kNMeasurements;i++)
    {
	fMeasurements[i] = new MeasurementA(Available[i]);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Measurement destructor
 *
 * Description : clean up measurement list. 
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions :
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Measurement::~Measurement (void)
{
    SET_DEBUG_STACK;
    for (uint32_t i=0;i<kNMeasurements; i++)
    {
	delete fMeasurements[i];
    }
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : Update
 *
 * Description : Update all in the measurement of the waveform.
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 27-Dec-22 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Measurement::Update(void) 
{
    SET_DEBUG_STACK;
    bool rv = Query("MEAS");
    //cout << *this << endl;
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetActiveIndex
 *
 * Description : Given an index see if 1) there are sufficent slots
 *               to actually do this and if so 2) store it in the
 *               fActive index array.
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Dec-22 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Measurement::SetActiveIndex(uint8_t idx)
{
    SET_DEBUG_STACK;
    bool rv = false;

    if (fNActive<kMaxReadout)
    {
	// It's ok, lets try.  find the next empty slot.

	uint8_t i=0;
	do {
	    i++;
	} while(fActive[i]>-1);
	fActive[i] = idx;
	rv = true;
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : SetActive
 *
 * Description : Set the specific measurement to be done. 
 *               Also, set the various arrays that to track this is active. 
 *               NOTE: Should I do a query rather than set it?? FIXME
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Measurement::SetActive(MTYPES type) 
{
    SET_DEBUG_STACK;
    DSA602  *pDSA602 = DSA602::GetThis();
    bool rv = false;

    /* 
     * Note: here I have to send the current measurements as well as 
     * the new one all in one message. Sooo, set that up. 
     *
     * First, can I add this measurement?
     */
    if (SetActiveIndex(type))
    {
	char command[128];
	char tmp[32];
	/*
	 * Setup the command to send. 
	 */
	memset( command, 0, sizeof(command));
	sprintf( command,"MSLI ");

	/*
	 * Now remake the list into a new master command by executing 
	 * the loop again and seting the proper values. 
	 */
	int8_t id;
	for (int8_t i=0;i<kMaxReadout;i++)
	{
	    id = fActive[i];
	    if (id > -1)
	    {
		sprintf(tmp, "%s,", fMeasurements[id]->Text());
		strcat( command, tmp);
	    }
	}

	// Now we are ready to send the command. 
	pDSA602->Command(command, NULL, 0);
	rv = true;
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : SetInactive
 *
 * Description : Reset the list with the specified ID removed. 
 *
 * Inputs : NONE
 *
 * Returns : true on success
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Measurement::SetInactive(MTYPES type) 
{
    SET_DEBUG_STACK;
    DSA602  *pDSA602 = DSA602::GetThis();
    bool rv = false;
    char command[128];
    char tmp[32];
    int8_t id;

    // Make a local copy of fActive
    int8_t Active[kMaxReadout];
    memcpy(Active, fActive, kMaxReadout*sizeof(int8_t));
    // loop over the local array and set the input id to -1 (inactive).
    for (int8_t i=0;i<kMaxReadout;i++)
    {
	if (Active[i] == type) Active[i] = -1;
    }
    // Clear the list - Set everything to false. 
    Clear();

    /*
     * Setup the command to send. 
     */
    sprintf( command,"MSLI ");

    /*
     * Now remake the list into a new master command by executing 
     * the loop again and seting the proper values. 
     */
    for (int8_t i=0;i<kMaxReadout;i++)
    {
	id = Active[i];
	if (id > -1)
	{
	    sprintf(tmp, "%s,", fMeasurements[id]->Text());
	    strcat(command, tmp);
	    fMeasurements[id]->SetState(true);
	    fActive[fNActive] = id;
	    fNActive++;
	}
    }

    // Now we are ready to send the command. 
    pDSA602->Command(command, NULL, 0);
    rv = true;

    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : NList
 *
 * Description : Return number of measurements that are currently 
 *               checked in the list. 
 *
 * Inputs : NONE
 *
 * Returns : Number of measurements available. 
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 25-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint32_t Measurement::NList(void)
{
    SET_DEBUG_STACK;
    DSA602  *pDSA602 = DSA602::GetThis();
    CLogger *log     = CLogger::GetThis();
    uint32_t rv = 0;
    char     Response[32];
    char    *p;
    ClearError(__LINE__);
    
    /*
     * Looking for something like: MSNUM 2
     */
    memset(Response, 0, sizeof(Response));
    if (pDSA602->Command( "MSN?", Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# Measurement::NList() %s\n", Response);
	}
	p = strchr(Response,' ');
	rv = atoi(p);
    }
    SET_DEBUG_STACK;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : ActiveList
 *
 * Description : Get the list of active elements to measure. 
 *               This is somewhat redundant with Update. 
 *
 * Inputs : NONE
 *
 * Returns : Number of measurements available. 
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
uint32_t Measurement::ActiveList(void)
{
    SET_DEBUG_STACK;
    const char *tok = ",";
    DSA602  *pDSA602 = DSA602::GetThis();
    CLogger *log     = CLogger::GetThis();
    char     Response[256];
    char     *p, *q;

    ClearError(__LINE__);

    Clear(); // Clear all active measurements
    /*
     * Looking for something like: MSLIST SFREQUENCY,SMAGNITUDE
     */
    memset(Response, 0, sizeof(Response));
    if (pDSA602->Command( "MSLIST?", Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# Measurement::ActiveList() %s\n", Response);
	}
	p = strchr(Response, ' ');  // Skip MSLIST response. 
	q = strtok( p, tok);        // Start the tokenization. 
	while (q!=NULL)
	{
	    if (strstr(q, "GAI") != NULL)
	    {
		fActive[fNActive] = Measurement::kGAIN;
		fNActive++;
		fMeasurements[kGAIN]->SetState(true);
	    }
	    else if (strstr(q, "MAX") != NULL)
	    {
		fActive[fNActive] = Measurement::kMAX;
		fNActive++;
		fMeasurements[kMAX]->SetState(true);
	    }
	    else if (strstr(q, "MEA") != NULL)
	    {
		fActive[fNActive] = Measurement::kMEAN;
		fNActive++;
		fMeasurements[kMEAN]->SetState(true);
	    }
	    else if (strstr(q, "MID") != NULL)
	    {
		fActive[fNActive] = Measurement::kMID;
		fNActive++;
		fMeasurements[kMID]->SetState(true);
	    }
	    else if (strstr(q, "MIN") != NULL)
	    {
		fActive[fNActive] = Measurement::kMIN;
		fNActive++;
		fMeasurements[kMIN]->SetState(true);
	    }
	    else if (strstr(q, "OVE") != NULL)
	    {
		fActive[fNActive] = Measurement::kOVERSHOOT;
		fNActive++;
		fMeasurements[kOVERSHOOT]->SetState(true);
	    }
	    else if (strstr(q, "PP") != NULL)
	    {
		fActive[fNActive] = Measurement::kPP;
		fNActive++;
		fMeasurements[kPP]->SetState(true);
	    }
	    else if (strstr(q, "RMS") != NULL)
	    {
		fActive[fNActive] = Measurement::kRMS;
		fNActive++;
		fMeasurements[kRMS]->SetState(true);
	    }
	    else if (strstr(q, "UND") != NULL)
	    {
		fActive[fNActive] = Measurement::kUNDERSHOOT;
		fNActive++;
		fMeasurements[kUNDERSHOOT]->SetState(true);
	    }
	    else if (strstr(q, "YTE") != NULL)
	    {
		fActive[fNActive] = Measurement::kYTENERGY;
		fNActive++;
		fMeasurements[kYTENERGY]->SetState(true);
	    }
	    else if (strstr(q, "YTM") != NULL)
	    {
		fActive[fNActive] = Measurement::kYTMNS_AREA;
		fNActive++;
		fMeasurements[kYTMNS_AREA]->SetState(true);
	    }
	    else if (strstr(q, "YTPL") != NULL)
	    {
		fActive[fNActive] = Measurement::kYTPLS_AREA;
		fNActive++;
		fMeasurements[kYTPLS_AREA]->SetState(true);
	    }
	    else if (strstr(q, "SFR") != NULL)
	    {
		fActive[fNActive] = Measurement::kSFREQ;
		fNActive++;
		fMeasurements[kSFREQ]->SetState(true);
	    }
	    else if (strstr(q, "SMA") != NULL)
	    {
		fActive[fNActive] = Measurement::kSMAG;
		fNActive++;
		fMeasurements[kSMAG]->SetState(true);
	    }
	    else if (strstr(q, "THD") != NULL)
	    {
		fActive[fNActive] = Measurement::kTHD;
		fNActive++;
		fMeasurements[kTHD]->SetState(true);
	    }
	    else if (strstr(q, "CRO") != NULL)
	    {
		fActive[fNActive] = Measurement::kCROSS;
		fNActive++;
		fMeasurements[kCROSS]->SetState(true);
	    }
	    else if (strstr(q, "DEL") != NULL)
	    {
		fActive[fNActive] = Measurement::kDELAY;
		fNActive++;
		fMeasurements[kDELAY]->SetState(true);
	    }
	    else if (strstr(q, "DUT") != NULL)
	    {
		fActive[fNActive] = Measurement::kDUTY;
		fNActive++;
		fMeasurements[kDUTY]->SetState(true);
	    }
	    else if (strstr(q, "FAL") != NULL)
	    {
		fActive[fNActive] = Measurement::kFALLTIME;
		fNActive++;
		fMeasurements[kFALLTIME]->SetState(true);
	    }
	    else if (strstr(q, "FRE") != NULL)
	    {
		fActive[fNActive] = Measurement::kFREQ;
		fNActive++;
		fMeasurements[kFREQ]->SetState(true);
	    }
	    else if (strstr(q, "PDE") != NULL)
	    {
		fActive[fNActive] = Measurement::kPDELAY;
		fNActive++;
		fMeasurements[kPDELAY]->SetState(true);
	    }
	    else if (strstr(q, "PER") != NULL)
	    {
		fActive[fNActive] = Measurement::kPERIOD;
		fNActive++;
		fMeasurements[kPERIOD]->SetState(true);
	    }
	    else if (strstr(q, "PHA") != NULL)
	    {
		fActive[fNActive] = Measurement::kPHASE;
		fNActive++;
		fMeasurements[kPHASE]->SetState(true);
	    }
	    else if (strstr(q, "RIS") != NULL)
	    {
		fActive[fNActive] = Measurement::kRISETIME;
		fNActive++;
		fMeasurements[kRISETIME]->SetState(true);
	    }
	    else if (strstr(q, "SKE") != NULL)
	    {
		fActive[fNActive] = Measurement::kSKEW;
		fNActive++;
		fMeasurements[kSKEW]->SetState(true);
	    }
	    else if (strstr(q, "TT") != NULL)
	    {
		fActive[fNActive] = Measurement::kTTRIG;
		fNActive++;
		fMeasurements[kTTRIG]->SetState(true);
	    }
	    else if (strstr(q, "WID") != NULL)
	    {
		fActive[fNActive] = Measurement::kWIDTH;
		fNActive++;
		fMeasurements[kWIDTH]->SetState(true);
	    }
	    q = strtok(NULL, tok);
	}
    }
    SET_DEBUG_STACK;
    return fNActive;
}

/**
 ******************************************************************
 *
 * Function Name : Query
 *
 * Description : Query and subsequently process the result (decode)
 *               into the proper variable. 
 *
 * Inputs : Command to query mainframe on. 
 *
 * Returns : true on success
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool Measurement::Query(const char *Command)
{
    SET_DEBUG_STACK;
    DSA602  *pDSA602 = DSA602::GetThis();
    CLogger *log     = CLogger::GetThis();
    char Response[128];
    char Interogate[16];
    ClearError(__LINE__);
    
    memset(Response, 0, sizeof(Response));
    sprintf(Interogate, "%s?",Command);

    if (pDSA602->Command( Interogate, Response, sizeof(Response)))
    {
	if(log->CheckVerbose(1))
	{
	    log->Log("# Measurement::Query() %s\n", Response);
	}

	// Make string of the response. 
	string res(Response);

	if (strcmp(Command,"MEAS") == 0)
	{
	    /*
	     * There are multiple responses. 
	     * For example:
	     * QUERY Response: 
	     *   MEAS FREQ:0.0E+0,ER,RISETIME:0.0E+0,EQ,PP:5.0E-1,EQ
	     * How do I parse this? Terminators are ER,EQ,LT,GT ???
	     * Advance to first space and strip
	     * Advance to first of string ER,EQ,LT,GT
	     * Go +2
	     * Pass to Decode
	     * Repeat
	     */

	    string toParse;

	    // Loop over the remaining string data. 
	    // Skip over the 'MEAS ' 
            // Starting point in parent string.
	    size_t start = res.find(' ')+1;  
	    size_t end, sep;

	    do {
		// Advance where we are looking. 
		end = res.find(',',start);

		// Advance +3, the length of the qualifier and comma.
		end += 3;

		// Parse out the substring
		toParse = res.substr(start,end-start);

		// toParse now contains something like "FREQ:0.0E+0,ER"
		start = end+1;   // This specifies where we will pick up 

		sep   = toParse.find(':');
		Decode(toParse.substr(0,sep).c_str(), 
		       toParse.substr(sep+1).c_str());
	    } while(end<res.size());
	    
	}
	else
	{
	    Decode(Command, res.substr(res.find(' ')+1).c_str());
	}
    }
    else
    {
	SetError(-1,__LINE__);
	SET_DEBUG_STACK;
	return false;
    }

    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Decode
 *
 * Description : Decode the singlets from individual commands.
 *  The format of the command should be something like "CROSS"
 *  The format of the input Result should have already stripped out
 *      the echo of the command. EG: 
 *
 *    The response from the GPIB query might look like: 
 *               "CROSS -9.942585E-7,EQ"
 *
 *    The input to this function should look like:
 *        Command = "CROSS"
 *        Result  = "-9.942585E-7,EQ"
 *
 * Inputs : the result of a singlet query. 
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Measurement::Decode(const char *Command, const char *Result)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    size_t rv;
    string cmd(Command);

    if (cmd.find("CROSS")==0)
    {
	fMeasurements[kCROSS]->Decode(Result);
    }
    else if (cmd.find("DELAY")==0)
    {
	// This may barf and find PDELAY - some degenerecy
	fMeasurements[kDELAY]->Decode(Result);
    }
    else if (cmd.find("DUTY")==0)
    {
	fMeasurements[kDUTY]->Decode(Result);
    }
    else if (cmd.find("FALLTIME")==0)
    {
	fMeasurements[kFALLTIME]->Decode(Result);
    }
    else if (cmd.find("FREQ")==0)
    {
	fMeasurements[kFREQ]->Decode(Result);
    }
    else if (cmd.find("GAIN")==0)
    {
	fMeasurements[kGAIN]->Decode(Result);
    }
    else if (cmd.find("MAX")==0)
    {
	fMeasurements[kMAX]->Decode(Result);
    }
    else if (cmd.find("MEAN")==0)
    {
	fMeasurements[kMEAN]->Decode(Result);
    }
    else if (cmd.find("MID")==0)
    {
	fMeasurements[kMID]->Decode(Result);
    }
    else if (cmd.find("MIN")==0)
    {
	fMeasurements[kMIN]->Decode(Result);
    }
    else if ((rv = cmd.find("OVE"))==0)
    {
	fMeasurements[kOVERSHOOT]->Decode(Result);
    }
    else if (cmd.find("PDE")==0)
    {
	fMeasurements[kPDELAY]->Decode(Result);
    }
    else if (cmd.find("PER")==0)
    {
	fMeasurements[kPERIOD]->Decode(Result);
    }
    else if (cmd.find("PHA")==0)
    {
	fMeasurements[kPHASE]->Decode(Result);
    }
    else if (cmd.find("PP")==0)
    {
	fMeasurements[kPP]->Decode(Result);
    }
    else if (cmd.find("RIS")==0)
    {
	fMeasurements[kRISETIME]->Decode(Result);
    }
    else if (cmd.find("RMS")==0)
    {
	fMeasurements[kRMS]->Decode(Result);
    }
    else if (cmd.find("SFR")==0)
    {
	fMeasurements[kSFREQ]->Decode(Result);
    }
    else if (cmd.find("SKEW")==0)
    {
	fMeasurements[kSKEW]->Decode(Result);
    }
    else if (cmd.find("SMA")==0)
    {
	fMeasurements[kSMAG]->Decode(Result);
    }
    else if (cmd.find("THD")==0)
    {
	fMeasurements[kTHD]->Decode(Result);
    }
    else if (cmd.find("TTR")==0)
    {
	fMeasurements[kTTRIG]->Decode(Result);
    }
    else if (cmd.find("UND")==0)
    {
	fMeasurements[kUNDERSHOOT]->Decode(Result);
    }
    else if (cmd.find("WID")==0)
    {
	fMeasurements[kWIDTH]->Decode(Result);
    }
    else if (cmd.find("YTE")==0)
    {
	fMeasurements[kYTENERGY]->Decode(Result);
    }
    else if (cmd.find("YTM")==0)
    {
	fMeasurements[kYTMNS_AREA]->Decode(Result);
    }
    else if (cmd.find("YTP")==0)
    {
	fMeasurements[kYTPLS_AREA]->Decode(Result);
    }
     SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Find
 *
 * Description : Return the Measurement A associated with the input name
 *
 * Inputs : v - name to find in list. 
 *
 * Returns : MeasurementA if in list. NULL if not. 
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
MeasurementA*  Measurement::Find(const char *v)
{
    SET_DEBUG_STACK;
    for (uint32_t i=0;i<kNMeasurements;i++)
    {
	if(fMeasurements[i]->Match(v))
	    return fMeasurements[i];
    }
    SET_DEBUG_STACK;
    return NULL;
}
/**
 ******************************************************************
 *
 * Function Name : Clear
 *
 * Description : Reset the State of the measuement vector to all false
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void Measurement::Clear(void)
{
    SET_DEBUG_STACK;
    // Number of active is now zero. 
    fNActive = 0;
    // Set the active ids to -1 (none)
    memset(fActive, -1, 6*sizeof(int8_t));

    // Reset all the Active measurements in the long array to off. 
    for (uint32_t i=0;i<kNMeasurements;i++)
    {
	fMeasurements[i]->SetState(false);
    }

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Measurement operator <<
 *
 * Description : print out all of the data and staus of the
 *               said data associated with the measurement 
 *               command group. 
 *
 * Inputs : 
 *    output - the stream to insert the character description into. 
 *    n      - the Measurement class 
 *
 * Returns : 
 *     Formatted string 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 27-Dec-22
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const Measurement &n)
{
    SET_DEBUG_STACK;
    output << "============================================" << endl
	   << "Measurement:  " << endl ;
    for (uint32_t i=0; i<Measurement::kNMeasurements; i++)
    {
	output << *n.fMeasurements[i]     << endl;
    }
    output << " Active " << n.fNActive << endl;
    for (int8_t i=0; i<Measurement::kMaxReadout; i++) 
	output << (int) n.fActive[i] << " ";
    output << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}

