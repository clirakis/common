/**
 ******************************************************************
 *
 * Module Name : BC635.h
 *
 * Author/Date : C.B. Lirakis / 22-Jul-10
 *
 * Description :
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
#ifndef __BC635_hh_
#define __BC635_hh_
#include "bcuser.h"
struct timespec;

class BC635 
{
public:
    BC635(void);
    virtual ~BC635(void);
    static struct BC_PCI_STRUCT* getHandle(void) {return fBC_PCI;}; 
    static BC635* Get635(void)   {return fBC635;};
    static int    Verbose(void)  {return fVerbose;};
    inline void   Verbose(int i) {fVerbose = i;};

    int           DriverVerbose(uint8_t i);
    unsigned char RequestTime(struct timespec *req, double *delta);
    bool          SetTime(time_t sec);
    bool          SyncRTC();
    bool          DisconnectBattery();
    bool          SetLeapSeconds(int sec, unsigned char flag);
    bool          SetYear(int year);
    bool          SetLocalOffset(int off, bool half = false);
    bool          SetTimeFormat(int f);
    bool          SetPropagationDelay(long d);
    bool          SetInputTimeFormat(int v);
    bool          SetInputModulation(int v);
    bool          SetOutputFormat(int v);
    bool          SetGeneratorOffset(int v, bool half);
    bool          SetHeartBeat(int mode, int counter1, int counter2);
    bool          DaylightSavings(bool set);
    bool          OutputFrequency(int v);
    bool          SetStrobeTime( bool major, time_t s, unsigned usec);
    bool          SetMode(int mode);
    int           OutputFrequency();
    bool          EnableJamSync(bool tf = true);
    bool          ForceJamSync();
    bool          SetDAC(unsigned short val);
    bool          SetGain(short val);
    bool          SetClock( long val);
    bool          SetClockSource(bool internal = true);
    void          EventTime(int counter=0, struct timespec *r=0);
    unsigned short RevisionID(void);

    inline int Error(void) const {return fError;};
    enum BC635Errors {NO_ERROR, OPEN_FAIL, REQUEST_FAIL};

private:
    static BC_PCI_STRUCT *fBC_PCI;
    static int            fVerbose;
    static BC635         *fBC635;
    int                   fError;
};


#endif
