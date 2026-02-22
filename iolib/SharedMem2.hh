/********************************************************************
 *
 * Module Name : SharedMem2.hh
 *
 * Author/Date : C.B. Lirakis / 19-Sep-20
 *
 * Description : class for handling shared memory, POSIX upgrade 
 * This will create both a shared memory segment and a semaphore to 
 * go with it. 
 *
 * The SM segment name will be identical to the name provided by the user. 
 * The semaphore will be created with SEM_name
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 22-Feb-26   CBL    changed PutData to a const input since it is 
 *                    technically only copied into the SM segment 
 *                    and the memcpy is const
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
#ifndef __SHAREDMEM2_h_
#define __SHAREDMEM2_h_
#include <time.h>
#include <semaphore.h>
#include <CObject.hh>  // parent class

/**
 * This is the structure that is associated with the size of the
 * fixed header information with a specific shared memory segment.
 */
struct MyMemoryHeader {
    size_t  Length;         // Total bytes of User payload, may be zero. 
    struct timespec LastUpdateTime; // Last time SM was updated
    double DoubleData;              // Default Double Data.
    bool   LAM;     // Look at me. Set when we want client to take notice.
};
/**
 * Class to share data between applications. 
 *
 * Organization of shared memory:
 *     1) size_t   - length of allocated shared memory.
 *     2) timespec - time of last access, automatically updated on PutData 
 *     3) double   - Default user space, there even if 0 user bytes allocated. 
 *
 */
class SharedMem2 : public CObject
{

public:
    /*
     * Constructor. 
     * This is the default constructor and does not allocate any memory.
     */
    SharedMem2(void);
    /*
     * Destructor
     */
    virtual ~SharedMem2();
    /**
     * Constructor with a name for access identification. 
     * This one only allocates as much memory as specified by size.
     *
     * UserSize - number of bytes requested by user for their payload
     * Server - Set to true if I am creating the user space, false for
     *          attaching to existing user space. 
     */
    SharedMem2(const char *Name, size_t UserSize=0, bool Server=false, int DebugLevel = 0);

    /**
     * Updates the access time of the shared memory
     * and then returns a pointer to the 
     * area where we should start writing. 
     * Set KeepLocked to true if we want to write stuff after 
     * updating time. The user will be responsible for Unlock.
     */
    bool UpdateTime(bool KeepLocked = false);

    /**
     * Populate the double data in the header. 
     */
    bool   PutData(double d);
    /**
     * Get the double data in the header. 
     */
    double GetData(void);


    /**
     * Fill the user data segment. (Payload)
     * This takes in a pointer to the user data stream and copies it
     * to the shared memory segment. 
     */
    bool PutData(const void *UserData = 0);
    /**
     * Get a copy of the pointer to the user SM segment. 
     * The user is responsible for locks!
     */
    virtual void *GetData(void)  const {return fUserSM_Segment;};

    /**
     * The user specifies a pointer to the user segment where
     * they wish the shared memory to be copied to. 
     */
    bool          GetData(void *);

    /**
     * time difference between now (time called) and last shared memory update.
     * This should be negative if the SM has been touched before
     * we make this call. If positive, the SM hasn't been updated 
     * since return time value in seconds. 
     */
    double TimeSinceLastUpdate();
    /**
     * Time difference between the last check of SM and the 
     * current check of SM. 
     */
    double DeltaCheck();

    /**
     * If we are using the pointer directly, lock and unlock the usage. 
     */
    bool Lock(unsigned int Line=0);
    /**
     * If we are using the pointer directly, lock and unlock the usage. 
     */
    bool Unlock(unsigned int Line=0);

    /**
     * Set LAM in header. User provides true/false
     * true - new data is available
     * false - nothing or ACK 
     */
    void LAM(bool val);

    /**
     * Set LAM in header. 
     */
    inline void SetLAM(void) {LAM(true);};

    /**
     * Clear LAM
     */
    inline void ClearLAM(void) {LAM(false);};

    /**
     * Check LAM
     */
    bool GetLAM(void);


    inline void*  GetROSharedMemoryAddress() const { return fUserSM_Segment;};
    inline size_t GetLength() const {return fHeader->Length;};

    enum SM_Errors{NO_ERROR, NO_NAME, NO_OPEN, NO_RESIZE, NO_OBJECT, 
		   NO_VIEW, NO_MAP,
		   NO_SEMAPHORE, NO_SEMAPHORE_MAP, NO_CONNECT, NO_ATTACH,
		   NO_SEM_RELEASE, NO_MEMUNMAP, NO_CLOSE, NO_UNLINK};
private:
    /**
     * Name of shared memory. 
     */
    char* fSM_Name; 

    /**
     * File descriptor for opening and closing the shared memory for 
     * the user segment. 
     */
    int    fSMHandle;
    /**
     * Number of bytes requested for user segment. 
     */
    size_t fNumberUserBytes;

    /**
     * What was the time value last time we checked?
     */
    struct timespec fLastTime; // Last time SM was updated
    
    /**
     * Boolean - is this the server (the process that allocated the shared 
     * memory OR the client the process that attaches to it. 
     */
    bool fServer; 

    /**
     * Head of the shared memory segment. (Primary address from allocation.)
     * Address where the data size is stored.
     */
    void   *fHead;
    /**
     * This is the struture of the user header that desribes the payload.
     */
    struct MyMemoryHeader *fHeader;
    /*
     * This points to the area AFTER the time and length storage. 
     */
    unsigned char* fUserSM_Segment;

    // Semaphore data
    sem_t*         fSemaphoreHandle;

    /*
     * Helper functions.
     */
    /**
     * For the server part, create the shared memory segment
     * with the header information in it. 
     */
    bool CreateSpace(void);
    /**
     * For the Client part, attach to the shared memory segment
     * with the header information in it. 
     */
    bool AttachSpace(void);
    /** 
     * Server Clean up.
     */
    void CloseSpace(void);
    /**
     * Client side clean up. 
     */
    void DetachSpace(void);
};
#endif
