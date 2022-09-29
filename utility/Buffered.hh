/**
 ******************************************************************
 *
 * Module Name : Buffered.hh
 *
 * Author/Date : C.B. Lirakis / 28-June-08
 *
 * Description : Permit the construction and manipulation of a series 
 * of data buffers. 
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
#ifndef  __BUFFERED_h_
# define __BUFFERED_h_
# include <time.h>
# include <ostream>

class Buffered {
public:
    Buffered(unsigned short N);
    ~Buffered();
    /// Put a character into the buffer with checks, advance pointer.
    int                    Put(unsigned char val);
    /// Put a batch of data in
    int                    PutBuffer(unsigned char *b, size_t s);
    /// Set the time on the buffer to NOW
    void                   SetTime();
    /// Get the declared size of the buffer. 
    inline unsigned short  GetSize()  const {return fSize;};
    /// Get the last timestamp on the buffer. 
    inline struct timespec GetTime()  const {return fnow;};
    /**
     *  Get the fill index. Usually used to see how many characters
     * there are in the buffer.
     */
    inline unsigned short  GetFill()  const {return fFillIndex;};
    /// Return the error from the last operation.
    inline int             GetError() const {return fError;};
    /// Check to see if the buffer is in use. 
    inline bool            Busy() const {return fBusy;};
    /// Set the in use flag for this buffer.
    inline void            SetBusy()    {fBusy = true;};
    /// Clear the in use flag for this buffer.
    void                   ClearBusy();
    /**
     * How many bytes are left in the buffer? Typically
     * when doing a drain on the buffer, the difference between
     * the read pointer and the beginning of the buffer.
     */
    inline unsigned Remaining() const {return (fFillIndex - (unsigned)(fdrain-fdata));};

    /// Dump all the data in the buffer in hex format.
    void HexDump( ofstream *o);
    /**
     * Reset all flags, clear the buffer. 
     */
    void Reset(); 
    /// Advance the drain pointer by i bytes in the buffer. 
    int Skip(unsigned i);

    /* Data extraction from the buffer. */
    /// Private Methods, used in decoding data.
    /// Get a character at location i, no change to drain pointer. 
    unsigned char Char(unsigned i);
    /// Get a character from the buffer and advance drain by 1
    unsigned char GetChar();
    /// Get a float from the buffer and advance drain by sizeof(float)
    float         GetSingle ();
    /// Get a short from the buffer and advance drain by sizeof(short)
    short         GetInt();
    /// Get a int from the buffer and advance drain by sizeof(int)
    int           GetLongInt();
    /// Get a double from the buffer and advance drain by sizeof(double)
    double        GetDouble();
    int           GetLine(char *p, size_t n);

    /// Use the next carefully.
    inline unsigned char* GetData() {return fdata;};
    inline void           FillIndex(unsigned short f) {fFillIndex = f;};
    /// and this one too.
    inline void DecrementFillCount() { fFillIndex--;};
    inline clock_t TimeStamp() const {return fTimeStamp;};
    const  char    *StatusBuf(void) const;

    enum {ERROR_NONE, BUFFER_EMPTY, BUFFER_OVERFLOW, BUFFER_BUGGERED};

private:
    /// Check to see if Fill index is buggered.
    inline bool Check() const {return (fFillIndex<fSize);};

    /// update fnow.
    void UpdateNow(void);

    /// Buffer is in use flag.
    bool            fBusy;
    /// The current pointer on write. The number of bytes on read. 
    unsigned short  fFillIndex;
    /// Declared size of buffer
    unsigned short  fSize;
    /// Time the buffer was last touched.
    struct timespec fnow;
    /// actual data buffer.
    unsigned char   *fdata;
    /// Pointer for use on read (drain) of buffer. 
    unsigned char   *fdrain;
    /// Last error
    int             fError;
    /// timestamp on when this buffer was filled. 
    clock_t         fTimeStamp;
};
#endif
