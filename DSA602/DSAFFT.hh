/**
 ******************************************************************
 *
 * Module Name : DSAFFT.hh
 *
 * Author/Date : C.B. Lirakis / 16-Dec-14
 *
 * Description : DSAFFT, 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References : DSA602A Programming Reference Manual
 *
 * 
 *******************************************************************
 */
#ifndef __DSAFFT_hh_
#define __DSAFFT_hh_
#  include <string> 
#  include "CObject.hh"
#  include "DSA602_Types.hh"
#  include "Units.hh"

/*
 * Typical channel query data. page 150
 * FFT AVG:ON,FORMAT:DBM,WINDOW:RECTANGULAR,PHASE:WRAP,DCSUP:OFF'
 */
class DSAFFT : public CObject
{
public:
    enum COMMANDs { kCAVG=0, kCDCSUP, kCFORMAT, kCPHASE, kCWINDOW, kCFILTER};

    DSAFFT(const char *val=NULL);
    ~DSAFFT();

    friend ostream& operator<<(ostream& output, const DSAFFT &n);
 
    static struct t_Commands DSAFFTCommands[7];

    void DecodeString(const char *val);

    // Data access functions;
    inline bool AVG(void)    const {return fAVG;};
    inline bool DCSUP(void)  const {return fDCSUP;};
    inline bool PHAse(void)  const {return fPHAse;};
    inline bool FILTer(void) const {return fFILTer;};
    inline FFT_WINDOW WINDow(void) const {return fWINDow;};
    inline FFTFORMAT  FORMat(void)   const {return fFORMat;};
    inline uint16_t   NAvg(void) const {return fNAvg;};
    inline string Text(void) const {return *fText;};

    bool SendCommand(COMMANDs c, bool t);
    bool SetFilter (bool t);
    bool SetPhase (bool t);
    bool SetFormat(FFTFORMAT t);
    bool SetWindow(FFT_WINDOW t);

    bool SetNAvg(uint16_t N);

    bool Update(void);



private:
    bool GetNAvg(void);          // Query NAvg. 
    string        *fText;        // Response string. 
    // Page 150
    bool          fAVG;          // true = on
    bool          fDCSUP;        // true = on
    FFTFORMAT     fFORMat;       // see format descriptions
    bool          fPHAse;        // true = UNWrap
    FFT_WINDOW    fWINDow;       // see descriptions
    bool          fFILTer;       // true = on
    /*! 
     * Don't know if this belongs here, but I'll put it here. 
     * Number of averages to perform, page 217
     * {2:65534}
     */
    uint16_t      fNAvg;         
};

#endif
