/********************************************************************
 *
 * Module Name : AmIRunning.cpp
 *
 * Author/Date : C.B. Lirakis / 26-Dec-10
 *
 * Description : Base module for CBL library set.
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
#include <ostream>
using namespace std;
#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>

/// Local Includes.
#include "debug.h"
#include "AmIRunning.hh"

static sem_t     *mysem;
static char      *UName;

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
bool AmIRunning(const char *name)
{   
    UName = strdup(name);
    mysem = sem_open( UName, O_CREAT|O_EXCL);
    if ( mysem == NULL )
    {      
        return true;
    }  
    return false;
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
void AmIRunningCleanUp()
{
    sem_unlink(UName);
    sem_close(mysem);
    if (UName) free(UName);
}

