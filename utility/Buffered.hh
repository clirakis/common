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
# include <stdint.h>

class Buffered {
public:
    // Construct a buffer of size N
    Buffered(unsigned short N);

    ~Buffered();

    /// Put a character into the buffer with checks, advance pointer.
    int32_t                Put(unsigned char val);

    /// Put a batch of data in
    int32_t                PutBuffer(unsigned char *b, size_t s);

    /// Set the time on the buffer to NOW
    void                   SetTime(void);

    /// Get the declared size of the buffer. 
    inline unsigned short  GetSize(void)  const {return fSize;};

    /// Get the last timestamp on the buffer. 
    inline struct timespec GetTime(void)  const {return fnow;};
    /**
     *  Get the fill index. Usually used to see how many characters
     * there are in the buffer.
     */
    inline unsigned short  GetFill(void)  const {return fFillIndex;};

    /// Return the error from the last operation.
    inline int             GetError(void) const {return fError;};

    /// Check to see if the buffer is in use. 
    inline bool            Busy(void) const {return fBusy;};

    /// Set the in use flag for this buffer.
    inline void            SetBusy(void)    {fBusy = true;};

    /// Clear the in use flag for this buffer.
    void                   ClearBusy(void);

    /**
     * How many bytes are left in the buffer? Typically
     * when doing a drain on the buffer, the difference between
     * the read pointer and the beginning of the buffer.
     */
    inline uint32_t Remaining(void) const 
	{return (fFillIndex - (unsigned)(fdrain-fdata));};


    /**
     * Reset all flags, clear the buffer. Clear CLEAR
     */
    void Reset(void); 

    /// Advance the drain pointer by i bytes in the buffer. 
    int Skip(unsigned i);

    /* Data extraction from the buffer. */
    /// Private Methods, used in decoding data.
    /// Get a character at location i, no change to drain pointer. 
    unsigned char Char(unsigned i);
    /// Get a character from the buffer and advance drain by 1
    unsigned char GetChar(void);
    /// Get a float from the buffer and advance drain by sizeof(float)
    float         GetSingle (void);
    /// Get a short from the buffer and advance drain by sizeof(short)
    short         GetInt(void);
    /// Get a int from the buffer and advance drain by sizeof(int)
    int           GetLongInt(void);
    /// Get a double from the buffer and advance drain by sizeof(double)
    double        GetDouble(void);
    int           GetLine(char *p, size_t n);

    /// Use the next carefully.
    inline unsigned char* GetData(void) {return fdata;};
    inline void           SetFillIndex(unsigned short f) {fFillIndex = f;};
    inline unsigned short GetFillIndex(void) {return fFillIndex;};

    /// and this one too.
    inline void    DecrementFillCount(void) { fFillIndex--;};
    inline clock_t TimeStamp (void) const {return fTimeStamp;};
    const  char    *StatusBuf(void) const;

    inline bool    IsFull(void) {return (fFillIndex>fSize-1);};


    friend ostream& operator<<(ostream& output, const Buffered &n);

    enum {kERROR_NONE, kBUFFER_EMPTY, kBUFFER_OVERFLOW, kBUFFER_BUGGERED};

private:
    /// Check to see if Fill index is buggered.
    inline bool Check(void) const {return (fFillIndex<fSize);};

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
