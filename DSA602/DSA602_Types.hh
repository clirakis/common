/**
 ******************************************************************
 *
 * Module Name : DSA602_Types.hh
 *
 * Author/Date : C.B. Lirakis / 29-Nov-14
 *
 * Description : header file for defining DS602 types etc. 
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
#ifndef __DSA602_TYPES_h_
#define __DSA602_TYPES_h_
    enum CALIBRATOR_FREQ {kCAL_FREQ_ZERO=0, kCAL_FREQ_1KHZ, kCAL_FREQ_1024MHZ};
    enum SLOT      {kSLOT_LEFT=0,kSLOT_CENTER,kSLOT_RIGHT, kSLOT_NONE};
    enum IMPEDANCE {kFIFTY=0, kONE_MEG, kONE_GIG}; // {50e1, 1e6, 1e9}
    enum COUPLING  {kCOUPLING_AC=0,kCOUPLING_DC,kCOUPLING_OFF,kCOUPLING_VC, kCOUPLING_NONE}; 
    enum COMMAND_TYPE {kCT_NONE=0,kCT_INT, kCT_BOOL,kCT_FLOAT,kCT_IMPEDANCE, 
		       kCT_COUPLING, kCT_STRING, kCT_ACCUMULATE, kCT_UNIT, 
		       kCT_FORMAT, kCT_WINDOW};
    enum HF_LIMIT { kTWENTY_MHZ=0, kONE_HUNDRED, kTHREE_HUNDRED, kTHREE_FIFTY};

    enum UNITS {kAMPS=0, kDB, kDEGREES, kDIVS, kHERTZ, kOHMS, kSECONDS, kVOLT, 
		kWATT, kUNITSNONE};
    enum ACCUMULATE {kINFPERSIST=0, kACC_OFF, kVARPERSIST};
    enum FFTFORMAT{ kDBM=0, kDBFUND, kDBVPEAK, kDBVRMS, kVPEAK, kVRMS, kFFTNONE};
    enum FFT_WINDOW{ kBLACKMAN=0, kBLHARRIS, kHAMMING, kHANNING, kRECTANGULAR, 
		     kTRIANGULAR, kWINDOW_NONE};
    enum PT_TYPES {kPT_ENV=0, kPT_XY, kPT_Y, kPT_NONE};
    enum MEASUREMENT_QUALIFIER {kEQUAL=0, kLESS_THAN, kGREATER_THAN, 
				kUNKNOWN, kERROR, kMNONE};

struct t_Commands
{
    const char*   Command;
    unsigned char Type;
    double        Upper;
    double        Lower;
    double        Step;      // Stepsize
    bool          Supported; // not every command is supported per module
};
struct t_Sensitivity 
{
    double Low;
    double High;
    double Stepsize;
};

struct t_TBLength 
{
    const unsigned short val;
    bool valid;
};

struct t_Period
{
    double         DT;
    unsigned short AllowedLengths;
    const char*    label;
};
#endif
