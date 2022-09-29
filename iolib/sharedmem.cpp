/********************************************************************
 *
 * Module Name : sharedmem.cpp
 *
 * Author/Date : C.B. Lirakis / 15-May-00
 *
 * Description : Generic setup for shared memory, IRIX version
 * 26-Feb-06 set it up so that all directories are subbed to /tmp/sharedmem
 * 15-Mar-06 set it up so that all directories are subbed to /usr/local/sm
 * 25-Nov-17 setup to use CLogger if available. 
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>


#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include <sys/sem.h>
#include <errno.h>
#include <fstream>
#include <csignal>

// Local Includes.
#include "sharedmem.hh"
#include "debug.h"
#include "CLogger.hh"

//const char *TopDir = "/tmp/sharedmem";
const char*  TopDir = "/usr/local/sm";
const int    Perms  = 0666;
//const size_t ErrorStringSize = 256;

/********************************************************************
 *
 * Function Name : SharedMem Constructor
 *
 * Description : This is the null constructor and isn't useful at all. 
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions : None
 *
 ********************************************************************/
SharedMem::SharedMem (void) : CObject()
{
    SET_DEBUG_STACK;
    fUserSM_Segment  = NULL;
    fHeader          = NULL;
    fSMHandle        = 0;
    fNumberUserBytes = 0;
    SetName("SharedMem");
    ClearError(__LINE__);
    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : Constructor CreateSharedMemory
 *
 * Description : use like a malloc with a name. At this time
 * both a unsigned long (size of shared memory allocated) and 
 * a struct timespec (last time accessed) are put into the 
 * shared memory portion. 
 *
 * Inputs :  name of shared memory 
 *           UserSize - limited to 32 bits of address, number of
 *                      bytes allocated by user.
 *
 * Returns : 
 *
 * Error Conditions :
 *
 ********************************************************************/
SharedMem::SharedMem( char Letter, size_t UserSize, const char *PrefixDir) : CObject()
{
    const  int  SGetFlags = Perms | IPC_CREAT;
    struct stat filestatus;
    char        Dirpath[256];
    char        msg[512];
    key_t       key;
    //mode_t      mode;
    CLogger *pLogger = CLogger::GetThis();
#if 0
    struct shmid_ds perms;
    int        rv;
#endif
    SET_DEBUG_STACK;
    SetName("SharedMem");
    ClearError(__LINE__);
    fUserSM_Segment = NULL;
    fHeader         = NULL;

    /*
     * Before we start, make sure the directory TopDir exists. 
     * if not, create it. 
     */
    if ((fDebug>1)&& pLogger)
    {
        pLogger->LogError(__FILE__, __LINE__, 'I', "Checking topdir.");
    }
    if (stat (TopDir, &filestatus) < 0)
    {
        if ((fDebug>1) && pLogger)
        {
            pLogger->LogTime("Directory: %s Doesn't exist, creating it.\n",
			     TopDir );
        }
        /*
         * I would prefer 0002, but I need to setup better groups for that.
         */
        //mode = umask(0000);
        // Directory does not exist. 
        if (mkdir( TopDir, 0777) < 0)
        {
	    SetError( NO_KEY_OR_DIRECTORY, __LINE__);
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'W', 
				  "Make directory failed.");
	    }
            SET_DEBUG_STACK;
            return;
        }
    }

    if ((fDebug>1) && pLogger)
    {
        pLogger->LogTime("Checking specific directory.\n");
    }

    /*
     * Before we start, make sure the directory PrefixDir exists. 
     * if not, create it. 
     */
    sprintf( Dirpath, "%s/%s", TopDir, PrefixDir);
    if (stat (Dirpath, &filestatus) < 0)
    {
        if ((fDebug>1) && pLogger)
        {
            pLogger->LogTime("Directory: %s Doesn't exist, creating it.\n",
			     Dirpath);
        }
        // Directory does not exist. 
        if (mkdir( Dirpath, 0777) < 0)
        {
	    SetError( NO_KEY_OR_DIRECTORY, __LINE__);
	    if (pLogger)
	    {
		snprintf(msg,sizeof(msg), 
			 "Failed to create directory: %s",Dirpath); 
		pLogger->LogError(__FILE__, __LINE__,'W',msg);
	    }
            SET_DEBUG_STACK;
            return;
        }
    }

    // Shared memory tool for creating a unique key. 
    key = ftok(Dirpath, Letter);
    if ( key < 0)
    {
	SetError( NO_KEY_OR_DIRECTORY, __LINE__);
	sprintf(msg,"Failed to create key: %d",key);
	pLogger->LogError(__FILE__, __LINE__,'W',msg);

        SET_DEBUG_STACK;
        return;
    }

    /*
     * Organization of shared memory. 
     * Add on additional size to allow for time storage. 
     * timespec - time of last access. 
     * size_t   - length of allocated shared memory.
     * sizeof double and float as tack on. 
     * 
     * size is the all
     */
    size_t AllocSize = sizeof(struct MyMemoryHeader) + UserSize;

    /* 
     * Create the shared memory. 
     * A return code of -1 is an error.
     */
    fSMHandle = shmget( key, AllocSize, SGetFlags);
    //SMHandle = shmget( IPC_PRIVATE, AllocSize, SGetFlags);
    if (fSMHandle < 0) 
    {
	SetError( NO_OBJECT, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__,'W',"Failed on shmemget."); 
	}
        SET_DEBUG_STACK;
        return;
    }
#if 0
    /* 05-Jul-08*/
    rv = shmctl( fSMHandle, IPC_STAT, &perms);
    cout << "RV: " << rv 
         << " " << perms.shm_segsz
         << " ID " << fSMHandle
         << " key " << key
         <<endl;
#endif
    /*
     * Now, let us attach to the memory.
     * Head is the beginning of the entire segment. 
     */
    fHead = shmat(fSMHandle, 0, SHM_R | SHM_W);
    if (fHead == (void *) -1) 
    { 
	SetError( NO_VIEW, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__,__LINE__,'W',"Failed on shmat."); 
	}
        fUserSM_Segment = NULL;
        SET_DEBUG_STACK;
        return;
    }
    /*
     * Start with SMA at head. 
     */
    fUserSM_Segment =  (unsigned char *)fHead;
    /* 
     * Hook up the header that identifies the SM.
     *
     * The variable Length is a pointer to this part of SM. 
     * It should not change during at all during operation of this
     * CSU.
     */
    fHeader = (struct MyMemoryHeader *) fUserSM_Segment;
    /*
     * Increment location by the size we just used. 
     */
    fUserSM_Segment += sizeof(struct MyMemoryHeader);

    /*
     * Populate the header data. 
     */
    fHeader->Length = AllocSize;
#ifdef MAC
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fHeader->LastUpdateTime.tv_sec  = tv.tv_sec;
    fHeader->LastUpdateTime.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fHeader->LastUpdateTime);
#endif
    fLastTime = fHeader->LastUpdateTime;
    fHeader->FloatData  = 0.0;
    fHeader->DoubleData = 0.0;
    fNumberUserBytes    = UserSize;

    // So far, so good, need semaphore now. 
    fSemKey = ftok( Dirpath, 'B');
    if ( fSemKey < 0)
    {
	SetError( NO_SEMAPHORE, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__,__LINE__,'W', 
			      "Failed, sempaphore does not exist."); 
	}
        SET_DEBUG_STACK;
        return;
    }

    /* 
     * Create the semaphore for the shared memory. 
     * Use same flags as the shared memory segment. 
     * We want a single semaphore.
     */
    fSemaphoreHandle = shmget( fSemKey, sizeof(long), SGetFlags);
    if (fSemaphoreHandle == -1)
    {
	SetError( NO_SEMAPHORE_MAP, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__,__LINE__,'W',
			      "Failed, map does not exist."); 
	}
        SET_DEBUG_STACK;
        return;
    }
    /*
     * Now setup the semaphore to do what we want. 
     * in this particular case it is a boolean lock value. 
     * to prevent both processes from writing to the same location
     * at the same time. 
     */
    semctl( fSemaphoreHandle, 1, SETVAL, 1);
    SET_DEBUG_STACK;
}

/********************************************************************
 *
 * Function Name : SharedMem Destructor
 *
 * Description : Free up our resources. This assumes that we always use
 * the address of Length as the head of our shared memory segment
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
SharedMem::~SharedMem ()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);
    if (fSMHandle > -1) 
    {
        if (fHead != NULL) 
        {
            shmdt(fHead);
        }
    }
    SET_DEBUG_STACK;
}

/********************************************************************
 *
 * Function Name : Constructor 
 *
 * Description : Attach To Exisiting Shared Memory
 *
 * Inputs : Name to attach to. 
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
SharedMem::SharedMem( char Letter, const char *PrefixDir)
{
    SET_DEBUG_STACK;
    const int SGetFlags = Perms;
    char      Dirpath[256];
    char      msg[512];   // These are way too long. fix. 
    key_t     key;
    CLogger *pLogger = CLogger::GetThis();
    
    SetName("SharedMem");
    ClearError(__LINE__);

    fUserSM_Segment = NULL;
    sprintf( Dirpath, "%s/%s", TopDir, PrefixDir);

    key = ftok( Dirpath, Letter);
    ClearError(__LINE__);

    /* 
     * Get existing shared memory. 
     * A return code of -1 is an error.
     */
    fSMHandle = shmget( key, 0, SGetFlags);
    if (fSMHandle < 0) 
    { 
	SetError( NO_CONNECT, __LINE__);
	if(pLogger)
	{
	    sprintf(msg, "Failed to connect to shared memory. %s", 
		    Dirpath );
	    pLogger->LogError(__FILE__, __LINE__, 'W', msg); 
	}
        SET_DEBUG_STACK;
        return;
    } 

    // Attach to SM
    fHead = shmat(fSMHandle, 0, SHM_R | SHM_W);
    if (fHead == 0) 
    { 
	SetError( NO_ATTACH, __LINE__);
	if(pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'W',
			      "Failed to attach to shared memory.");
	} 
        SET_DEBUG_STACK;
        return;
    }

    fUserSM_Segment  = (unsigned char*) fHead;
    fHeader          = (struct MyMemoryHeader *) fUserSM_Segment;
    fNumberUserBytes = fHeader->Length - sizeof(struct MyMemoryHeader);
    /*
     * Increment location by the size we just used. 
     */
    fUserSM_Segment += sizeof(struct MyMemoryHeader);

    // So far, so good, need semaphore now.     
 
    fSemKey = ftok( Dirpath, 'B');
    /* 
     * Create the semaphore for the shared memory. 
     * Use same flags as the shared memory segment. 
     * We want a single semaphore.
     */
    fSemaphoreHandle = shmget( fSemKey, 1, SGetFlags);
    if (fSemaphoreHandle == -1)
    {
	SetError( NO_SEMAPHORE, __LINE__);
	if(pLogger)
	{
	    sprintf(msg, "Failed to attach to semaphore. %s",
		    Dirpath );
	    pLogger->LogError(__FILE__, __LINE__, 'W',	msg);
	}
        SET_DEBUG_STACK;
        return;
    }
    fLastTime = fHeader->LastUpdateTime;

    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : RWData
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool SharedMem::RWData(bool Read_WriteNot)
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

#if 0
    if (Read_WriteNot) 
    {
        // In the event that this is a read we don't have
        // to have semaphore blocking. 
        DataEntryPoint(Read_WriteNot);

    } else {
        CMutex A(false,MutexName.c_str(), NULL);
        // Try for 500 ms to get the flag.
        if (A.Lock(500) != 0) 
        {
            // Update the time we wrote to the memory.
            _ftime( (struct _timeb *)LastUpdateTime );
            // Update data from user. 
            DataEntryPoint(Read_WriteNot);

            // The FlushViewOfFile function copies the specified number 
            // of bytes of the file view to the physical file, without 
            // waiting for the cached write operation to occur: 
            if (!FlushViewOfFile(((LPDWORD) SharedMemoryAddress), 0)) { 
                fLastError = "Could not flush memory to disk."; 
                return false;
            }
            A.Unlock();
        } 
        else 
        {
            fLastError = "Timed out preforming write to shared memory."; 
            return false;
        }
    }
#endif
    SET_DEBUG_STACK;
    return true;
}
/********************************************************************
 *
 * Function Name : TimeSinceLastUpdate
 *
 * Description : returns the time since the last shared memory update
 *               with millisecond resolution and the current clock time.
 *
 * Inputs : none
 *
 * Returns : time difference between now and last shared memory update.
 *
 * Error Conditions : none
 *
 ********************************************************************/
double SharedMem::TimeSinceLastUpdate()
{
    SET_DEBUG_STACK;
    struct timespec CurrentTime, NewTime;
    double rc = 0.0;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);

    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "Time since last update no header.");
    }

    if (fHeader)
    {
 #ifdef MAC
       struct timeval tv;
       struct timezone tz;
       struct tm *tm;
       gettimeofday (&tv, &tz);
       tm = gmtime(&tv.tv_sec);
       CurrentTime.tv_sec  = tv.tv_sec;
       CurrentTime.tv_nsec = tv.tv_usec * 1000; 
#else
       clock_gettime(CLOCK_REALTIME, &CurrentTime);
#endif
        
        NewTime.tv_sec   = CurrentTime.tv_sec -fHeader->LastUpdateTime.tv_sec;
        NewTime.tv_nsec  = CurrentTime.tv_nsec-fHeader->LastUpdateTime.tv_nsec;
        rc  = (double) (NewTime.tv_nsec)/1.0E9;
        rc += (double) (NewTime.tv_sec);
	ClearError(__LINE__);
    }
    SET_DEBUG_STACK;
    return rc;
}
/********************************************************************
 *
 * Function Name : TimeSinceLastUpdate
 *
 * Description : returns the time since the last shared memory update
 *               and the last time we checked the SM time. 
 *
 * Inputs : none
 *
 * Returns : time difference between last SM time and current SM time.
 *
 * Error Conditions : none
 *
 ********************************************************************/
double SharedMem::DeltaCheck()
{
    SET_DEBUG_STACK;
    struct timespec NewTime;
    CLogger *pLogger = CLogger::GetThis();
    double rc = 0.0;

    ClearError(__LINE__);
    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__,__LINE__,'I',"Delta Check no header.");
    }
    if (fHeader)
    {
        NewTime.tv_sec   = fHeader->LastUpdateTime.tv_sec - fLastTime.tv_sec;
        NewTime.tv_nsec  = fHeader->LastUpdateTime.tv_nsec - fLastTime.tv_nsec;
        rc  = (double) (NewTime.tv_nsec)/1.0E9;
        rc += (double) (NewTime.tv_sec);
        fLastTime = fHeader->LastUpdateTime;
	ClearError(__LINE__);
    }
    SET_DEBUG_STACK;
    return rc;
}
/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : This version just updates the LastUpdateTime 
 * variable without actually putting any data into SM. 
 *
 * Inputs : none
 *
 * Returns : false if initialization went poorly. 
 *
 * Error Conditions : NONE
 *
 ********************************************************************/
void *SharedMem::PutData()
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);

    if (!fHead)
    {
	SetError( NO_OBJECT, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__,'W',
			      "Failed to put data, no header payload!");
	}
	SET_DEBUG_STACK;
        return 0;
    }
#ifdef MAC
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fHeader->LastUpdateTime.tv_sec  = tv.tv_sec;
    fHeader->LastUpdateTime.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fHeader->LastUpdateTime);
#endif
    SET_DEBUG_STACK;
    return fUserSM_Segment;
}

/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : Quick access for the default Float and Double data
 * in the shared memory. Also returns the pointer to the User portion 
 * of data. This assumes that the constructor was called with UserSize>0.
 * otherwise the pointer is of no use. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
void *SharedMem::PutData(double f, float d, void *UserData)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);

    if (!fHead)
    {
	SetError( NO_OBJECT, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__,'W',
			      "Failed to put data, no header pointer NULL!");
	}
	SET_DEBUG_STACK;
        return 0;
    }
#ifdef MAC
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fHeader->LastUpdateTime.tv_sec  = tv.tv_sec;
    fHeader->LastUpdateTime.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fHeader->LastUpdateTime);
#endif
    fHeader->DoubleData = f;
    fHeader->FloatData  = d;
    if (fHeader->Length > 0)
    {
        if ((UserData) && (fNumberUserBytes>0))
        {
            memcpy( fUserSM_Segment, UserData, fNumberUserBytes);
        }
	ClearError(__LINE__);
        SET_DEBUG_STACK;
        return fUserSM_Segment;
    }
    SET_DEBUG_STACK;
    return 0;
}
/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : Copy ptr of user data into system. This is a little
 * bit of a bad formulation since it is possible to have ptr not
 * be of the correct size. 
 *
 * Inputs : ptr - void data to copy into USER SM.
 *
 * Returns : true on success
 *
 * Error Conditions : ptr NULL, SM not allocated, or number user bytes
 *                    zero.
 *
 ********************************************************************/
bool SharedMem::CopyAndPutUserData( const void *ptr)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);
    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I',"CopyAndPutUserData.");
    }

    if ((ptr) && (fNumberUserBytes>0) && (fUserSM_Segment != NULL))
    {
 #ifdef MAC
        struct timeval tv;
        struct timezone tz;
        struct tm *tm;
        gettimeofday (&tv, &tz);
        tm = gmtime(&tv.tv_sec);
        fHeader->LastUpdateTime.tv_sec  = tv.tv_sec;
        fHeader->LastUpdateTime.tv_nsec = tv.tv_usec * 1000; 
#else
        clock_gettime(CLOCK_REALTIME, &fHeader->LastUpdateTime);
#endif
        memcpy( fUserSM_Segment, ptr,  fNumberUserBytes);
	ClearError(__LINE__);
        SET_DEBUG_STACK;
        return true;
    }
    SET_DEBUG_STACK;
    return false;
}
/**
 ******************************************************************
 *
 * Function Name : GetAndCopyUserData
 *
 * Description :
 *
 * Inputs :  data - Pointer to user data segment
 *           size - size of user data segment
 *
 * Returns : true on success. 
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
bool SharedMem::GetAndCopyUserData( void *data, size_t size)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    bool rc       = false;
    size_t tocopy = 0;

    ClearError(__LINE__);
    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__,__LINE__,'I',"CopyAndPutUserData");
    }
    fLastTime = fHeader->LastUpdateTime;

    /*
     * Perform some checks to see that we are ok to copy.
     */
    if ((data) && ( fNumberUserBytes>0) && (fUserSM_Segment != NULL))
    {
        if (size ==  fNumberUserBytes)
        {
            tocopy = fNumberUserBytes;
        }
        else if (size < fNumberUserBytes)
        {
            tocopy = size;
        }
        else
        {
            tocopy =  fNumberUserBytes;
        }
        memcpy ( data,  fUserSM_Segment, tocopy);
        rc = true;
	ClearError(__LINE__);
    }
    SET_DEBUG_STACK;
    return rc;
}
/**
 ******************************************************************
 *
 * Function Name : GetData
 *
 * Description : this particular method copies the data out of sm
 * into the user space.
 *
 * Inputs :  data - Pointer to user data segment
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
void SharedMem::GetData( void *data)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();

    ClearError(__LINE__);
    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__,'I',"GetData." );
    }
    /*
     * Perform some checks to see that we are ok to copy.
     */
    if ((data) && ( fNumberUserBytes>0) && (fUserSM_Segment != NULL))
    {
        memcpy ( data,  fUserSM_Segment, fNumberUserBytes);
	ClearError(__LINE__);
    }
    SET_DEBUG_STACK;
}
void SharedMem::GetData(double *d, double *f, void *p)
{
    SET_DEBUG_STACK;
    if (d)
    {
        *d = fHeader->DoubleData;
    }
    if (f)
    {
        *f = fHeader->FloatData;
    }
    if (p)
    {
        GetData(p);
    }
    SET_DEBUG_STACK;
}
