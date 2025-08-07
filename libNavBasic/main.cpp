/**
 ******************************************************************
 *
 * Module Name : main.cpp
 *
 * Author/Date : C.B. Lirakis / 19-Feb-17
 *
 * Description : test program for Geodetic
 *
 * Restrictions/Limitations : 
 *
 * Change Descriptions : 
 *
 * Classification : Unclassified
 *
 * References : 
 *
 *******************************************************************
 */
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <csignal>
#include <unistd.h>

// Qt includes, 

// Local Includes.
#include "debug.h"
#include "CLogger.hh"
#include "Geodetic.hh"

static CLogger* Logger;
static bool     Verbose;
/**
******************************************************************
*
* Function Name : Terminate
*
* Description : Deal with errors in a clean way!
*               ALL, and I mean ALL exits are brought 
*               through here!
*
* Inputs : Signal causing termination. 
*
* Returns : none
*
* Error Conditions : Well, we got an error to get here. 
*
*******************************************************************
*/ 
void Terminate (int sig) 
{
    static int i=0;
    char   msg[128];

    i++;
    if (i>1) 
    {
        _exit(-1);
    }
    switch (sig)
    {
    case -1: 
      sprintf( msg, "User abnormal termination");
      break;
    case 0:                    // Normal termination
        sprintf( msg, "Normal program termination.");
        break;
    case SIGHUP:
        sprintf( msg, " Hangup");
        break;
    case SIGINT:               // CTRL+C signal 
        sprintf( msg, " SIGINT ");
        break;
    case SIGQUIT:               //QUIT 
        sprintf( msg, " SIGQUIT ");
        break;
    case SIGILL:               // Illegal instruction 
        sprintf( msg, " SIGILL ");
        break;
    case SIGABRT:              // Abnormal termination 
        sprintf( msg, " SIGABRT ");
        break;
    case SIGBUS:               //Bus Error! 
        sprintf( msg, " SIGBUS ");
        break;
    case SIGFPE:               // Floating-point error 
        sprintf( msg, " SIGFPE ");
        break;
    case SIGKILL:               // Kill!!!! 
        sprintf( msg, " SIGKILL");
        break;
    case SIGSEGV:              // Illegal storage access 
        sprintf( msg, " SIGSEGV ");
        break;
    case SIGTERM:              // Termination request 
        sprintf( msg, " SIGTERM ");
        break;
    case SIGTSTP:               // 
        sprintf( msg, " SIGTSTP");
        break;
    case SIGXCPU:               // 
        sprintf( msg, " SIGXCPU");
        break;
    case SIGXFSZ:               // 
        sprintf( msg, " SIGXFSZ");
        break;
    case SIGSTOP:               // 
        sprintf( msg, " SIGSTOP ");
        break;
    case SIGSYS:               // 
        sprintf( msg, " SIGSYS ");
        break;
#ifndef __APPLE__
    case SIGSTKFLT:               // Stack fault
        cout << " SIGSTKFLT " << endl;
        break;
    case SIGPWR:               // 
        cout << " SIGPWR " << endl;
        break;
#endif
    default:
        sprintf( msg, " Uknown signal type: %d", sig);
        break;
    }
    if (sig!=0)
    {
        Logger->LogError( LastFile, LastLine, 'F', msg);
    }

    // User termination here

    // Finally close logfile.
    delete Logger;

    if (sig == 0)
    {
        _exit (0);
    }
    else
    {
        _exit (-1);
    }
}

/**
 ******************************************************************
 *
 * Function Name : Help
 *
 * Description : provides user with help if needed.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 *
 *******************************************************************
 */
static void Help(void)
{
    SET_DEBUG_STACK;
    cout << "********************************************" << endl;
    cout << "* Test file for text Logging.              *" << endl;
    cout << "* Built on "<< __DATE__ << " " << __TIME__ << "*" << endl;
    cout << "* Available options are :                  *" << endl;
    cout << "*                                          *" << endl;
    cout << "********************************************" << endl;
}
/**
 ******************************************************************
 *
 * Function Name :  ProcessCommandLineArgs
 *
 * Description : Loop over all command line arguments
 *               and parse them into useful data.
 *
 * Inputs : command line arguments. 
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
static void
ProcessCommandLineArgs(int argc, char **argv)
{
    int option;
    SET_DEBUG_STACK;
    do
    {
        option = getopt( argc, argv, "hHv");
        switch(option)
        {
        case 'h':
        case 'H':
            Help();
            Terminate(0);
            break;
        case 'v':
            Verbose = true;
            break;
	}
    } while(option != -1);
}
/**
 ******************************************************************
 *
 * Function Name : Initialize
 *
 * Description : Initialze the process
 *               - Setup traceback utility
 *               - Connect all signals to route through the terminate 
 *                 method
 *               - Perform any user initialization
 *
 * Inputs : none
 *
 * Returns : true on success. 
 *
 * Error Conditions : depends mostly on user code
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
static bool Initialize(void)
{
    SET_DEBUG_STACK;
    signal (SIGHUP , Terminate);  // Hangup.
    signal (SIGINT , Terminate);  // CTRL+C signal 
    signal (SIGKILL, Terminate);  // 
    signal (SIGQUIT, Terminate);  // 
    signal (SIGILL , Terminate);  // Illegal instruction 
    signal (SIGABRT, Terminate);  // Abnormal termination 
    signal (SIGIOT , Terminate);  // 
    signal (SIGBUS , Terminate);  // 
    signal (SIGFPE , Terminate);  // 
    signal (SIGSEGV, Terminate);  // Illegal storage access 
    signal (SIGTERM, Terminate);  // Termination request 
    signal (SIGSTOP, Terminate);   // 
#ifndef __APPLE__
    signal (SIGSTKFLT, Terminate); // 
    signal (SIGPWR, Terminate); // 
#endif
    signal (SIGSYS, Terminate); // 
    // User initialization goes here. 
    double version = 1.0;
    Logger = new CLogger("Navigation.log", "Navigation", version);

    return true;
}

/**
 ******************************************************************
 *
 * Function Name : main
 *
 * Description : It all starts here:
 *               - Process any command line arguments
 *               - Do any necessary initialization as a result of that
 *               - Do the operations
 *               - Terminate and cleanup
 *
 * Inputs : command line arguments
 *
 * Returns : exit code
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
int main(int argc, char **argv)
{
    SET_DEBUG_STACK;

    if (Initialize())
    {
	Geodetic *Geo = new Geodetic(41.308300018310547, -73.892997741699219);
	/*
	 * Online UTM result
	 * 592666.17 4573573.17
	 *
	 * TM result using central meridian of -73.892997741699219 	
	 * X: 0 Y: 5029710.11688301
	 * 
	 * Program test returns center of:
	 * Center: 0.0287539 5.02971e+06
	 * close enough. 
	 */
	cout << "RadToDeg: " << RadToDeg << endl;
	cout << "Lat: "      << Geo->CenterLat() * RadToDeg
	     << " Lon: "     << Geo->CenterLon() * RadToDeg
	     << " Center: " << Geo->XY0() 
	     << endl;

	/*
	 * Using these data -73.8 and 41.31 
	 *
	 * Online Result:
	 * X: 10352.4612508815
	 * Y: 5029961.09472725
	 *
	 * Test Result:
	 * X:  10352.5
	 * Y: 5.02996e+06
	 */
	Point p ( -73.8*DegToRad, 41.31*DegToRad, 0.0);
	Point q = Geo->ToXY(p);
	cout << "rev: " << q << endl;
    }
    Terminate(0);
}
