/**
 ******************************************************************
 *
 * Module Name : Timeout.cpp
 *
 * Author/Date : C.B. Lirakis / 29-Feb-24
 *
 * Description : Timeout routines
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
//#include <cmath>
//#include <csignal>
#include <unistd.h>
#include <time.h>
#include <fstream>
#include <cstdlib>

/// Local Includes.
#include "Timeout.hh"
#include "debug.h"
#include "tools.h"

/*
 * Argument structure to pass into Timeout thread.
 */
struct TimeoutStruct {
    struct timespec fWaitTime;   // time to sleep before signaling.
    bool *fRun;                  // address pointer to parent flag to trip
};

void* TimeoutThread(void *arg)
{
    if (arg)
    {
	struct TimeoutStruct *to = (struct TimeoutStruct *)arg;
	nanosleep(&to->fWaitTime, NULL);
	*to->fRun = false;
    }
    return NULL;
}
bool Timeout(bool *RunFlag, const struct timespec &TimeToSleep)
{
    pthread_t pThread;
    static struct TimeoutStruct args;
    args.fRun      = RunFlag;
    args.fWaitTime = TimeToSleep;
    
    if( pthread_create(&pThread, NULL, TimeoutThread, &args) == 0)
    {
	*RunFlag = true;
    }
    else
    {
	*RunFlag = false;
	return false;
    }
    return true;
}
