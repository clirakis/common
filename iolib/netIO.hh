/********************************************************************
 *
 * Module Name : netIO.h
 *
 * Author/Date : C.B. Lirakis / 07-Jun-00
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
 * RCS header info.
 * ----------------
 * $RCSfile$
 * $Author$
 * $Date$
 * $Locker$
 * $Name$
 * $Revision$
 *
 * $Log: $
 *
 *********************************************************************************/
#ifndef __NETIO_hh_
#define __NETIO_hh_

// Allow no more than 5 simultaneous connections. 
const int MAX_CONNECTIONS = 5;

class NetIO {
public:
    NetIO();
    /// Use this to spawn a server connection
    NetIO(int SocketFD);
    ~NetIO();
    unsigned char GetData();
    unsigned char GetData(const unsigned char *buf, const size_t size);
    bool  SendData(void *data, size_t NumberBytes, int fd=-1);
    bool  ReadData(void *data, size_t NumberBytes, int fd=-1);
    bool  Client(const char *ServerName, const short ConnectPort);
    bool  Server(const short Port);
    bool  Accept();
    inline int   GetAcceptFD() const {return fAcceptFD;};
    inline int   GetError() const { return fError;};
    const  char* GetErrorString () const;
    inline int   GetAcceptFD() {return fAcceptFD;};
    inline int   GetReadCount() const {return fReadCount;};
    void  DecrementConnectionNumber() {fNumberOfCurrentConnections--;};
    void  ClearReadBuffer();

    enum {NO_ERROR, ERROR_NAME_RESOLUTION, ERROR_OPENING_SOCKET,
	  ERROR_CONNECTING_SOCKET, ERROR_MODIFYING_ATTRIBUTES,
	  ERROR_READ_EXCEEDS_BUFFER, ERROR_CONNECT, HOST_CLOSED, 
    	  ERROR_HOSTNAME, ERROR_BIND, ERROR_LISTEN, ERROR_SEND, ERROR_READ,
	  NO_SOCKET, NO_READDATA, ERROR_ACCEPT, DO_AGAIN, RC_INTR};

private:
    // Private Methods

    // Private data
    int    fSocketFD, fAcceptFD;
    int    fError;
    // Tracking for read
    size_t fReadCount;
    char*  fReadPointer;
    char   fReadBuffer[512];
    char   hostname[128];
    bool   fServerRunning;
    int    fNumberOfCurrentConnections;
    
};
#endif
