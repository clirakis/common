/********************************************************************
 *
 * Module Name : Measurement.cpp
 *
 * Author/Date : C.B. Lirakis / 01-Feb-11
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
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Measurement::Measurement (void) : CObject()
{
    SetName("Measurement");
    ClearError(__LINE__);
    memset(fActive, 0, 6*sizeof(uint8_t));
}

/**
 ******************************************************************
 *
 * Function Name : Measurement destructor
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
Measurement::~Measurement (void)
{
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
 * Unit Tested on: 
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
 *
 * Inputs : NONE
 *
 * Returns : Number of measurements available. 
 *
 * Error Conditions : On GPIB read
 * 
 * Unit Tested on: 
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
    uint32_t rv = 0;
    char     Response[256];
    char     *p, *q;

    ClearError(__LINE__);

    memset(fActive, 0, 6*sizeof(uint8_t));
    
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
	uint8_t count = 0;          // Measurement list count. 
	p = strchr(Response, ' ');  // Skip MSLIST response. 
	q = strtok( p, tok);        // Start the tokenization. 
	while (q!=NULL)
	{
	    if (strstr(q, "GAI") != NULL)
	    {
		fActive[count] = Measurement::kGAIN;
	    }
	    else if (strstr(q, "MAX") != NULL)
	    {
		fActive[count] = Measurement::kMAX;
	    }
	    else if (strstr(q, "MEA") != NULL)
	    {
		fActive[count] = Measurement::kMEAN;
	    }
	    else if (strstr(q, "MID") != NULL)
	    {
		fActive[count] = Measurement::kMID;
	    }
	    else if (strstr(q, "MIN") != NULL)
	    {
		fActive[count] = Measurement::kMIN;
	    }
	    else if (strstr(q, "OVE") != NULL)
	    {
		fActive[count] = Measurement::kOVERSHOOT;
	    }
	    else if (strstr(q, "PP") != NULL)
	    {
		fActive[count] = Measurement::kPP;
	    }
	    else if (strstr(q, "RMS") != NULL)
	    {
		fActive[count] = Measurement::kRMS;
	    }
	    else if (strstr(q, "UND") != NULL)
	    {
		fActive[count] = Measurement::kUNDERSHOOT;
	    }
	    else if (strstr(q, "YTE") != NULL)
	    {
		fActive[count] = Measurement::kYTENERGY;
	    }
	    else if (strstr(q, "YTM") != NULL)
	    {
		fActive[count] = Measurement::kYTMNS_AREA;
	    }
	    else if (strstr(q, "YTPL") != NULL)
	    {
		fActive[count] = Measurement::kYTPLS_AREA;
	    }
	    else if (strstr(q, "SFR") != NULL)
	    {
		fActive[count] = Measurement::kSFREQ;
	    }
	    else if (strstr(q, "SMA") != NULL)
	    {
		fActive[count] = Measurement::kSMAG;
	    }
	    else if (strstr(q, "THD") != NULL)
	    {
		fActive[count] = Measurement::kTHD;
	    }
	    else if (strstr(q, "CRO") != NULL)
	    {
		fActive[count] = Measurement::kCROSS;
	    }
	    else if (strstr(q, "DEL") != NULL)
	    {
		fActive[count] = Measurement::kDELAY;
	    }
	    else if (strstr(q, "DUT") != NULL)
	    {
		fActive[count] = Measurement::kDUTY;
	    }
	    else if (strstr(q, "FAL") != NULL)
	    {
		fActive[count] = Measurement::kFALLTIME;
	    }
	    else if (strstr(q, "FRE") != NULL)
	    {
		fActive[count] = Measurement::kFREQ;
	    }
	    else if (strstr(q, "PDE") != NULL)
	    {
		fActive[count] = Measurement::kPDELAY;
	    }
	    else if (strstr(q, "PER") != NULL)
	    {
		fActive[count] = Measurement::kPERIOD;
	    }
	    else if (strstr(q, "PHA") != NULL)
	    {
		fActive[count] = Measurement::kPHASE;
	    }
	    else if (strstr(q, "RIS") != NULL)
	    {
		fActive[count] = Measurement::kRISETIME;
	    }
	    else if (strstr(q, "SKE") != NULL)
	    {
		fActive[count] = Measurement::kSKEW;
	    }
	    else if (strstr(q, "TT") != NULL)
	    {
		fActive[count] = Measurement::kTTRIG;
	    }
	    else if (strstr(q, "WID") != NULL)
	    {
		fActive[count] = Measurement::kWIDTH;
	    }
	    count++;
	    q = strtok(NULL, tok);
	}
	rv = count;
    }
    SET_DEBUG_STACK;
    return rv;
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
 * Unit Tested on: 25-Jan-21
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
\\ */
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
 * Unit Tested on: 25-Jan-21
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
	fCross.Decode(Result);
    }
    else if (cmd.find("DELAY")==0)
    {
	fDelay.Decode(Result);
    }
    else if (cmd.find("DUTY")==0)
    {
	fDuty.Decode(Result);
    }
    else if (cmd.find("FALLTIME")==0)
    {
	fFalltime.Decode(Result);
    }
    else if (cmd.find("FREQ")==0)
    {
	fFreq.Decode( Result);
    }
    else if (cmd.find("GAIN")==0)
    {
	fGain.Decode( Result);
    }
    else if (cmd.find("MAX")==0)
    {
	fMax.Decode( Result);
    }
    else if (cmd.find("MEAN")==0)
    {
	fMean.Decode( Result);
    }
    else if (cmd.find("MID")==0)
    {
	fMidpoint.Decode( Result);
    }
    else if (cmd.find("MIN")==0)
    {
	fMin.Decode( Result);
    }
    else if ((rv = cmd.find("OVE"))==0)
    {
	fOvershoot.Decode( Result);
    }
    else if (cmd.find("PDE")==0)
    {
	fPDelay.Decode( Result);
    }
    else if (cmd.find("PER")==0)
    {
	fPeriod.Decode( Result);
    }
    else if (cmd.find("PHA")==0)
    {
	fPhase.Decode( Result);
    }
    else if (cmd.find("PP")==0)
    {
	fPeakToPeak.Decode( Result);
    }
    else if (cmd.find("RIS")==0)
    {
	fRisetime.Decode( Result);
    }
    else if (cmd.find("RMS")==0)
    {
	fRMS.Decode( Result);
    }
    else if (cmd.find("SFR")==0)
    {
	fSpectralFrequency.Decode( Result);
    }
    else if (cmd.find("SKEW")==0)
    {
	fSkew.Decode( Result);
    }
    else if (cmd.find("SMA")==0)
    {
	fSpectralMagnitude.Decode( Result);
    }
    else if (cmd.find("THD")==0)
    {
	fTotalHarmonicDistortion.Decode(Result);
    }
    else if (cmd.find("TTR")==0)
    {
	fTimeToTrigger.Decode(Result);
    }
    else if (cmd.find("UND")==0)
    {
	fUndershoot.Decode(Result);
    }
    else if (cmd.find("WID")==0)
    {
	fWidth.Decode(Result);
    }
    else if (cmd.find("YTE")==0)
    {
	fYTEnergy.Decode(Result);
    }
    else if (cmd.find("YTM")==0)
    {
	fYTMNS_Area.Decode(Result);
    }
    else if (cmd.find("YTP")==0)
    {
	fYTPLS_Area.Decode(Result);
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
 * Unit Tested on: 25-Jan-21
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
    output << "CROSS:        " << n.fCross << endl;
    output << "Delay:        " << n.fDelay << endl;
    output << "Duty :        " << n.fDuty << endl;
    output << "Falltime :    " << n.fFalltime << endl;
    output << "Frequency:    " << n.fFreq << endl;
    output << "Gain :        " << n.fGain << endl;
    output << "Max :         " << n.fMax << endl;
    output << "Mean :        " << n.fMean << endl;
    output << "Midpoint :    " << n.fMidpoint << endl;
    output << "Min :         " << n.fMin << endl;
    output << "Overshoot :   " << n.fOvershoot << endl;
    output << "PDelay :      " << n.fPDelay << endl;
    output << "Period :      " << n.fPeriod << endl;
    output << "Phase :       " << n.fPhase << endl;
    output << "Peak to Peak :" << n.fPeakToPeak << endl;
    output << "Rise Time :   " << n.fRisetime << endl;
    output << "RMS :         " << n.fRMS << endl;
    output << "Spectral Frequency :" << n.fSpectralFrequency << endl;
    output << "Skew :        " << n.fSkew << endl;
    output << "Spectral Magnitude :" << n.fSpectralMagnitude << endl;
    output << "Total Harmonic Distortion :" << n.fTotalHarmonicDistortion << endl;
    output << "Time To Trigger :" << n.fTimeToTrigger << endl;
    output << "Undershoot :   " << n.fUndershoot << endl;
    output << "Width:         " << n.fWidth << endl;
    output << "Y Total Energy:" << n.fYTEnergy << endl;
    output << "YT MNS Area:   " << n.fYTMNS_Area << endl;
    output << "YT PLS Area:   " << n.fYTPLS_Area << endl;
    output << "============================================" << endl;
    SET_DEBUG_STACK;
    return output;
}

