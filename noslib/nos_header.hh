/**
 ******************************************************************
 *
 * Module Name : nos_header.h
 *
 * Author/Date : C.B. Lirakis / 09-Sep-01
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
 *
 *
 *******************************************************************
 */
#ifndef __NOS_HEADER_h_
#define __NOS_HEADER_h_
#include <vector>
#include <list>

/// Module documentation here. 
class NOS_RGB {
public:
    /// Default constructor.
    NOS_RGB(int index = 0, unsigned char R=0, unsigned char G=0, 
	    unsigned char B=0 ) 
    {Number=index; RGB[0] = R; RGB[1] = G; RGB[2] = B;};
    
    /// Constructor using a string input as directly read from the NOS file. 
    NOS_RGB(const string &value);
    
    NOS_RGB operator = (const NOS_RGB &in) 
    {Number=in.Number; memcpy(RGB, in.RGB, 3); return *this;};

    inline unsigned char GetIndex() const {return Number;};
    inline unsigned char GetRGB(const int i) const 
    {if (i>=0 && i<3) return RGB[i]; else return 0;};
    string RGBString() const;
    friend ostream& operator<<(ostream& output, const NOS_RGB &n);

private:
    inline void SetIndex(const unsigned char i) {Number = i;};
    unsigned char Number;
    unsigned char RGB[3];
};

class NOSDecode {
public:
    NOSDecode();
    void ResetPointer();
    inline unsigned char GetColor() const {return Color;};

    inline unsigned GetRunLength() const {return RunLength;};
    inline unsigned GetLine() const {return LineNumber;};
    inline void     SetVerbose(bool k) {verbose = k;};

protected:
    void Skip2EOL(const unsigned char *buffer);
    bool Decompress(const unsigned char *buffer, unsigned short IFMValue);
    inline void SetWidth(unsigned short *X) { Width = X[2]-X[0];};
    bool     verbose;
    /// File size in bytes, also the size of BigBuffer
    unsigned long FileSize;

private:
    void DumpBytes(const unsigned char *buffer) const;
    inline unsigned char GetNext(const unsigned char *buffer) 
    {Continue=((buffer[Pointer] & 0x80)>0); return(buffer[Pointer]&0x7F);};
    bool          Continue;
    bool          BOL;
    unsigned      LineNumber;  // Line number in use.
    unsigned char Color;       // color of current run
    unsigned      RunLength;   // Length of current run
    unsigned      SumLength;   // Total length of current line
    unsigned      Width;
    unsigned long Pointer;
    static unsigned long InternalLineCount;
};

class NOS : public NOSDecode {
public:
    /// Default Constructor
    NOS(const char *filename);
    /// Default destructor
    ~NOS();
#if 0
    bool DrawExtents(unsigned short X0, unsigned short Y0, 
		     unsigned short X1, unsigned short Y1,
		     unsigned short Decimation = 1);

    /// A place for the calling application to put its drawing code.
    virtual bool DrawLine(short X0, short X1, short Y, char color);
#endif
    inline  unsigned short GetExtents(int i) const 
    {if (i>=0 && i<4) return Extents[i]; else return 0;};

    inline unsigned short GetDisplayUnits() const {return DisplayUnits;};
    inline int GetError() const {return ErrorCode;};

    /// Get access to big buffer.
    unsigned long  DataSize() const {return FileSize;};
    unsigned char *Data() {return BigBuffer;};
    string RGBString(unsigned index) const;
    inline unsigned GetNumberColors() const {return rgb.size();};
    NOS_RGB GetColor(unsigned index); 

    void Dump();
    inline void Go2EOL() {Skip2EOL(BigBuffer);};
    inline bool GetData() {return Decompress( BigBuffer, IFMValue);};
    inline unsigned char Color() const {return NOSDecode::GetColor();};

protected:
    /// RGB records, define some colors for use. 
    vector <NOS_RGB *> rgb;

private:
    /// NOS header data.
    /// Chart name
    string *Name;
    /// Date file was created. 
    string *Date;
    /// Time file was created. 
    string *Time;
    /**
     * Extents are in terms of pixels. 
     * Upper Left XY and Lower Right XY.
     */
    unsigned short Extents[4];
    /// Display units in pixels per inch.
    unsigned short DisplayUnits;
    /// Binary image format version. 
    unsigned short Version;
    /// OST - The number of image lines between strip offset values. 
    unsigned short OSTValue;
    /// Compression code, IFM 1-7
    unsigned short IFMValue;
    /// Error code for last operation.
    unsigned short ErrorCode;

    /// Number of bits to shift to get data from first line in byte
    unsigned char shift;
    /// Bits to mask off to get color data.
    unsigned char mask;
    /// Bits to mask off to get data off color byte.
    unsigned char dmask;
    /// End of header position.
    unsigned long EndHeader;
    
    /// Current pen in use for line to be drawn. 
    unsigned char  color;
    /// Current pixel pointers.
    unsigned short X0, X1, Y; 
     
    /// Store the offsets for all of the lines in the file
    /// so we can get there quick!
    long *LineOffset;
    
    /// Buffer that is dynamically allocated when reading the entire 
    /// file into memory. 
    unsigned char *BigBuffer;

    /// Function to parse the header information. 
    bool ParseHeader(const char *buffer);
    /// 
    bool ParseBuffer();

    /// Once we have a line of data, decode and manipulate it. 
    void DecodeBuffer(const unsigned char *buffer,  int Nbytes);

    /**
     * Get the X value from the buffer, return the pointer to the new
     * location. Nbytes contains the new byte count left in the 
     * buffer.
     */
    unsigned short GetXData(unsigned char **ptr);
    /**
     * Get the Y value from the buffer, return the pointer to the new
     * location. Nbytes contains the new byte count left in the 
     * buffer.
     */
    unsigned short GetYData(unsigned char **ptr);
    /**
     * Another helper function, get a buffer of data that represents 
     * a series of pixels. Fills the buffer allocated by the 
     * calling program, and returns the number of bytes
     * in the buffer. 
     */
    int FillDataBuffer(unsigned char *ptr, int BufferSize);
};
#endif
