/********************************************************************
 *
 * Module Name : serialIO.h
 *
 * Author/Date : C.B. Lirakis / 03-Mar-99
 *
 * Description : MS Windows specific implimentation of serial I/O 
 *               interface. 
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * 09-Jun-02 CBL         Modified so that the user can pass in the 
 *                       termination character. 
 * 09-May-20 CBL         Accidentially deleted header, recovering. 
 * 17-Feb-24 CBL         commenting and neating up. 
 *
 * Classification : Unclassified
 *
 * References :
 *
 *
 ********************************************************************/
#ifndef __SERIALIO_h_
#define __SERIALIO_h_
#  include <termios.h>
#  include <unistd.h>
#  include <errno.h>
#  include "CObject.hh"

/// Serial IO class 
class  SerialIO : public CObject
{
public:
    /// Baud rate enumurator
    enum Parity {NONE, ODD, EVEN};
    enum OpenMode {ModeRaw, ModeCanonical};
    /// Error codes
    enum STATUS {
	NOError=0,     /** No error incured during last operation */
	RxError,       /** Set when a GetData fails */
	TxError,       /** Could not write all data. */
	BadReturn,     /** Return did not match what was sent */
	BadCheckSum,   /** Not currently implimented */
	GotNULL,       /** Not currently used        */
	BadOpen,       /** Could not open!           */
	ErrorSetAttributes, /** Could not set an attritbute */
	ErrorGetAttributes, /** Could not get an attritbute */
	ErrorSetBaudRate    /** Baud rate not set correctly. */
    };

    /**
     * Constructor
     * Inputs:
     *     Com port to open
     *     Serial mode as enumerated above. 
     *     Extra data, depends on mode.
     *         Mode = Raw then the extra character is the minimum # bytes.
     *         Mode = Canonical, then it specifies the EOF character. 
     *
     * Constructor, Use com 1 as default, with a baud rate of 9600, 
     * and (Data1) termination character of 0x0D.
     * In the event that the mode is set as Raw this is the 
     * number of minimum characters. 
     * For raw mode, data2 = number of 1/10 seconds to wait
     * for a character. 
     */
    SerialIO(const char *ComPortName="ttyUSB0",
	     speed_t BR=B9600, 
	     Parity pr=SerialIO::NONE,  
	     OpenMode Mode=SerialIO::ModeRaw,
	     unsigned char Data1=0x0D, unsigned char Data2=2, 
	     bool Block = false); 
    /// Serial IO destructor.
    ~SerialIO(void);

    /// Get a  single byte of data.
    unsigned char Read(void);
    /**
     * Get a  single byte of data. returns true if a single character 
     * was received. 
     */
    bool Read(unsigned char *buf);
    /** 
     * Get a buffer of data, check against size. 
     * returns the number of characters retreived. 
     * INPUTS:
     *    buf  - the adress of the buffer to fill. 
     *    size - The number of bytes to fill the above buffer with. 
     */
    int32_t  Read(unsigned char *buf, const size_t size, 
		  const bool CheckSize = false);
    /**
     * Put a buffer of unsigned chars. 
     * INPUTS
     *    buf  - the buffer of data to put on the serial line. 
     *    size - The number of bytes to transmit. 
     */
    int  Write(const unsigned char *buf, const size_t size);
    bool Write(unsigned char val);
    /**
     * Put a null on the line.
     */
    bool PutNull(void);
    /**
     * Put a single character, then read it back, check it against
     * the character we put. 
     * Returns SerialIO_NOError if successful. 
     */
    int PutSingleWithCheck(const unsigned char *buf);

    /**
     * Send a Break
     */
    void Break(void);

    /**
     * Flush the io buffer. 
     */
    void Flush(void);

    /**
     * Purge the input and output buffers.
     */
    void FullPurge(void);

    /** 
     * Get status of DCD
     */
    bool GetDCD(void);

    /**
     * Get the name of the serial port if open. 
     */
    inline const char * GetPortName(void) {return fPortName;};

    /*!
     * Get access  to errno!
     */
    int GetErrno(void) const;

    /*!
     * Set status of DTR line 
     */
    int SetDTR(const bool value);

    /*!
     * Get the error string associated with the open. 
     */
    const char* GetErrorString(void);

    bool IsCTSEnabled(void);


private:
    /**
     * set the non-canonical mode, use 
     * nmin - minimum  number  of characters to  process prior to timeout.
     * nwait - number of 1/10 second to wait.
     */
    void SetModeRaw(struct termios *termios_p, char  nmin, char nwait);
    void SetModeCanonical(struct termios *termios_p,unsigned char *eof);
    bool SetAndCheck( struct termios *termios_p);
    int   fPort;
    char *fPortName;

    /*
     * What mode was this opened in? 
     * true  = Canonical
     * false = Raw 
     */
    bool fMode; 
};
#endif
