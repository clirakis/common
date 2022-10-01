/********************************************************************
 *
 * Module Name : nos_header.cpp
 *
 * Author/Date : C.B. Lirakis / 09-Sep-01
 *
 * Description : Generic module
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
 ********************************************************************/

// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cstring>
#include <cmath>
#include <fstream>

// Local Includes.
#include "nos_header.hh"
unsigned long NOSDecode::InternalLineCount = 0L;

/**
 ******************************************************************
 *
 * Function Name : NOS_RGB
 *
 * Description : Constructor based on ASCII string found in NOS
 * header file.
 *
 * Inputs : string to decode. 
 *
 * Returns : fully constructed NOS_RGB value
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 25-Apr-09
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
NOS_RGB::NOS_RGB(const string &value)
{
    int N;
    string temp;
    
    N = value.find(",");
    Number = atoi(value.substr(0,N).c_str());
    temp = value.substr(N+1,value.length());
    
    N = temp.find(",");
    RGB[0] = atoi(temp.substr(0,N).c_str());
    temp = temp.substr(N+1,temp.length());
    
    N = temp.find(",");
    RGB[1] = atoi(temp.substr(0,N).c_str());
    temp = temp.substr(N+1,temp.length());
    
    N = temp.length();
    RGB[2] = atoi(temp.substr(0,N).c_str());
}
/**
 ******************************************************************
 *
 * Function Name : RGBString()
 *
 * Description : Return a hex string that represents the 
 * value of the RGB for the system.
 *
 * Inputs : none
 *
 * Returns : string
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string NOS_RGB::RGBString() const
{
    char tmp[16];
    sprintf(tmp, "#%2.2X%2.2X%2.2X", RGB[0],RGB[1],RGB[2]);
    return string(tmp);
}
/**
 ******************************************************************
 *
 * Function Name : operator << used with NOS_RGB
 *
 * Description : Dump the data value of the RGB for the system.
 *
 * Inputs : none
 *
 * Returns : string
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const NOS_RGB &n)
{
    output << "Index: " << (int) n.Number
	   << " " << (int) n.RGB[0] 
	   << " " << (int) n.RGB[1] 
	   << " " << (int) n.RGB[2];
    return output;
}

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NOS::ParseHeader(const char *buffer)
{
    // A listing of the keywords we expect
    const string NOSNA("NOS/NA=");
    const string RA("RA=");
    const string DA("DA=");
    const string TI("TI=");
    const string DU("DU=");
    const string VER("VER/");
    const string OST("OST/");
    const string IFM("IFM/");
    const string RGB("RGB/");


    string   instr(buffer);
    unsigned N, N2;
    string   temp;

    ErrorCode = 0;
    // Ok now we gotta get the various keywords. 
    N = instr.find(NOSNA);
    N += NOSNA.length();
    N2 = instr.find(",");
    temp = instr.substr(N2+1,instr.length());
    Name = new string(instr.substr(N,N2-N));

    // Look for number of pixels X & Y.
    N = temp.find(RA);
    N += RA.length();
    temp = temp.substr(N,temp.length());

    for (int i=0; i < 4; i++)
    {
	N = temp.find(",");
	Extents[i] = atoi(temp.substr(0,N).c_str());
	temp = temp.substr(N+1,temp.length());
    }
    // Data file generated. 
    N  = temp.find(DA);
    N += DA.length();
    N2 = temp.find(",");
    Date = new string(temp.substr(N,N2-N));
    temp = temp.substr(N2+1,temp.length());

    // Time field
    N = temp.find(TI);
    N += TI.length();
    N2 = temp.find(",");
    Time = new string(temp.substr(N,N2-N));
    temp = temp.substr(N2+1,temp.length());

    // Display Units.
    N = temp.find(DU);
    N += DU.length();
    N2 = temp.find("\r");
    DisplayUnits = atoi(temp.substr(N,N2-N).c_str());
    temp = temp.substr(N2+1,temp.length());

    // Map Version 
    N = temp.find(VER);
    N += VER.length();
    N2 = temp.find("\r");
    Version = atoi(temp.substr(N,N2-N).c_str());
    temp = temp.substr(N2+1,temp.length());

    // OST, The strip offset count. 
    N = temp.find(OST);
    N += OST.length();
    N2 = temp.find("\r");
    OSTValue = atoi(temp.substr(N,N2-N).c_str());
    temp = temp.substr(N2+1,temp.length());

    // IFM - The compression type. 
    N = temp.find(IFM);
    N += IFM.length();
    N2 = temp.find("\r");
    IFMValue = atoi(temp.substr(N,N2-N).c_str());
    temp = temp.substr(N2+1,temp.length());

    // RGB Table. 
    NOS_RGB *pt;

    while((N = temp.find(RGB)) != temp.npos)
    {
	N += RGB.length();
	temp = temp.substr(N,temp.length());
	N2 = temp.find("\r");
	pt = new NOS_RGB(temp.substr(0,N2));
	rgb.push_back(pt);
	temp = temp.substr(N2+1,temp.length());
    }
    return true;
}

/********************************************************************
 *
 * Function Name : NOS constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
NOS::NOS (const char *filename) : NOSDecode()
{
    // Input file stream pointer. 
    ifstream *nos;
    // Chart name
    Name = NULL;
    // Date file was created. 
    Date = NULL;
    // Time file was created. 
    Time = NULL;
    BigBuffer   = NULL;
    //LineOffset  = NULL;

    /**
     * We need to open the file. Do the usual checks,
     * 1) is the filename NULL?
     * 2) Does it already contain the geo extention?
     *
     * We assume that the parent application is storing the path for us. 
     */
    if (filename == NULL)
    {
	ErrorCode = 1;
	return;
    }
   
    // Open the file and start to read it. 
    nos = new ifstream(filename, ios::in|ios::binary);
    if (nos->fail())
    {
	// Error opening file. Maybe it doesn't exist. 
	ErrorCode = 2;
	return;
    }

    char buffer[2048];

    nos->get(buffer,sizeof(buffer),0x1A);
    int ngot = nos->tellg();
    if (!nos->fail())
    {
	// Parse the line. 
	ParseHeader(buffer);
    }

    /**
     * Now we have some stuff having to do with how the data is packed. 
     * The first byte will be the remaining 1A terminator.
     * The second byte will be the null/00 terminator 
     * the third should be the same as the IFM number. 
     * Check encryption method. 
     */
    memset(buffer,0,sizeof(buffer));
    nos->read(buffer,3);
    if (buffer[2] != IFMValue)
    {
	ErrorCode = 3;
    }
    shift = 7 - IFMValue;
    mask  = (0x01<<IFMValue) - 1;
    dmask = (0x01<<shift) - 1;

    X0 = X1 = Y = 0;
    // Done!
    EndHeader = nos->tellg(); // Our current position;

    // Seek to EOF to determine file size. 
    nos->seekg(0,ios::end);
    long EndByte = nos->tellg();

    // Size of binary data. 
    FileSize = EndByte - EndHeader;
    // Reposition file
    nos->seekg(EndHeader,ios::beg);

    // Allocate space for line offsets. 
    LineOffset = new long[Extents[3]];
    // Make sure this is zeroed.
    memset(LineOffset, 0, Extents[3]*sizeof(long));

    // Allocate the necessary memory
    BigBuffer = new unsigned char[FileSize];
    // Get it all and close the file. 
    nos->read((char *)BigBuffer, FileSize);
    nos->close();
    delete nos;
    ParseBuffer();
    SetWidth( Extents);
}
/********************************************************************
 *
 * Function Name : NOS destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
NOS::~NOS ()
{
    if (Name)
	delete Name;
    if (Date)
	delete Date;
    if (Time)
	delete Time;
    if (BigBuffer)
	delete[] BigBuffer;
    if (LineOffset)
	delete[] LineOffset;

    /// RGB records, define some colors for use. 
	vector <NOS_RGB *>::iterator colors;

	for (colors = rgb.begin(); colors != rgb.end(); colors++)
	{
	    delete (NOS_RGB *) *colors;
	}

}
/********************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
unsigned short  NOS::GetYData(unsigned char **ptr)
{
    bool cont;           // Continuation marker. 
    unsigned char X;     // Extracted data.
    unsigned short rv;

    X  = 0;
    rv = 0;
    cont = true;

    while(cont)
    {
	// Get data and continuation marker. 
        X    = (**ptr)&0x7F;
	cont = int((**ptr)&0x80) > 0;
	rv += X;
	if (cont)
	{
	    rv = rv<<7;
	}
	(*ptr)++;
    }
    // Thursday
    while (**ptr == 0)
    {
	(*ptr)++;
    }
    return rv;
}
/********************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
unsigned short  NOS::GetXData(unsigned char **ptr)
{
    bool           cont; // Continuation marker. 
    unsigned char  X;    // Extracted data.
    unsigned short rv;   // Return value once extracted. 

    color = 0;           // Each pixel group has its own color!
    rv = 0;              // Initialize the system. 
    X  = 0;
    cont = true;

    while(cont)
    {
	// Get data and continuation marker. 
	cont = (int)((**ptr)&0x80) > 0;
        if (color == 0)
	{
	    color = ((**ptr)>>shift)&mask;
            X     = (**ptr)&dmask;
	}
	else
	{
            X = (**ptr)&0x7F;
	}

	rv += X;
	if (cont)
	{
	    rv = rv <<7;
	}
	(*ptr)++;
    }
    while(**ptr==0)
    {
	(*ptr)++;
    }
    return rv;
}
#if 0
int NOS::FillDataBuffer(unsigned char *buffer, int BufferSize)
{
    long BufferCount, StartCount, Ngot;
    bool cont;
    bool testme = false;
    BufferCount = 0L;

    /**
     * Read a line. All lines are terminated by a NULL byte. 
     * However, it is possible to get a NULL as part of the data
     * stream. Allowing the stream to control how much to read
     * is faster though. 
     */
    cont = true;
    memset(buffer,0,BufferSize);
    StartCount = nos->tellg();              /* Get the starting position. */
    int val = nos->peek();
    if (val == 0)
    {
	nos->seekg(1,ios::cur);
	StartCount++;
    }
    Ngot = nos->tellg();
    nos->get((char *)&buffer[BufferCount], BufferSize, 0x00);

    if (!nos->fail())
    {
        Ngot = (int)nos->tellg();
	Ngot -= StartCount;        // How many bytes did we get?
	BufferCount += Ngot;
	nos->read((char *)&buffer[BufferCount], 1);
	BufferCount++;
    }
    else
    {
	int k = ios::goodbit;
	k = ios::badbit;
	k = ios::failbit;
	k = ios::eofbit;
	nos->rdstate();
	BufferCount = -1;
    }
    return BufferCount;
}

/********************************************************************
 *
 * Function Name : NOS function
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NOS::ReadLine()
{
    unsigned char buffer[4096], *ptr;
    int Ynew = -1;
    int NLeft = 2;
    long offset;

    // Initialize the start and end points of this line. 
    X0 = X1 = 1;

    // Each read the count on the number of bytes includes the null byte.
    while (X0 < Extents[2])
    {
	offset = nos->tellg();
	// Get a buffer of data;
	NLeft = FillDataBuffer(buffer,sizeof(buffer));
	if ( NLeft <=0)
	{
	    // error...
	    return false;
	}
        ptr = buffer;
	if (Ynew < 0)
	{
	    // First order of business, get the current line number. 
	    Ynew  = GetYData(&ptr);
	    if (Ynew < Y)
	    {
		// Error!
	    }
	    else
	    {
		Y = Ynew;
		if (LineOffset[Y-1] == 0)
		{
		    LineOffset[Y-1] = offset;
		}
	    }
	}
	while (NLeft > 1)
	{
	    X1 = GetXData(&ptr) + X0; 
	    DrawLine(X0,X1,Y,color);
	    X0 = X1+1;
	}
    }
    return true;
}
void NOS::Rewind() 
{
    if (nos != NULL)
    {
	nos->seekg(EndHeader,ios::beg);
    }
}

/********************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NOS::DrawExtents(unsigned short X0in, unsigned short Y0in, 
		      unsigned short X1in, unsigned short Y1in,
		      unsigned short Decimation)
{
    unsigned char *ptr;
    unsigned short YLo, YHi, XLo, XHi;

    // Make sure that the upper and lower bounds make sense.
    if (Y0in<Y1in)
    {
	YLo = Y0in;
	YHi = Y1in;
    }
    else
    {
	YLo = Y1in;
	YHi = Y0in;
    }
    YLo = (YLo>Extents[1]) ? YLo:Extents[1];
    YHi = (YHi<Extents[3]) ? YHi:Extents[3];

    // Make sure that the upper and lower bounds make sense.
    if (X0in<X1in)
    {
	XLo = X0in;
	XHi = X1in;
    }
    else
    {
	XLo = X1in;
	XHi = X0in;
    }
    XLo = (XLo>Extents[0]) ? XLo:Extents[0];
    XHi = (XHi<Extents[2]) ? XHi:Extents[2];

    short step = Decimation;
    if (step > 30)
	step = 30;

    for (short i=YLo; i <YHi ;i+=Decimation)
    {
	ptr = &BigBuffer[LineOffset[i]];
	X0 = X1 = 1;
	Y = GetYData(&ptr);
	while (X0 < Extents[2])
	{
	    X1 = GetXData(&ptr) + X0; 
	    if (X0 >= XLo && X1<=XHi)
	    {
		DrawLine(X0,X1,Y,color);
	    }
	    else if (X0<XLo && X1<=XHi)
	    {
		DrawLine(XLo,X1,Y,color);
	    } 
	    else if (X0>=XLo && X1>XHi)
	    {
		DrawLine(X0,XHi,Y,color);
	    }
	    X0 = X1+1;
	}
    }
    return true;
}

/********************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NOS::DrawLine(short X0, short X1, short Y, char color)
{
    // line is at Y and from X0 to X1
    // This function is expected to be overridden by the 
    // class sub-classed to this. 
    return true;
}
#endif
/********************************************************************
 *
 * Function Name : 
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NOS::ParseBuffer()
{
    int Delta;
    unsigned char  *ptr = BigBuffer;
    unsigned short Ynew;

    do {
	Delta = ptr-BigBuffer;
        // Initialize the start and end points of this line. 
	X0 = X1 = 1;
	Ynew  = GetYData(&ptr);
	if (Ynew < Y)
	{
	    // This constitutes an error. 
	    return false;
	}
	else
	{
	    Y = Ynew;
	    // Save the offset for this line. 
	    if (LineOffset[Y-1] == 0)
	    {
		LineOffset[Y-1] = Delta;
	    }
	}
	while (X0 < Extents[2])
	{
	    X1 = GetXData(&ptr) + X0; 
	    X0 = X1+1;
	}
    } while (Y<Extents[3]);
    return true;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NOS::Dump()
{
    unsigned i;
    cout << "NOS Dump-----------------------------" << endl
	 << *Name << " "
	 << *Date << " "
	 << *Time << " "
	 << endl 
	 << "Extents: " << Extents[0] << " "
	 << Extents[1] << " " << Extents[2] << " " << Extents[3]
	 << endl
	 << "Display Units: " << DisplayUnits << " Pixels/inch " 
	 << endl
	 << "Compression: " << IFMValue 
	 << " Number Colors: " << rgb.size()
	 << endl
	 << "OST Value: " << OSTValue
	 << endl;
    cout << "Colors:" << endl;
    for(i=0;i<rgb.size();i++)
    {
	cout << i << " " << (int) rgb[i]->GetIndex() << " " 
	     << rgb[i]->RGBString() 
	     << endl;
    }
}

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
string NOS::RGBString(unsigned index) const
{
    if (index<0 || index>=rgb.size()) index = 0;

    return rgb[index]->RGBString();
}

/******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
NOSDecode::NOSDecode()
{
    BOL        = true; 
    verbose    = false; 
    Pointer    = 0L; 
    SumLength  = 0;
    Width      = 0;
    LineNumber = 0;
    InternalLineCount      = 0;
};
/******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NOSDecode::ResetPointer()
{
    Pointer    = 0L; 
    BOL        = true; 
    SumLength  = 0;
    LineNumber = 0;
    InternalLineCount      = 0;
}
/**
 ******************************************************************
 *
 * Function Name : Decompress
 *
 * Description : Decompress and interpret data in BigBuffer
 * based on compression type.
 *
 * Inputs : ptr - location in BigBuffer
 *
 * Returns : true if BOL
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool NOSDecode::Decompress(const unsigned char *buffer, 
			   unsigned short IFMValue)
{
    bool          rv;
    int           count   = 0;
    /* Shift is the number of bits used for the Runlength */
    unsigned char shift = (7-IFMValue); 
    unsigned char data;
    unsigned      Line;

    /*
     * data == 0 represents and end of line.
     */
    RunLength = 0;
    Color     = 0;
    Line      = 0;

    rv = false;

    if (Pointer >= FileSize)
    {
	cout << "Pointer exceeds filesize." << endl;
    }
    else if (BOL)
    {
	Line = GetNext(buffer);
	while (Continue && (count<4))
	{
	    Pointer++;
	    Line  = (Line<<7);
	    Line += (unsigned) GetNext(buffer);
	    count++;
	}
	if (Line != LineNumber+1)
	{
	    cout << InternalLineCount << " Line: " << Line 
		 << " Error in line number! " << LineNumber
		 << endl;
	}
	LineNumber = Line;
	BOL = false;
	Pointer++;
	rv = true;
	InternalLineCount++;
    }
    else
    {
	/* Compression is in color. */

	data  = GetNext(buffer);
	Color = data;
	RunLength = 0;
	if (IFMValue < 7)
	{
	    Color = Color>>shift;

	    /* Start unpacking run length. */
	    RunLength = data & ((1<<shift)-1);
	    count++;
	}
	// Loop over all necessary bytes.
	while (Continue && (count<4))
	{
	    Pointer++;
	    RunLength = (RunLength << 7) + GetNext(buffer);
	    count++;
	}
	if(count>3)
	{
	    cout << "ERROR count" << endl;
	}
	Pointer++;
	RunLength++;
	SumLength += RunLength;
	if (verbose)
	{
	    cout << InternalLineCount
		 << " Color: " << (int) Color << " Runlength: " << RunLength
		 << " " << SumLength << endl;
	}
	if (SumLength >= Width)
	{
	    SumLength = 0;
	    // Check to see that we have the prerequisite 0 at EOL
	    if ( buffer[Pointer] != 0)
	    {
		cout << "Line: " << LineNumber
		     << " Did not find 0 at EOL." << (int) buffer[Pointer] 
		     << endl;
	    }
	    Pointer++;
	    BOL = true;
	    if (verbose) 
	    {
		DumpBytes(buffer);
	    }
	}
#if 0
	cout << "Type: " << IFMValue
	     << " Shift: " << (int) shift
	     << " Color: " << (int) Color 
	     << " Run Length: " << RunLength
	     << " Pointer: " << Pointer << endl;
#endif
    }
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NOSDecode::Skip2EOL(const unsigned char *buffer)
{
    while (buffer[Pointer] != 0)
    {
	Pointer++;
    } 
    BOL = true;
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NOSDecode::DumpBytes(const unsigned char *buffer) const
{
    int i;
    for (i=0;i<16;i++)
    {
	printf("%2.2X ",buffer[i+Pointer]);
    }
    printf("\n");
}
/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
NOS_RGB NOS::GetColor(unsigned index)
{
    if (index<0 || index>=rgb.size()) index = 0;

    return *rgb[index];
}
