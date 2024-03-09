/**
 ******************************************************************
 *
 * Module Name : main.cpp
 *
 * Author/Date : C.B. Lirakis / 27-Feb-24
 *
 * Description : regression testing of various library funcitons. 
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
#include <cstring>
#include <cmath>
#include <csignal>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <cstdlib>
#include <random>

/// Local Includes.
#include "debug.h"
#include "tools.h"
#include "CLogger.hh"
#include "UserSignals.hh"
#include "Version.hh"
#include "SimpleCommand.hh"
#include "Timeout.hh"
#include "TimeUtil.hh"
#include "hist1D.hh"

/** Control the verbosity of the program output via the bits shown. */
static unsigned int VerboseLevel = 0;

/** Pointer to the logger structure. */
static CLogger   *logger;
static bool      Server = true;
static bool      run;
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
    cout << "*     -C client                            *" << endl;
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
        option = getopt( argc, argv, "CchHnv");
        switch(option)
        {
	case 'c':
	case 'C':
	    Server = false;
	    break;
        case 'h':
        case 'H':
            Help();
        Terminate(0);
        break;
	case 'v':
	    VerboseLevel = atoi(optarg);
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
    char   msg[32];
    double version;

    SetSignals();
    // User initialization goes here. 
    sprintf(msg, "%d.%d",MAJOR_VERSION, MINOR_VERSION);
    version = atof( msg);
    logger = new CLogger("xxx.log", "xxx", version);
    logger->SetVerbose(VerboseLevel);

    return true;
}

static void TimeoutTest(void)
{
    bool trun; 
    struct timespec sleeptime = {10L, 100000000L};
    if (Timeout(&trun, sleeptime))
    {
	cout << "Timeout established." << endl;
	while (trun)
	{
	    sleep(1);
	}
    }
    cout << "Terminate" << endl;
}
static void TestSimpleCommand(void)
{
    struct timespec sleeptime={0L, 100000000L};
    struct CommandStructure *cs;
    SimpleCommand *sc;
    time_t now;

    run = true;
    if(Server)
    {
	sc = new SimpleCommand(true);
	sleep(10);
	// test an ALL
	cout << "ALL NONE" << endl;
	sc->Command("ALL", SimpleCommand::kNONE);
	sleep(10);
	cout << "ALL Change file." << endl;
	sc->Command("ALL", SimpleCommand::kCHANGE_FILE_NAME);
	sleep(10);
	cout << "OTHER SHUTDOWN" << endl;
	sc->Command("OTHER", SimpleCommand::kSHUTDOWN);
	sleep(10);
	cout << "TEST SHUTDOWN" << endl;
	sc->Command("TEST", SimpleCommand::kSHUTDOWN);
    }
    else
    {
	sc = new SimpleCommand(false,"TEST");
	do {
	    cs = sc->Command(NULL, SimpleCommand::kNONE); // doesn't really matter
	    time(&now);

	    if (cs->Command != SimpleCommand::kINVALID)
	    {
		cout << " Command = 0x" << hex << cs->Command << ctime(&now);
		sc->Reset();
	    }
	    run = !(cs->Command == SimpleCommand::kSHUTDOWN);

	    nanosleep(&sleeptime, NULL);
	} while (run);
    }
}
static void TestMidnight(void)
{
    struct timespec sleeptime={0L, 250000000L};
    time_t current;
    run = true;
    TimeUtil tu;

    current = time(NULL);
    cout << "Start: " << ctime(&current);

    while(run)
    {
	//run = !tu->MidnightGMT();
	run = !tu.LocalTime(8,43,0);
	nanosleep(&sleeptime, NULL);
    }
    current = time(NULL);
    cout << "Finish: " << ctime(&current);

}
static void TestH1D(void)
{
    Hist1D *hid = new Hist1D("TestMe", 30, 0.0, 1.0);
    uint32_t N = 1000;
    double x, y;
    double sigma = 0.1;
    double sigma2 = sigma *sigma;
    double mean  = 0.5;
#if 0
    time_t now;
    time(&now);
    srand(now);
#else
    /*
     * https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
     *
     * make a normal distribution. 
     */
    std::random_device rd{};   // self explainatory
    std::mt19937 gen{rd()};    // helps mix the random numbers better
    std::normal_distribution d{mean,sigma};

#endif


    cout << "TEST H1D, filling with normal distribution." << endl;
    for (uint32_t i=0;i<N;i++)
    {
#if 0
	x = rand();
	x = x/((double) RAND_MAX);
	//cout << "X: " << x << endl;
	//y = Norm * exp( -0.5*pow(x-mean,2.0)/sigma2);
	//cout << y << endl;
#else
	y = d(gen);
#endif
	hid->Fill(y);
    }
    cout << "DONE" << endl;
    hid->Print(1);
    cout << *hid;
    hid->WriteJSON("test.json");
    delete hid;
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
    ProcessCommandLineArgs(argc, argv);
    if (Initialize())
    {
	//TestSimpleCommand();
	//TimeoutTest();
	//TestMidnight();
	TestH1D();
    }
    Terminate(0);
}
