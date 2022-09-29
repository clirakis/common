/**
 ******************************************************************
 *
 * Module Name : GPIB.hh
 *
 * Author/Date : C.B. Lirakis / 26-Jan-14
 *
 * Description : header file for DS602 digitizing scope with GPIB 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 27-Nov-14 CBL moved some base GPIB commands here. 
 * 28-Oct-18 CBL Put more error checking in. 
 * 17-Jan-21 CBL Updated to utilize more of the other utilties for
 *               logging etc. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
#ifndef __GPIB_hh_
#define __GPIB_hh_
#    include <stdint.h>   // define integer on various machines
#    include <fstream>
#    include "CObject.hh"
#    include "gpib/ib.h"
#    include "gpib/gpib_user.h"

/// GPIB documentation here. 
class GPIB : public CObject
{
public:
    /// Default Constructor
    GPIB();
    GPIB(int gpib_address, bool verbose=false);
    /// Default destructor
    virtual ~GPIB();
    bool SetAddress(int gpib_address);
    /// Module function
    void SimpleRead();
    bool Read(void *buffer, size_t n) const;
    bool Command(const char *Command, char *Response, size_t n) const;
    /*! Provide a string of the status of the last command */
    const char* str_Status(void) const;
    /*! Provide a string for the last GPIB error. */
    const char* str_Error(const char* Function, int LineNumber) const;

    inline int GetHandle(void)  const {return fHandle;};
    inline int GetCount(void)   const {return ibcnt;};
    inline int GetError(void)   const {return iberr;};
    inline int GetStation(void) const {return ibsta;};

    bool gDeviceClear(void);
    bool SetTimeout(int Value);

    // General inline commands
    inline bool RemoteEnable(void)   { return Command("REN", NULL, 0);};
    inline bool InterfaceClear(void) { return Command("IFC", NULL, 0);};
    inline bool LocalLockout(void)   { return Command("LLO", NULL, 0);};
    inline bool GoToLocal(void)      { return Command("GTL", NULL, 0);};
    inline bool DeviceClear(void)    { return Command("DCL", NULL, 0);};
    inline bool SelectiveDeviceClear(void) { return Command("SDC", NULL, 0);};
    // Group Execute Trigger
    inline bool GET(void)            { return Command("GET", NULL, 0);};
    inline bool Execute(void)        { return Command("X", NULL, 0);};

    /*! Check ibsta for error bit. */
    inline bool IsError(void) const  { return ((ibsta&ERR)>0);};

    /* Helper functions */
    /*! Return the GPIB address of this instance. {1:31} */
    inline const int Address(void) const {return fAddress;};

protected:
    int fHandle;   // GPIB handle
    int fAddress;  // address set at startup
};
#endif
