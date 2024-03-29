/* 
 *
 * Need a list of Public NTP Secondary (stratum 2) Time Servers?
 * http://www.eecis.udel.edu/~mills/ntp/clock2b.html
 *
 * A good reference of the standard:
 * http://www.eecis.udel.edu/~mills/database/rfc/rfc2030.txt
 *
 *  Below is a description of the NTP/SNTP Version 4 message format,
 *  which follows the IP and UDP headers. This format is identical to
 *  that described in RFC-1305, with the exception of the contents of the
 *  reference identifier field. The header fields are defined as follows:
 *
 *                          1                   2                   3
 *      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                          Root Delay                           |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                       Root Dispersion                         |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                     Reference Identifier                      |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                   Reference Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                   Originate Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                    Receive Timestamp (64)                     |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                    Transmit Timestamp (64)                    |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                 Key Identifier (optional) (32)                |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     |                                                               |
 *     |                 Message Digest (optional) (128)               |
 *     |                                                               |
 *     |                                                               |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 *
 *  Reference Timestamp: This is the time at which the local clock was
 *  last set or corrected, in 64-bit timestamp format.
 *
 *  Originate Timestamp: This is the time at which the request departed
 *  the client for the server, in 64-bit timestamp format.
 *
 *  Receive Timestamp: This is the time at which the request arrived at
 *  the server, in 64-bit timestamp format.
 *
 *  Transmit Timestamp: This is the time at which the reply departed the
 *  server for the client, in 64-bit timestamp format.
 *
 * References
 *    https://stackoverflow.com/questions/29112071/how-to-convert-ntp-time-to-unix-epoch-time-in-c-language-linux
 *
 * 28-Mar-24    It appears that querying the serer sometimes hangs. 
 *              can try moving the open into the query and
 *              establishing a timeout on recvfrom
 *
 ***********************************************************************/
#include <iostream>
using namespace std;
#include <ostream>
#include <math.h>
#include <string.h>       // strdup
#include <netdb.h>        // gethostbyname
#include <unistd.h>

// Local includes
#include "queryTimeServer.hh"
#include "debug.h"
#include "CLogger.hh"

/*
 * Time of day conversion constant.  Ntp's time scale starts in 1900,
 * Unix in 1970.
 */
#define JAN_1970        0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define SA      struct sockaddr

extern int h_errno;

/**
 ******************************************************************
 *
 * Function Name : Constructor
 *
 * Description :
 *          Open the inital connection. 
 *
 * Inputs : Server address
 *          how verbose would you like us to be?
 *
 * Returns : NONE
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
QueryTS::QueryTS( const char *TargetAddress, bool verbose) : CObject()
{

    SetVersion(1,1);
    ClearError(__LINE__);
    if (verbose) SetDebug(HIGH);

    fHostAddressName = strdup(TargetAddress);
    fVerbose         = verbose;


    // Much simplier way to allocate this. 
    fMSG= (struct pkt *) calloc( 1, sizeof(struct pkt));
    fFracMult        = pow(2.0, -32);
}

/**
 ******************************************************************
 *
 * Function Name : Destructor, clean up after ourselves. 
 *
 * Description :
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
QueryTS::~QueryTS(void)
{
    free(fMSG);
    free(fHostAddressName);
}
/**
 ******************************************************************
 *
 * Function Name : OpenServer
 *
 * Description :
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool QueryTS::OpenServer(void)
{
    SET_DEBUG_STACK;
    CLogger *pLog = CLogger::GetThis();
    char               **pptr;
    char               str[64];
    struct hostent*    hptr;
    struct timeval timeout = {1,0};

    fSockfd = -1;
    if ((hptr = gethostbyname(fHostAddressName)) == NULL) 
    {
	SetError(-1,__LINE__);
	pLog->LogTime(" gethostbyname error for host: %s: %s",
		    fHostAddressName, hstrerror(h_errno));
	return false;
    }

    if ((hptr->h_addrtype == AF_INET) && (pptr = hptr->h_addr_list) != NULL) 
    {

	inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
    } 
    else
    {
	SetError(-2,__LINE__);
	pLog->LogTime("Error call inet_ntop \n");

	return false;
    }

    fSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fSockfd == -1)
    {
	SetError( -3, __LINE__);
	pLog->LogTime("Error in socket \n");
	return false;
    }
    bzero(&fServaddr, sizeof(fServaddr));
    fServaddr.sin_family = AF_INET;
    fServaddr.sin_port = htons(123);
    inet_pton(AF_INET, str, &fServaddr.sin_addr);

    if (connect(fSockfd, (SA *) & fServaddr, sizeof(fServaddr)) == -1 )
    {
	SetError(-4, __LINE__);
	pLog->LogTime("Error in connect \n");
	return false;
    }

    if (setsockopt(fSockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, 
		   sizeof(timeout)) <0)
	pLog->LogTime("Error setting recieve timeout\n");
    
    SET_DEBUG_STACK;
    return true;
}

/**
 ******************************************************************
 *
 * Function Name : OpenServer
 *
 * Description :
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void QueryTS::CloseServer(void)
{
    SET_DEBUG_STACK;
    close(fSockfd);
    fSockfd = -1;
}
/**
 ******************************************************************
 *
 * Function Name : GetTime
 *
 * Description : Do a time query on the server.
 * Here are the various time measurements:
 *
 * Notes on packet
 * Leap indicator 2 bits wide. 
 * Version number 3 bits wide.
 * Mode (3 bits):
 *   0 - reserved
 *   1 - symmetric active
 *   2 - symmetric passive
 *   3 - client
 *   4 - server
 *   5 - broadcast or multicast.
 *   6 - ntp control message
 *   7 - future use. 
 *
 *   Stratum 8 bit intenger 1 - most precise, 16 least precise. 
 *   Poll - 8 bit signed maxium interval between success messages
 *   Precision - 8 bit signed 
 *   Root delay - round trip delay to primary source (this should be offset)
 *   root dispersion - maxium error of the local clock relative to the primary
 *                     reference. 
 *   Reference ID: reference ID of source
 *   Reference timestamp - the local time at which the local clock was last 
 *                         set or corrected
 *   Originate Timestamp
 *   Recieve timestamp   - the local time which the request arrived at the 
 *                         service host (T2)
 *   Transmit Timestamp  - The local time at which the reply departed from 
 *                         the service host for the client. (T3)
 *
 * Online doc say that the offset is calculated by:
 *     Offset = 0.5 * ((T2-T1) + (T3-T4))

 *
 * Inputs : NONE
 *
 * Returns : struct timespec from the server. 
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 17-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
struct timespec QueryTS::GetTime(void)
{
    struct timespec rv = {0,0};
    if(OpenServer())
    {
	rv = PerformQuery();
	CloseServer();
    }
    return rv;
}
struct timespec QueryTS::PerformQuery(void)
{
    /* 
     * Overall packet length, 
     * See: https://www.meinbergglobal.com/english/info/ntp-packet.htm
     * 12 x 32bit words for 48 bytes total. 
     */
    const int len     = 48;
    struct timespec   rv = {0,0};   // return value
    int servlen       = sizeof(fServaddr);
    struct sockaddr *pcliaddr = (struct sockaddr *) &fServaddr;

    /*
     * Client side send. 
     */
    struct pkt Client;
    memset(&Client, 0, sizeof(struct pkt));
    Client.li_vn_mode = 0L;
    ntp_ts val;
    val.GetSystemTime();
    Client.org[0] = val.Seconds();
    Client.org[1] = val.Fractional();
    /*
     * Allocate a byte array and then convert as necessary. 
     */
    //uint8_t data[len];

    sendto(fSockfd, (char *) &Client, len, 0, pcliaddr, servlen);

    // Receive the response
    memset (fMSG, 0, sizeof(struct pkt));
    /* 
     * see how well this unpacks - 18-Mar-24, unpacks well into the structure
     * upto the long values. not sure what is going on there. 
     */
    size_t rbytes = recvfrom(fSockfd, fMSG, len, 0, NULL, NULL);
    if (rbytes>0)
    {
	// Do some swaps. 
	// rootdelay and rootdispersion apparently don't need the conversion. 
	// fMSG->rootdelay      = ntohl(fMSG->rootdelay);
	// fMSG->rootdispersion = ntohl(fMSG->rootdispersion);

	fMSG->ref[0] = ntohl(fMSG->ref[0]);
	fMSG->ref[1] = ntohl(fMSG->ref[1]);
	fMSG->rec[0] = ntohl(fMSG->rec[0]);
	fMSG->rec[1] = ntohl(fMSG->rec[1]);
	fMSG->xmt[0] = ntohl(fMSG->xmt[0]);
	fMSG->xmt[1] = ntohl(fMSG->xmt[1]);

	// Put in origin clock.  
	fMSG->org[0] = val.Seconds();
	fMSG->org[1] = val.Fractional();

	ntp_ts tt;
	tt.GetSystemTime();
	fT4[0] = tt.Seconds();
	fT4[1] = tt.Fractional();

	// Calculate the offset between the host and server.
	Calculate();

	if(Debug(CObject::LOW))
	{
	    char buffer[30];
	    strftime(buffer,30,"%m-%d-%Y  %T",localtime(&rv.tv_sec));
	    fprintf(stderr,"xmt: %s.%u\n",buffer,(unsigned)rv.tv_nsec);
	}
    }
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : Subtract
 *
 * Description : Private helper function to subtract two 
 *               NTP time values and return the double value. 
 *
 * Inputs : fp2 - 2 word array containing seconds and fraction
 *          fp1 - 2 word array containing seconds and fraction
 *
 * Returns : double value difference (fp2 - fp1)
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double QueryTS::Subtract(const uint32_t *fp2, const uint32_t *fp1)
{
    ntp_ts two(fp2[0], fp2[1]);
    ntp_ts one(fp1[0], fp1[1]);
    ntp_ts res = two-one;

    return res.DSeconds();
}

/**
 ******************************************************************
 *
 * Function Name : Calculate
 *
 * Description : Determine, from the NTP equation, what is the
 *               time difference from the server and the host. 
 *               Fills the fOffset value. This is a private function
 *
 * Inputs : NONE
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void QueryTS::Calculate(void)
{
    double T2MinusT1 = Subtract( fMSG->rec, fMSG->org);
    double T4MinusT3 = Subtract( fT4, fMSG->xmt);
    fOffset = 0.5 * (T2MinusT1 + T4MinusT3);
}

/**
 ******************************************************************
 *
 * Function Name : operator <<
 *
 * Description : formatted output of QueryTS, tells everything
 *               about the last query of the NTP server
 *               specified in the construcor
 *
 * Inputs : ostream and QueryTS. 
 *
 * Returns : fully populated ostream
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: NONE
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const QueryTS &n)
{
    char REFID[8];
    // offset corrected recieve time at server. 
    uint32_t rcv     = n.fMSG->rec[0] - kNTPOffset;
    time_t   rcvtime = rcv;

    memset(REFID, 0, sizeof(REFID));
    memcpy(REFID, &n.fMSG->refid, 4);

    output << " ======================================= "        << endl
	   << " NTP server: " << n.fHostAddressName              << endl
	   << "    li_vn_mode: " << hex << (int) n.fMSG->li_vn_mode << endl 
	   << "       stratum: " << dec << (int) n.fMSG->stratum << endl
	   << "         ppoll: " << (int) n.fMSG->ppoll          << endl
	   << "     precision: " << (int) n.fMSG->precision      << endl
	   << "     rootdelay: " << (int) n.fMSG->rootdelay
	   << "  "  << n.Drootdelay()      << endl
	   << "rootdispersion: " << (int) n.fMSG->rootdispersion 
	   << "  " <<  n.Drootdispersion() << endl
	   << "         refid: " << REFID          << endl
	   << " -----------------------------------------"       << endl
	   << "          ref: " << n.fMSG->ref[0] << "  " << n.fMSG->ref[1] << endl

	   << "    org (T1) : " << n.fMSG->org[0] << "  " << n.fMSG->org[1] << endl

	   << "    rec (T2) : " << n.fMSG->rec[0] 
	   << "  " << n.fMSG->rec[1] << endl

	   << " recieve FMT: " << ctime(&rcvtime)
	
	   << "    xmt (T3): " << n.fMSG->xmt[0]
	   << "  " << n.fMSG->xmt[1] << endl
	   << " total response: " << n.Correction() << endl
	   << " DELTA: " << (n.fMSG->rec[0] - n.fMSG->org[0]) << endl
	   << " ======================================= " << endl;

    return output;
}
/**
 ******************************************************************
 *
 * Function Name : GetSystemTime
 *
 * Description : return current clock time in ntp time format. 
 *
 * Inputs : NONE
 *
 * Returns : System time in NTP format seconds and fractional
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void ntp_ts::GetSystemTime(void)
{
    ntp_ts            rv;
    struct timespec   t1;
    uint64_t          tmp;

    // timestamp local PC time. 
    clock_gettime(CLOCK_REALTIME, &t1);   // Host time
    struct tm *local = localtime(&t1.tv_sec);
    uint32_t GMToff  = local->tm_gmtoff;

    /* 
     *  Correct for GMT offset AND the fact
     * that system time epoch is Jan 1, 1970 and NTP is 1900. 
     * 
     */
    fSeconds = t1.tv_sec + GMToff + kNTPOffset;

    /* 
     * Now convert from ns to this weird format used by ntp. 
     */
    tmp = t1.tv_nsec;
    // Shift left 32 bits. 
    tmp <<= 32;
    // divide by 1e9
    tmp /= 1000000000;

    fFractional = tmp;
}

/**
 ******************************************************************
 *
 * Function Name : DSeconds
 *
 * Description : Double value of seconds stored
 *
 * Inputs : NONE
 *
 * Returns : Double value of seconds since NTP epoch
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
double ntp_ts::DSeconds(void) const
{
    const double kFracmult    = pow(2.0, -32);
    double val = ((double)fSeconds + kFracmult * (double)fFractional);

    if (fMinus) val *= -1.0;

    return val; 
}
/**
 ******************************************************************
 *
 * Function Name : Convert
 *
 * Description : Convert from NTP time values with epoch to 
 *               standard UNIX epoch using struct timespec
 *
 * Inputs : NONE
 *
 * Returns : struct timespec seconds and nanoseconds since epoch
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
struct timespec ntp_ts::Convert(void)
{
    struct timespec rv;
    uint64_t conv = fFractional;
    conv *= 1000000000;   // multiply by nanoseconds
    conv >>= 32;          // divide by 2^32

    rv.tv_sec  = fSeconds - kNTPOffset;
    rv.tv_nsec = conv;
    return rv;
}

/**
 ******************************************************************
 *
 * Function Name : operator -
 *
 * Description : Subtract two ntp_ts values. Done as 64 bit
 *               number to avoid issues. 
 *
 * Inputs : npt_ts value to subtract. 
 *
 * Returns : new ntp_ts value. 
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
ntp_ts ntp_ts::operator - (const ntp_ts &in) const
{
    int64_t number1;
    int64_t number2;
    int64_t result;

    number1 = fSeconds;
    number1 <<= 32;
    number1 += fFractional;

    number2 = in.fSeconds;
    number2 <<= 32;
    number2 += in.fFractional;

    result = number1 - number2; 

    ntp_ts rv;

    if (result<0)
    {
	result *= -1;
	rv.Seconds   (result>>32);
	rv.Fractional(result&0xFFFFFFFF);
	rv.Negative  (true);
    }
    else
    {
	rv.Seconds   ( result>>32);
	rv.Fractional( result&0xFFFFFFFF);
    }
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : operator -
 *
 * Description : Subtract two ntp_ts values. 
 *               Slight issue since they are uint32_t they don't
 *               have a sign and I have arbitrarily set this up
 *               to reverse the order. 
 *
 * Inputs : uint32_t value to subtract from seconds field. 
 *
 * Returns : ntp_ts value after subtracting the specified number of seconds
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 28-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ntp_ts ntp_ts::operator - (uint32_t val) const
{
    uint32_t itemp;
    if (val<fSeconds)
	itemp = fSeconds-val;
    else
	itemp = val - fSeconds;

    ntp_ts temp( itemp, fFractional);
    return temp;
}

/**
 ******************************************************************
 *
 * Function Name : operator =
 *
 * Description : equate two ntp_ts classes
 *
 * Inputs : RHS of equate
 *
 * Returns : LHS of equate
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ntp_ts ntp_ts::operator = (const ntp_ts &in)
{
    fSeconds    = in.fSeconds;
    fFractional = in.fFractional;
    fMinus      = in.fMinus;
    return *this;
}
/**
 ******************************************************************
 *
 * Function Name : fmt
 *
 * Description : create a ctime from the ntp_ts value
 *
 * Inputs : NONE
 *
 * Returns : character stream, not reentrant. 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 18-Mar-24
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char* ntp_ts::fmt(void)
{
    uint32_t val = fSeconds - kNTPOffset;
    time_t   val2 = val;
    return ctime(&val2);
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

/**
 ******************************************************************
 *
 * Function Name : operator <<
 *
 * Description : Create an ostream given an ntp_ts value
 *
 * Inputs : input ostream and ntp_ts to fill it with
 *
 * Returns : ostream 
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, const ntp_ts &n)
{
    output << "seconds: "   << n.fSeconds 
	   << " Fraction: " << n.fFractional
	   << " value: "    << n.DSeconds();
    return output;
}
