
#ifndef _NMEA_GPS_HH_
#define _NMEA_GPS_HH_
#include <stdint.h>
#include <time.h>

#include "GGA.hh"
#include "RMC.hh"
#include "VTG.hh"
#include "GSA.hh"



#if 0
// Satellites in view
class SatelliteData
{
public: 
    SatelliteData(void);
    bool Decode(const char *);
private:
    unsigned char fID;
    unsigned char fElevation;
    unsigned char fAzimuth;
    unsigned char fSNR;
}
class GSV
{
public:
    GSV(void);
    bool Decode(const char *);
private:
    unsigned char fNMsg;
    SatelliteData fSat[12];
};
#endif
// Course and speed

class NMEA_GPS 
{
 public:
    enum ErrorCodes{ERROR_NONE=0, SERIAL_OPEN_FAIL};
    enum MessageID {MESSAGE_NONE=0,MESSAGE_GGA, MESSAGE_RMC, MESSAGE_VTG, 
		    MESSAGE_GSA, MESSAGE_GSV, MESSAGE_LOG, MESSAGE_AWAKE};
 
    // Constructor - calls serial port open for us. 
    NMEA_GPS( void);
    ~NMEA_GPS(void);

    static NMEA_GPS* GetThis(void) {return fNMEA;};

    inline int  ErrorCode(void) {return fErrorCode;};
    inline bool Error(void)     {return !(fErrorCode == ERROR_NONE);};
    inline int  LastID(void)    {return fLastID;};
    inline GGA* pGGA(void)      {return fGGA;};
    inline RMC* pRMC(void)      {return fRMC;};
    inline VTG* pVTG(void)      {return fVTG;};
    inline GSA* pGSA(void)      {return fGSA;};

    // See if anything is available. 
    bool Read(void);

    
private:
    static NMEA_GPS *fNMEA;

    // Private funciton calls. 
    uint8_t parseResponse(char *response);
    uint8_t parseHex(char c);
    bool parseNMEA(char *response);    
    bool parse(const char *);
    bool CheckSum(const char *line);
    bool ParseGGA(const char *line);
    bool ParseRMC(const char *line);

    // Private variables
    GGA  *fGGA;
    RMC  *fRMC;
    VTG  *fVTG;
    GSA  *fGSA;

    bool paused;    
    int  fErrorCode;
    int  fLastID;
    bool fRecvdflag;
};


#endif
