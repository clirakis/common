/**
 ******************************************************************
 *
 * Module Name : queryTimeServer.hh
 *
 * Author/Date : C.B. Lirakis / 13-Jul-14
 *
 * Description : entry points to query a time server. 
 *
 * Restrictions/Limitations : none
 *
 * Change Descriptions :
 *    17-Mar-24   CBL a bit of an overhaul. also put into library. 
 *                    big rewrite, I think the unpacking is wrong. 
 *
 * Classification : Unclassified
 *
 * References :
 *  https://stackoverflow.com/questions/29112071/how-to-convert-ntp-time-to-unix-epoch-time-in-c-language-linux
 * 
 * https://www.meinbergglobal.com/english/info/ntp-packet.htm
 *
 *******************************************************************
 */
#ifndef __QUERYTIMESERVER_hh_
#  define __QUERYTIMESERVER_hh_
#  include <stdint.h>
#  include <arpa/inet.h>
#  include <CObject.hh>

/*
 * NTP uses two fixed point formats.  The first (l_fp) is the "long"
 * format and is 64 bits long with the decimal between bits 31 and 32.
 * This is used for time stamps in the NTP packet header (in network
 * byte order) and for internal computations of offsets (in local host
 * byte order). We use the same structure for both signed and unsigned
 * values, which is a big hack but saves rewriting all the operators
 * twice. Just to confuse this, we also sometimes just carry the
 * fractional part in calculations, in both signed and unsigned forms.
 * Anyway, an l_fp looks like:
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                         Integral Part                         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                         Fractional Part                       |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * REF http://www.eecis.udel.edu/~mills/database/rfc/rfc2030.txt
 *
 * NOTE:
 *     linux epoch is referenced to January 1, 1970 00:00:00
 *     NTP   epoch is referenced to January 1, 1900 00:00:00
 * 
 * Also helpful: 
 *    https://stackoverflow.com/questions/29112071/how-to-convert-ntp-time-to-unix-epoch-time-in-c-language-linux
 *
 * https://www.meinbergglobal.com/english/info/ntp-packet.htm
 *
 * https://tickelton.gitlab.io/articles/ntp-timestamps/
 *
 * https://blog.meinbergglobal.com/2021/02/25/the-root-of-all-timing-understanding-root-delay-and-root-dispersion-in-ntp/
 *
 */


/*
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
 *   Root delay - round trip delay to primary source
 *   root dispersion - maxium error of the local clock relative to the primary
 *                     reference. 
 *   Reference ID: reference ID of source
 *   Reference timestamp - the local time at which the locdal clock was last 
 *                         set or corrected
 *   Originate Timestamp
 *   Recieve timestamp   - the local time which the request arrived at the 
 *                         service host
 *   Transmit Timestamp  - The local time at which the reply departed from 
 *                         the service host for the client. 
 *
 * Online doc say that the offset is calculated by:
 *     Offset = 0.5 * ((T2-T1) + (T3-T4))
 */

/*
 * difference between
 *     linux epoch is referenced to January 1, 1970 00:00:00
 *     NTP   epoch is referenced to January 1, 1900 00:00:00
 */
const uint32_t kNTPOffset = 2208988800; //in article verified 1970-1900.


class ntp_ts {

public:
    /*!
     * Description: 
     *   Constructor for the class using another ntp_ts class.
     *
     * Arguments:
     *   ntp_ts
     *
     * Returns:
     *
     * Errors:
     *     NONE
     */
    ntp_ts(const ntp_ts &val) {fSeconds = val.fSeconds; 
	fFractional=val.fFractional; fMinus=val.fMinus;};

    /*!
     * Description: 
     *   Constructor, but using two uint32_t 
     *   Automatically sets fMinus to false. 
     *
     *
     * Arguments:
     *   s = seconds, could be UNIX epoch of Jan 1, 1970
     *                or NTP epoch Jan 1, 1900
     *
     *
     * Returns:
     *
     * Errors:
     *    NONE
     */
    ntp_ts(uint32_t s=0, uint32_t f=0) {fSeconds =s; fFractional=f; 
	fMinus=false;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */

    /// return value of fSeconds
    inline uint32_t Seconds(void)    const {return fSeconds;};
    /// Set value of fSeconds
    inline void     Seconds(uint32_t v)    {fSeconds = v;};
    /// return value of fractional time. 
    inline uint32_t Fractional(void) const {return fFractional;};
    /// Set value of Fractional time
    inline void     Fractional(uint32_t v) {fFractional=v;};
    /// Return value of rollover. 
    inline bool     Negative(void)   const {return fMinus;};
    /// Set value of rollover (negative value)
    inline void     Negative(bool val)     {fMinus = val;};

    /*!
     * Description: 
     *   convert the values into double seconds.    
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   double value of seconds and fractional seconds. 
     * Errors:
     *   NONE
     */
    double DSeconds(void) const;

    /*!
     * Description: 
     *   populate this structure properly with the current system time. 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    void GetSystemTime(void);

    /*!
     * Description: 
     *   return this in a corrected struct timespec with
     *   the standard unix epoch of Jan 1, 1970.
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    struct timespec Convert(void);

    /// Allow one ntp_ts to be equated with the other. 
    ntp_ts operator = (const ntp_ts &in);
    ntp_ts operator - (const ntp_ts &in) const;
    ntp_ts operator - (uint32_t val) const;

    /*!
     * Description: 
     *   This class holds data in NTP time format referenced from 
     *   the Jan 1, 1900 epoch. This will correct for that
     *   offset and return a ctime string. 
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *    string containing the time of this NTP value
     * Errors:
     *    NONE
     *
     */
    const char *fmt(void);

    friend std::ostream& operator<<(std::ostream& output, const ntp_ts &n);

private:
    uint32_t fSeconds;      // seconds since NTP epoch
    uint32_t fFractional;   // 32 bit word, super fine grained. 
    bool     fMinus;        // set on rollover since these are uint
};
/*!
 * Full NTP packet structure. 
 * This is the way the actual reply is formatted. 
 * The uint32 values need ntohl. 
 */

struct pkt {
    uint8_t  li_vn_mode;     /* leap indicator, version and mode */
    uint8_t  stratum;        /* peer stratum                     */
    uint8_t  ppoll;          /* peer poll interval               */
    int8_t   precision;      /* peer clock precision             */
    uint32_t rootdelay;      /* distance to primary clock (sec)  */
    uint32_t rootdispersion; /* clock dispersion, error   (sec)  */
    uint32_t refid;          /* reference clock ID               */
    uint32_t ref[2];         /* time peer clock was last updated */
    uint32_t org[2];         /* originate time stamp T1          */
    uint32_t rec[2];         /* receive time stamp   T2          */
    uint32_t xmt[2];         /* transmit time stamp  T3          */
};

class QueryTS : public CObject
{
public:
    /*!
     * class Query time server. 
     * give the IPV4 address or name of the timeserver you wish to query
     * and this will allow the user to get the data from that 
     * server as needed. 
     */
    QueryTS(const char* ServerAddress, bool verbose=false);

    ~QueryTS(void);

    /*!
     * Description: 
     *   Query the server specified in the class construction
     *   and populate the relevant fields. 
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *   struct timespec with the returned time value. 
     * 
     * Errors:
     *   NONE
     */
    struct timespec GetTime(void);

    /*!
     * Description: 
     *   Allow access to the full packet. 
     *
     * Arguments:
     *   NONE
     *
     * Returns:
     *    the message returned from the server. 
     *
     * Errors:
     *    NONE
     */
    inline const struct pkt* GetMSG(void) const {return fMSG;};

    /*!
     * Description: 
     *    Return a character string with the host server name.    
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    inline const char* GetServerName(void) const {return fHostAddressName;};
    /*!
     * Description: 
     *   
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */


    /*!
     * Description: 
     *   root delay in numbers we can read, real seconds
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    inline double Drootdelay(void) const {
	return (fFracMult*(double) fMSG->rootdelay);};

    /*!
     * Description: 
     *   root dispersion in numbers we can understand, real seconds 
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    inline double Drootdispersion(void) const {
	return (fFracMult*(double)fMSG->rootdispersion);};

    inline double Precision(void) const {
	return fFracMult*(double)fMSG->precision;};


    /*!
     * Description: 
     *   Calling GetTime on the server populates a field 
     *   that is calculated by the equation. This call
     *   returns the calculated field. It should be the same
     *   as ??
     *
     * Arguments:
     *   
     *
     * Returns:
     *
     * Errors:
     *
     */
    inline double Correction(void) const {return fOffset;};

    /*! Enable a more friendly way of printing the contents of the class. */
    friend std::ostream& operator<<(std::ostream& output, const QueryTS &n);

private:
    struct timespec    PerformQuery(void);
    bool               OpenServer(void);
    void               CloseServer(void);

    double             Subtract(const uint32_t *fp2, const uint32_t *fp1);
    void               Calculate(void);

    char               *fHostAddressName;
    int                fSockfd;
    bool               fVerbose;
    struct sockaddr_in fServaddr;
    struct pkt         *fMSG;      // Full NTP server response. 
    uint32_t           fT4[2];     // time of response from NTP server
    // multiplier to go from NTP fraction to seconds
    double             fFracMult;  
    double             fOffset;    // time difference between client and server
};

#endif
