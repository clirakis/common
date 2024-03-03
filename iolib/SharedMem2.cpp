/********************************************************************
 *
 * Module Name : sharedmem.cpp
 *
 * Author/Date : C.B. Lirakis / 19-Sep-20
 *
 * Description : Generic setup for shared memory, POSIX version
 *
 * This is going to be setup with 2 segments of shared memory. 
 * The first segment of shared memory will be fixed in size
 * and will act as a header for the user payload. This will have it's own 
 * semaphore associated with it. 
 * 
 * The second segment of shared memory will be the user payload if 
 * at creation time the user specifies that they need more than the header 
 * information. 
 *
 * The naming conventions shall be: 
 * Name - provided by the user. 
 * Name_Fixed - The name of the payload header. 
 * Name_Fixed_SEM - The name of the associated semaphore with the fixed header.
 *
 * Name - Name of payload if needed. 
 * Name_SEM - Name of accociated semaphore if needed. 
 *
 * Restrictions/Limitations : 
 *
 * Change Descriptions :
 * 19-Feb-24 Changed permissions from 644 to ? If i do this it may present 
 *           a security leak. 
 *
 * Classification : Unclassified
 *
 * References :
 * https://stackoverflow.com/questions/5658568/how-to-list-processes-attached-to-a-shared-memory-segment-in-linux
 * 
 *
 ********************************************************************/
// System includes.
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


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
#include <fcntl.h>
#include <sys/mman.h>
#include <cfloat>
#include <limits>

#include <sys/sem.h>
#include <errno.h>
#include <fstream>
#include <csignal>

// Local Includes.
#include "SharedMem2.hh"
#include "debug.h"
#include "CLogger.hh"
// 01-Nov-22
//static const int kPERM  = 0600;
static const int kPERM    = 0644;
static const size_t kSHM_Size = 4096; // Size of shared memory at initial 


/********************************************************************
 *
 * Function Name : SharedMem2 Constructor
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
SharedMem2::SharedMem2 (void) : CObject()
{
    SET_DEBUG_STACK;
    fUserSM_Segment  = NULL;
    fHeader          = NULL;
    fSMHandle        = 0;
    fServer          = false;
    fNumberUserBytes = 0;
    fSM_Name         = NULL;
    fHead            = NULL;

    SetName("SharedMem2");
    ClearError(__LINE__);
    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : Constructor CreateSharedMemory
 *
 * Description : This is used by the SERVER process and creates the
 * actual shared memory segment. Use like a malloc with a name. The
 * shared memory segment is composed of a header described by the 
 * structure MyMemoryHeader and the user segment where the user 
 * provides the layout. 
 *
 * Inputs :  Name - name of shared memory that the user wants to know it by
 *           UserSize - limited to 32 bits of address, number of
 *                      bytes allocated by user.
 *           Server   - if this is true, then this will create the shared 
 *                      memory, otherwise it 
 *
 * Returns : constructed class. 
 *
 * Error Conditions :
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
SharedMem2::SharedMem2( const char *Name, size_t UserSize, bool Server, int DebugLevel) : CObject()
{
    CLogger *pLogger = CLogger::GetThis();
    bool     rc;


    SetName("SharedMem2");
    SetDebug(DebugLevel);
    ClearError(__LINE__);
    fUserSM_Segment  = NULL;
    fHeader          = NULL;
    fSM_Name         = NULL;
    fSemaphoreHandle = NULL;
    fHead            = NULL;

    if (!Name)
    {
	SetError( NO_NAME, __LINE__);
	return;
    }

    fServer  = Server;
    fSM_Name = strdup(Name);
    fNumberUserBytes = UserSize;
    size_t AllocSize = sizeof(struct MyMemoryHeader) + fNumberUserBytes;

    if ((fDebug>0) && pLogger)
    {
	pLogger->LogError(__FILE__,__LINE__,'I',"SharedMem2 - Creation");
	pLogger->Log("# %s\n", strerror(errno));
	pLogger->Log("#Name: %s Header Size: %d User Size: %d Total Size %d\n", 
		     Name, sizeof(struct MyMemoryHeader), UserSize, AllocSize);
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
    if(fServer)
    {
	rc = CreateSpace();
    }
    else
    {
	rc = AttachSpace();
    }

    if (!rc)
    {
	// Error in last command. 
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

    if (Server)
    {
	/*
	 * Populate the header data. 
	 */
	fHeader->Length  = AllocSize;
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

	fHeader->DoubleData = 0.0;
    }
    else
    {
	fNumberUserBytes = fHeader->Length - sizeof(struct MyMemoryHeader);
    }
    fLastTime = fHeader->LastUpdateTime;


    SET_DEBUG_STACK;
}

/********************************************************************
 *
 * Function Name : SharedMem2 Destructor
 *
 * Description : Free up our resources. This assumes that we always use
 * the address of Length as the head of our shared memory segment
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions : Only the subordinate functions. 
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
SharedMem2::~SharedMem2 ()
{
    SET_DEBUG_STACK;
    ClearError(__LINE__);

    if (fServer)
    {
	CloseSpace();
    }
    else
    {
	DetachSpace();
    }

    free(fSM_Name);
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : CreateSpace
 *
 * Description : For the Server side, create the SM space
 *
 * Inputs : None
 *
 * Returns : True on success
 *
 * Error Conditions : 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::CreateSpace( void )
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int rc;
    char Working[256];

    ClearError(__LINE__);
    if ((fDebug>0) && pLogger)
    {
	pLogger->LogError(__FILE__,__LINE__,'I',"CreateFixedSMSpace");
    }

    size_t AllocSize = sizeof(struct MyMemoryHeader) + fNumberUserBytes;

    /*
     * Create the shared memory
     * 01-Nov-22 change how we open. 
     * Using O_EXCL causes an error to be thrown if used with O_CREAT
     * and the segment already exists. This can happen if the clean up
     * didn't happen appropriately. With O_EXCL 
     * if the segment existed, errno would equal EEXIST
     */
    //fSMHandle = shm_open( fSM_Name, O_RDWR | O_CREAT | O_EXCL, kPERM);
    fSMHandle = shm_open( fSM_Name, O_RDWR | O_CREAT , kPERM);
    
    if (fSMHandle == -1) 
    {
        fSMHandle = 0;
	if(pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'F', 
			      "SharedMem2 - Error shm_open.");
	    pLogger->LogError(__FILE__, __LINE__, 'F', 
			      strerror(errno));
	}
	SetError( NO_OPEN, __LINE__);
	return false;
    }
    else 
    {
        // The memory is created as a file that's 0 bytes long. Resize it.
        rc = ftruncate( fSMHandle, AllocSize);
        if (rc) 
	{
	    shm_unlink(fSM_Name);
	    fSMHandle = 0;
	    if(pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'F', 
			      "SharedMem2 - Error truncate.");
		pLogger->Log("# %s\n", strerror(errno));
	    }
	    SetError( NO_RESIZE, __LINE__);
	    return false;
        }
        else 
	{
            /*
	     *  MMap the shared memory. Arguments are:
	     *  address - NULL, let the kernel choose. 
	     *  length - 
	     *  prot - memory protection. 
	     *  flags -
	     *  file descriptor
	     *  offset 
	     */
            fHead = mmap((void *)0, AllocSize, 
			 PROT_READ | PROT_WRITE, MAP_SHARED, 
			 fSMHandle, 0);
            if (fHead == MAP_FAILED) 
	    {
                fHead = NULL;
		if(pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'F', 
				      "SharedMem2 - MMapping the shared memory failed");
		    pLogger->Log("# %s\n", strerror(errno));
		}
		SetError( NO_MAP, __LINE__);
		return false;
	    }
	    else 
	    {
		if ((fDebug>0)&& pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'I', 
				      "SharedMem2 - Shared Memory mapped.");
		}
	    }
        }
    }
    
    // We have successfuly created the shared memory. 
    // Create the semaphore
    sprintf(Working, "SEM_%s", fSM_Name);
    fSemaphoreHandle = sem_open(Working, O_CREAT, kPERM, 0);
    
    if (fSemaphoreHandle == SEM_FAILED) 
    {
	if(pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'F', 
			      "SharedMem2 - Semaphore failed");
	    pLogger->Log("# %s\n", strerror(errno));
	}   
	SetError( NO_SEMAPHORE, __LINE__);
	fSemaphoreHandle = NULL;
	return false;
    }
    /*
     * Opening the semaphore locks it!
     */
    Unlock(__LINE__);

    return true;
}


/**
 ******************************************************************
 *
 * Function Name : AttachSpace
 *
 * Description : For the Client portion, attach to existing SM space. 
 * memory segment. 
 *
 * Inputs : None
 *
 * Returns : True on success
 *
 * Error Conditions : 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::AttachSpace( void )
{

    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    char Working[256];
    ClearError(__LINE__);

    fSMHandle        = 0;
    fUserSM_Segment  = NULL;
    fServer          = false;
    memset(&fLastTime, 0, sizeof(struct timespec));
    fHead            = NULL;
    fSemaphoreHandle = NULL;

    if ((fDebug>0)&& pLogger)
    {
        pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "SharedMem2 - Client side.");
    }

    /* Attach to existing Semaphore. */
    sprintf(Working, "SEM_%s", fSM_Name);
    fSemaphoreHandle = sem_open( Working, 0);

    if ( fSemaphoreHandle == SEM_FAILED) 
    {
        fSemaphoreHandle = NULL;
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'F', 
			      "SharedMem2 - Error attaching to semaphore.");
	    pLogger->Log("# %s\n", strerror(errno));
	}
	SetError(NO_SEMAPHORE, __LINE__);
	return false;
    }
    else 
    {
	if ((fDebug>0)&& pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - Client side got semaphore.");
	    pLogger->Log("# %s\n", strerror(errno));
	}

        // get a handle to the shared memory
        fSMHandle = shm_open( fSM_Name, O_RDWR, kPERM);
        
        if (fSMHandle == -1) 
	{
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'F', 
				  "SharedMem2 - Error attaching to SM");
		pLogger->Log("# %s\n", strerror(errno));
	    }
	    SetError(NO_ATTACH, __LINE__);
	    return false;
        }
        else 
	{
            // mmap it. - HOW DO I DEAL WITH NEEDING TO KNOW THE SIZE in 
            // Advance. Try opening with minimum size, get the size 
	    // the user requested and then resize. 
	    size_t AllocSize = sizeof(struct MyMemoryHeader);
            fHead = mmap((void *)0, AllocSize, 
				 PROT_READ | PROT_WRITE, MAP_SHARED, 
			 fSMHandle, 0);
            if (fHead == MAP_FAILED) 
	    {
		SetError( NO_ATTACH, __LINE__);
		if(pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'F',
				      "SharedMem2 - failed mmap.");
		    pLogger->Log("# %s\n", strerror(errno));
		} 
		SET_DEBUG_STACK;
		return false;
            }
            else 
	    {
		if ((fDebug>0)&& pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - mmap succeeded, good to go!");
		}
	    }
	}
    }
    SET_DEBUG_STACK;

    return true;
}

/**
 ******************************************************************
 *
 * Function Name : CloseSpace
 *
 * Description : Close out everything associated with the shared 
 * memory space. (Server like)
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions : if freeing any of the allocated resources fails. 
 *            Can't allocate the semaphore. 
 *            Can't unmap the shared memory.
 *            Can't unlink the shared memory.
 *            Can't close the semaphore.
 *            Can't unlink the semaphore.
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SharedMem2::CloseSpace(void)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int      rc;
    char     Working[256];
    struct timespec SleepTime = {0L, 500000000L};
    ClearError(__LINE__);

    if ((fDebug>0)&& pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "SharedMem2 - Close Shared memory space.");
    }

    sprintf(Working, "SEM_%s", fSM_Name);
    /* 
     * Don't do this unless there is a semaphore. 
     * Not sure this is what we want to do. 
     */
    if (fSemaphoreHandle)
    {
	rc = sem_timedwait( fSemaphoreHandle, &SleepTime);
	//rc = sem_wait( fSemaphoreHandle);
	if (rc) 
	{
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'I', 
				  "SharedMem2 - sem_timedwait failed.");
	    }
	    else
	    {
		cout << "Acquiring the semaphore failed; errno: "
		     << errno << endl;
	    }
	    // This isn't necessarily a fail, it is just a timeout
	    // which may mean the semaphore wasn't locked. 
	}
    }

    /* Next step release the shared memory if it exists. */
    if (fHead) 
    {
	//printf("Destroying the shared memory.\n");
	// Un-mmap the memory...
	size_t AllocSize = sizeof(struct MyMemoryHeader) + fNumberUserBytes;

	rc = munmap( fHead, AllocSize);
	if (rc) 
	{
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'I', 
				  "SharedMem2 - munmap failed.");
	    }
	    else
	    {
		cout << "Unmapping the memory failed; errno is: " 
		     << errno << endl;
	    }
	    SetError( NO_MEMUNMAP, __LINE__);
	    return;
	}
                
	// ...close the file descriptor...
	if (-1 == close(fSMHandle)) 
	{
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'I', 
				  "SharedMem2 - munmap failed.");
	    }
	    else
	    {
		cout << "Closing the memory's file descriptor failed; errno: "
		     << errno << endl;
	    }
	    SetError( NO_CLOSE, __LINE__);
	    return;
	}

	// ...and destroy the shared memory.
	rc = shm_unlink(fSM_Name);
	if (rc) 
	{
	    if (pLogger)
	    {
		pLogger->LogError(__FILE__, __LINE__, 'I', 
				  "SharedMem2 - shm_unlink failed.");
	    }
	    else
	    {
		cout << "Unlinking the memory failed; errno: "
		     << errno << endl;
	    }
	    SetError( NO_UNLINK, __LINE__);
	    return;
        }

	if (fSemaphoreHandle)
	{
	    //printf("Destroying the semaphore.");
	    // Clean up the semaphore
	    rc = sem_close(fSemaphoreHandle);
	    if (rc) 
	    {
		if (pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'I', 
				      "SharedMem2 - sem_close failed.");
		}
		else
		{
		    cout << "Closing the semaphore failed; errno: "
			 <<  errno << endl;
		}
		SetError( NO_CLOSE, __LINE__);
		return;
	    } // sem_close
	    rc = sem_unlink(Working);
	    if (rc) 
	    {
		if (pLogger)
		{
		    pLogger->LogError(__FILE__, __LINE__, 'I', 
				      "SharedMem2 - sem_unlink failed.");
		}
		else
		{
		    cout << "Unlinking the semaphore failed; errno: "
			 << errno << endl;
		}
		SetError( NO_CLOSE, __LINE__);
		return;
	    } // sem_unlink
	}
    } // Acquire semaphore
}
/**
 ******************************************************************
 *
 * Function Name : DetachSpace
 *
 * Description : Detach from the shared memory. (Client Like)
 *
 * Inputs : None
 *
 * Returns : None
 *
 * Error Conditions :
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SharedMem2::DetachSpace(void)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int  rc;
    ClearError(__LINE__);

    if ((fDebug>0)&& pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "SharedMem2 - Detach Shared memory space.");
    }

    size_t AllocSize = sizeof(struct MyMemoryHeader) + fNumberUserBytes;
    // Un-mmap the memory
    rc = munmap( fHead, AllocSize);
    if (rc) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - munmap failed.");
	    return;
	}
	else
	{
	    cout << "Unmapping the memory failed; errno: " 
		 << errno << endl;
	}
	SetError( NO_MEMUNMAP, __LINE__);
	return;
    }
            
    // Close the shared memory segment's file descriptor            
    if (-1 == close( fSMHandle)) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - close failed.");
	    return;
	}
	else
	{
	    cout << "Closing memory's file descriptor failed; errno: "
		 << errno << endl;
	}
	SetError( NO_CLOSE, __LINE__);
	return;
    }

    rc = sem_close( fSemaphoreHandle);
    if (rc) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - close failed.");
	    return;
	}
	else
	{
	    cout << "Closing the semaphore failed; errno: "
		 <<  errno << endl;
	}
	SetError( NO_CLOSE, __LINE__);
	return;
    }
    SET_DEBUG_STACK;
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
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
double SharedMem2::TimeSinceLastUpdate(void)
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
	if (Lock(__LINE__))
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
        
	    NewTime.tv_sec   = CurrentTime.tv_sec - 
		fHeader->LastUpdateTime.tv_sec;
	    NewTime.tv_nsec  = CurrentTime.tv_nsec - 
		fHeader->LastUpdateTime.tv_nsec;

	    if (Unlock(__LINE__))
	    {
		rc  = (double) (NewTime.tv_nsec)/1.0E9;
		rc += (double) (NewTime.tv_sec);
		ClearError(__LINE__);
	    }
	    else
	    {
		rc = -1.0;
	    }
	}
	else
	{
	    rc = -1.0;
	}
    }
    SET_DEBUG_STACK;
    return rc;
}
/********************************************************************
 *
 * Function Name : DeltaCheck
 *
 * Description : returns the time since the last we checked for
 *               a shared memory update
 *
 * Inputs : none
 *
 * Returns : time difference between last check SM time and current SM time.
 *
 * Error Conditions : none
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
double SharedMem2::DeltaCheck(void)
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
	if (Lock(__LINE__))
	{

	    NewTime.tv_sec   = fHeader->LastUpdateTime.tv_sec - 
		fLastTime.tv_sec;
	    NewTime.tv_nsec  = fHeader->LastUpdateTime.tv_nsec - 
		fLastTime.tv_nsec;
	    if(Unlock(__LINE__))
	    {
		rc  = (double) (NewTime.tv_nsec)/1.0E9;
		rc += (double) (NewTime.tv_sec);
		fLastTime = fHeader->LastUpdateTime;
		ClearError(__LINE__);
	    }
	    else
	    {
		rc = -1.0;
	    }
	}
    }
    SET_DEBUG_STACK;
    return rc;
}
/********************************************************************
 *
 * Function Name : UpdateTime
 *
 * Description : This version just updates the LastUpdateTime 
 * variable without actually putting any data into SM. 
 *
 * Inputs : none
 *
 * Returns : true on success. 
 *
 * Error Conditions : semaphores couldn't be locked/unlocked. 
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
bool SharedMem2::UpdateTime(bool KeepLocked)
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
        return false;
    }

    if (Lock(__LINE__))
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
	if (!KeepLocked)
	{
	    if(!Unlock(__LINE__))
	    {
		SET_DEBUG_STACK;
		return false;
	    }
	}
    }
    SET_DEBUG_STACK;
    return true;
}

/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : Put the double data in the header. 
 *
 * Inputs : double value to put into fixed header. 
 *
 * Returns : true on success. 
 *
 * Error Conditions : if the semaphores fail.
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
bool SharedMem2::PutData(double Val)
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
        return false;
    }
    /* Update time will check the semaphores etc. */
    if (UpdateTime())
    {
	fHeader->DoubleData = Val;
	fHeader->LAM = true;
	if(!Unlock(__LINE__))
	{
	    SET_DEBUG_STACK;
	    return false;
	}
    }
    else
    {
	SET_DEBUG_STACK;
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : Put the double data in the header. 
 *
 * Inputs : none
 *
 * Returns : Double data stored in header. 
 *
 * Error Conditions : Can't get semaphores. 
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
double SharedMem2::GetData(void)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);
    double rc = -DBL_MAX;

    if (!fHead)
    {
	SetError( NO_OBJECT, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__,'W',
			      "Failed to get data, no header pointer NULL!");
	}
	SET_DEBUG_STACK;
        return rc;
    }

    if (Lock())
    {
	rc = fHeader->DoubleData ;
	//fHeader->LAM = false; // We picked it up. leave this to the user
	if(!Unlock(__LINE__))
	{
	    SET_DEBUG_STACK;
	    return -DBL_MAX;
	}
    }
    else
    {
	SET_DEBUG_STACK;
    }
    return rc;
}


/********************************************************************
 *
 * Function Name : PutData
 *
 * Description : Put the user payload. 
 *               Copy ptr of user data into system. This is a little
 *               bit of a bad formulation since it is possible to have 
 *               ptr not be of the correct size. 
 *
 * Inputs : Pointer to user data stream. 
 *
 * Returns : true on success. 
 *
 * Error Conditions : Semaphores fail. 
 *
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 ********************************************************************/
bool SharedMem2::PutData(void *UserData)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    ClearError(__LINE__);

    if (UserData == 0)
    {
	return true; // Don't care. 
    }

    if (!fHead)
    {
	SetError( NO_OBJECT, __LINE__);
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__,'W',
			      "Failed to put data, no header pointer NULL!");
	}
	SET_DEBUG_STACK;
        return false;
    }

    if (fHeader->Length > 0)
    {
	if (UpdateTime(true))
	{
	    if (fNumberUserBytes>0)
	    {
		memcpy( fUserSM_Segment, UserData, fNumberUserBytes);
		fHeader->LAM = true;
	    }
	    if (!Unlock(__LINE__))
	    {
		SET_DEBUG_STACK;
		return false;
	    }
	}
	else
	{
	    SET_DEBUG_STACK;
	    return false;
	}
    }

    SET_DEBUG_STACK;
    return true;
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
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::GetData( void *data)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int rv;

    ClearError(__LINE__);
    if ((fDebug>1) && pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__,'I',"GetData." );
    }

    rv = sem_wait( fSemaphoreHandle);
    if (rv) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - semwait failed.");
	}
	else
	{
	    cout << "Acquiring the semaphore failed; errno: "
		 <<  errno << endl;
	}
	SetError( NO_SEMAPHORE, __LINE__);
	return false;
    }

    /*
     * Perform some checks to see that we are ok to copy.
     */
    if ((data) && ( fNumberUserBytes>0) && (fUserSM_Segment != NULL))
    {
        memcpy ( data,  fUserSM_Segment, fNumberUserBytes);
	ClearError(__LINE__);
    }

    rv = sem_post( fSemaphoreHandle);
    if (rv) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, __LINE__, 'I', 
			      "SharedMem2 - sempost failed.");
	}
	else
	{
	    cout << "Releasing the semaphore failed; errno: "
		 << errno << endl;
	}
	SetError( NO_SEMAPHORE, __LINE__);
    }
    
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : Lock
 *
 * Description : Lock the semaphore associated with this 
 *               shared memory object. 
 *
 * Inputs : None
 *
 * Returns : true on success
 *
 * Error Conditions : When acquring the semaphore fails. 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::Lock(unsigned int Line)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int rv;
    ClearError(__LINE__);

    if ((fDebug>1)&& pLogger)
    {
	pLogger->LogError(__FILE__, Line, 'I', 
			  "SharedMem2 - Lock");
    }
    rv = sem_wait( fSemaphoreHandle);
    if (rv) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, Line, 'I', 
			      "SharedMem2 - semwait failed.");
	}
	else
	{
	    cout << "Acquiring the semaphore failed; errno: "
		 <<  errno << endl;
	}
	SetError( NO_SEMAPHORE, __LINE__);
	return false;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : Unlock
 *
 * Description : Unlock the semaphore associated with this 
 *               shared memory object. 
 *
 * Inputs : None
 *
 * Returns : true on success
 *
 * Error Conditions : When unlocking the semaphore fails. 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::Unlock(unsigned int Line)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int rv;

    ClearError(__LINE__);

    if ((fDebug>2)&& pLogger)
    {
	pLogger->LogError(__FILE__, Line, 'I', 
			  "SharedMem2 - Unlock");
    }
    rv = sem_post( fSemaphoreHandle);
    if (rv) 
    {
	if (pLogger)
	{
	    pLogger->LogError(__FILE__, Line, 'I', 
			      "SharedMem2 - sempost failed.");
	}
	else
	{
	    cout << "Releasing the semaphore failed; errno: "
		 << errno << endl;
	}
	SetError( NO_SEMAPHORE, __LINE__);
    }
    
    SET_DEBUG_STACK;

    return true;
}
/**
 ******************************************************************
 *
 * Function Name : SetLAM
 *
 * Description : Set the Look-At-Me in the header.
 *
 * Inputs : None
 *
 * Returns : true on success
 *
 * Error Conditions : When unlocking the semaphore fails. 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void SharedMem2::LAM(bool val)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();

    ClearError(__LINE__);

    if ((fDebug>2)&& pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "SharedMem2 - SetLAM");
    }
    
    if(Lock(__LINE__))
    {
	fHeader->LAM = val;
	Unlock(__LINE__);
    }
    
    SET_DEBUG_STACK;
}


/**
 ******************************************************************
 *
 * Function Name : GetLAM
 *
 * Description : return true if LAM is set false if not. 
 *
 * Inputs : None
 *
 * Returns : true on success
 *
 * Error Conditions : When unlocking the semaphore fails. 
 * 
 * Unit Tested on: 20-Sep-20
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool SharedMem2::GetLAM(void)
{
    SET_DEBUG_STACK;
    CLogger *pLogger = CLogger::GetThis();
    int rc = false;

    ClearError(__LINE__);

    if ((fDebug>2)&& pLogger)
    {
	pLogger->LogError(__FILE__, __LINE__, 'I', 
			  "SharedMem2 - GetLAM");
    }
    if(Lock(__LINE__))
    {
	rc = fHeader->LAM;
	Unlock(__LINE__);
    }
    
    SET_DEBUG_STACK;
    return rc;
}

