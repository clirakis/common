/********************************************************************
 *
 * Module Name : Buffered.cpp
 *
 * Author/Date : C.B. Lirakis / 15-Mar-05
 *
 * Description : Generic Buffered
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 ********************************************************************/
// System includes.
#include <iostream>
#include <string>
//#include <cmath>
#include <ostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <iomanip>
using namespace std;
#ifdef __APPLE__
#  include <sys/time.h>
#endif

// Local Includes.
#include "debug.h"
#include "Buffered.hh"

/**
 ******************************************************************
 *
 * Function Name : Buffered constructor
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
Buffered::Buffered (unsigned short N)
{
    SET_DEBUG_STACK;
    fdata           = new unsigned char[N];
    fdrain          = fdata;
    fFillIndex      = 0;
    fSize           = N;
    UpdateNow();
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : Buffered destructor
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
Buffered::~Buffered (void)
{
    SET_DEBUG_STACK;
    delete fdata;
}

/**
 ******************************************************************
 *
 * Function Name : Put
 *
 * Description : Put a single character into the buffer and update everything
 *
 * Inputs : val - byte to store. 
 *
 * Returns : current buffer pointer
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
int32_t Buffered::Put(unsigned char val)
{
    SET_DEBUG_STACK;
    fError = kERROR_NONE;

    // First character put, timestamp on this sentance. 
    if (fFillIndex == 0)
	UpdateNow();

    // Last time buffer was touched. 
    fTimeStamp = clock();
    
    fdata[fFillIndex] = val;
    fFillIndex        = (fFillIndex+1)%fSize;
    if (fFillIndex == 0)
    {
	fError = kBUFFER_OVERFLOW;
    }
    SET_DEBUG_STACK;
    return fFillIndex;
}
/**
 ******************************************************************
 *
 * Function Name : PutBuffer
 *
 * Description : put one big buffer into this format. 
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
int Buffered::PutBuffer(unsigned char *b, size_t s)
{
    SET_DEBUG_STACK;
    fError = kERROR_NONE;
    UpdateNow();
    memcpy(fdata, b, s);
    fdrain = fdata;
    fFillIndex += s;
    SET_DEBUG_STACK;
    return fFillIndex;
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
void Buffered::SetTime(void)
{
    SET_DEBUG_STACK;
    fError     = kERROR_NONE;
    UpdateNow();
    fTimeStamp = clock();
}
/**
 ******************************************************************
 *
 * Function Name : GetSingle
 *
 * Description : 4 byte float in IEEE floating point format
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
float Buffered::GetSingle(void)
{
    SET_DEBUG_STACK;
    float rv;
    unsigned char *d  = (unsigned char *) &rv;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = 0.0;
    }
    else if (Remaining() < sizeof(float))
    {
	rv = 0.0;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	d[0] = fdrain[3];
	d[1] = fdrain[2];
	d[2] = fdrain[1];
	d[3] = fdrain[0];
	SET_DEBUG_STACK;
	fdrain += sizeof (float);
    }
    SET_DEBUG_STACK;
    return rv;
};
/**
 ******************************************************************
 *
 * Function Name : GetInt
 *
 * Description : 2 byte integer
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
short Buffered::GetInt (void)
{
    SET_DEBUG_STACK;
    short rv;
    unsigned char *d = (unsigned char *) &rv;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = 0;
    }
    else if (Remaining() < sizeof(short))
    {
	rv = 0;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	d[0] = fdrain[1];
	d[1] = fdrain[0];
	SET_DEBUG_STACK;
	fdrain += sizeof (short);
    }
    SET_DEBUG_STACK;
    return rv;
};
/**
 ******************************************************************
 *
 * Function Name : GetLongInt
 *
 * Description : 4 byte integer
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
int Buffered::GetLongInt (void)
{
    SET_DEBUG_STACK;
    int rv;
    unsigned char *d = (unsigned char *) &rv;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = 0;
    }
    else if (Remaining() < sizeof(int))
    {
	rv = 0;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	d[0] = fdrain[3];
	d[1] = fdrain[2];
	d[2] = fdrain[1];
	d[3] = fdrain[0];
	SET_DEBUG_STACK;
	fdrain += sizeof (int);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : GetDouble
 *
 * Description : 8 byte IEEE floating point
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
double Buffered::GetDouble (void)
{
    SET_DEBUG_STACK;
    double rv;
    unsigned char *d  = (unsigned char *) &rv;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = 0.0;
    }
    else if (Remaining() < sizeof(double))
    {
	rv = 0.0;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	d[0] = fdrain[7];
	d[1] = fdrain[6];
	d[2] = fdrain[5];
	d[3] = fdrain[4];
	d[4] = fdrain[3];
	d[5] = fdrain[2];
	d[6] = fdrain[1];
	d[7] = fdrain[0];
	SET_DEBUG_STACK;
	fdrain += sizeof (double);
    }
    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : GetChar
 *
 * Description : Get a character from the buffer, advance the pointer.
 *
 * Inputs : none
 *
 * Returns : character obtained from buffer
 *
 * Error Conditions : Set the error status if there are insufficent bytes
 * in the buffer.
 * 
 * Unit Tested on: 29-June-08 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
unsigned char Buffered::GetChar (void)
{
    SET_DEBUG_STACK;
    char rv;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = 0;
    }
    else if (Remaining() < sizeof(char))
    {
	rv = 0;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	SET_DEBUG_STACK;
	rv     = *fdrain;
	fdrain += sizeof (char);
    }
    SET_DEBUG_STACK;
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
int Buffered::Skip(unsigned i)
{
    int rv = 0;

    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rv = -1;
    }
    else if ( Remaining() < i)
    {
	rv    = -1;
	fError = kBUFFER_EMPTY;
    }
    else
    {
	fdrain += i;
    }
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : ostream operator
 *
 * Description : Dump hex data from buffer. 
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
ostream& operator<<(ostream& output, const Buffered &n)
{
    SET_DEBUG_STACK;
    unsigned short i;
    output << "# Buffered ----------------------------------------" 
	   << dec << endl;
    output << "#         Size: " << n.fSize 
	   << "    FillIndex: " << n.fFillIndex
	   << "         Busy: " << n.fBusy
	   << "        Error: " << n.fError
	   << std::right << std::setw(2) << hex << endl;
    for (i=0;i<n.fFillIndex;i++)
    {
        if (i%10 == 0)
        {
	    if (i>0) output << endl;
            output << "# " << dec << std::setw(2) << i << ") " << hex;
        }
	output << hex << std::setw(2) << (int) n.fdata[i] << " ";
    }

    output << endl << dec 
		   << "# ------------------------------------------" << endl;


    SET_DEBUG_STACK;
    return output;
}

void Buffered::Reset()
{ 
    fError     = kERROR_NONE;
    fFillIndex = 0;          // Nothing has been put into the buffer.
    fdrain     = fdata;      // drain starts at beginning of buffer. 
    fBusy      = false;      // buffer is not busy.
    memset(fdata, 0, fSize); // Zero buffer.
    UpdateNow();
}

unsigned char Buffered::Char(unsigned i)
{
    unsigned char rc = 0;
    fError = kERROR_NONE;
    if (!Check())
    {
	fError = kBUFFER_BUGGERED;
	rc = 0;
    }
    else if (i<fSize)
    {
	rc = fdata[i];
    }
    return rc;
} 

void Buffered::ClearBusy(void)  
{
    fBusy = false;
    SetTime();
}

const char *Buffered::StatusBuf(void) const
{
    static char StatusB[256];
    sprintf(StatusB, " Fill index: %d Drain: %d ",
	    fFillIndex, Remaining());
    return StatusB;
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
int Buffered::GetLine(char *p, size_t n)
{
    size_t i = 0;
    char s;

    do
    {
	s    = (char )GetChar();
	p[i] = s;
	i++;
    } while ((i<n) && (fError==kERROR_NONE) && ((s!='\n')|| (s!='\r')));
    
    return i;
}
void Buffered::UpdateNow(void)
{
#ifdef __APPLE__
    struct timeval tv;
    struct timezone tz;
    struct tm *tm;
    gettimeofday (&tv, &tz);
    tm = gmtime(&tv.tv_sec);
    fnow.tv_sec  = tv.tv_sec;
    fnow.tv_nsec = tv.tv_usec * 1000; 
#else
    clock_gettime(CLOCK_REALTIME, &fnow);
#endif
}
