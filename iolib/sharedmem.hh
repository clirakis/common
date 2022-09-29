/********************************************************************
 *
 * Module Name : sharedmem.hh
 *
 * Author/Date : C.B. Lirakis / 23-May-00
 *
 * Description : class for handling shared memory. 
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
#ifndef __SHAREDMEM_h_
#define __SHAREDMEM_h_
#include <time.h>
#include <semaphore.h>
#include <CObject.hh>  // parent class

struct MyMemoryHeader {
    unsigned short Length;          // Total bytes of data allocated. 
    struct timespec LastUpdateTime; // Last time SM was updated
    float  FloatData;               // Default Float Data.
    double DoubleData;              // Default Double Data.
};
/**
 * Class to share data between applications. 
 *
 * Organization of shared memory:
 *     1) size_t   - length of allocated shared memory.
 *     2) timespec - time of last access, automatically updated on PutData 
 *     3) double   - Default user space, there even if 0 user bytes allocated. 
 *     4) float    - Default user space, there even if 0 user bytes allocated.
 *
 */
class SharedMem : public CObject 
{

public:
    /*
     * Constructor. 
     * This is the default constructor and does not allocate any memory.
     */
    SharedMem(void);
    /*
     * Destructor
     */
    virtual ~SharedMem();
    /**
     * Constructor with a letter for access identification. 
     * use /tmp as the base directory and the letter specified
     * as the key mechanism. Used to attach to existing SM. 
     */
    SharedMem(char Letter, const char *Basedir = "/tmp");
    /**
     * Constructor with a letter for access identification. 
     * Use /tmp as the base directory and the letter specified
     * as the key mechanism. This one only allocates 
     * as much memory as specified by size.
     *
     * UserSize - number of bytes requested by user.
     */
    SharedMem(char Letter, size_t UserSize=0, const char *Basedir = "/tmp");

    /*
     */
    bool RWData(bool Read_WriteNot);
    /**
     * Updates the access time of the shared memory
     * and then returns a pointer to the 
     * area where we should start writing. 
     */
    virtual void *PutData();
    // Put only the built in double argument
    inline  void *PutData(double d) {return PutData( d, 0.0, 0);};
    // Put only the built in float argument. 
    inline  void *PutData(float d) {return PutData( 0.0, d, 0);};
    // Put both builtins in SM. 
    virtual void *PutData(double d, float f, void *UserData = 0);

    virtual void *GetData()  const {return fUserSM_Segment;};
    inline double GetDataD() const {return fHeader->DoubleData;};
    inline float  GetDataF() const {return fHeader->FloatData;};
    void          GetData(void *);
    void          GetData(double *d, double *f, void *);
    bool CopyAndPutUserData( const void *);
    bool GetAndCopyUserData( void *, size_t);

#if 0
    virtual void DataEntryPoint(bool Read_WriteNot) = 0;
#endif
    /**
     * time difference between now (time called) and last shared memory update.
     * This should be negative if the SM has been touched before
     * we make this call. If positive, the SM hasn't been updated 
     * since return time value in seconds. 
     */
    double TimeSinceLastUpdate();
    /*
     * Time difference between the last check of SM and the 
     * current check of SM. 
     */
    double DeltaCheck();

    inline void*  GetROSharedMemoryAddress() const { return fUserSM_Segment;};
    inline size_t GetLength() const {return fHeader->Length;};

    enum SM_Errors{NO_ERROR, NO_KEY_OR_DIRECTORY, NO_OBJECT, NO_VIEW, 
		   NO_SEMAPHORE, NO_SEMAPHORE_MAP, NO_CONNECT, NO_ATTACH};
private:
    int    fSMHandle;
    unsigned short fNumberUserBytes;
    /*
     * What was the time value last time we checked?
     */
    struct timespec fLastTime; // Last time SM was updated

    /**
     * Head of the shared memory segment. (Primary address from allocation.)
     * Address where the data size is stored.
     */
    void   *fHead;
    struct MyMemoryHeader *fHeader;
    /*
     * This points to the area AFTER the time and length storage. 
     */
    unsigned char* fUserSM_Segment;

    // Semaphore name.
    key_t fSemKey;
    int   fSemaphoreHandle;
};
#endif
