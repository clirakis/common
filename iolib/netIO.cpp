/********************************************************************
 *
 * Module Name : netIO.cpp
 *
 * Author/Date : C.B. Lirakis / 07-Jun-00
 *
 * Description : I am the server. Each time a host tries to make a connection
 *               spawn another thread to take care of it. 
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
 ********************************************************************/
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <string.h>     // strerror
#include <errno.h>      // errno
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>     // for getaddrinfo

// Local Includes.
#include "netIO.hh"
#include "debug.h"


/********************************************************************
 *
 * Function Name : Constructor
 *
 * Description : Create, only open up the socket and set the options. 
 * decide what it's function is in a bit. 
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
NetIO::NetIO()
{
    SET_DEBUG_STACK;
    int    option = 16535;          // Junk for the moment, see Stevens, p 315
    int    rv;

    fError                      = NO_ERROR;
    fNumberOfCurrentConnections = 0;
    ClearReadBuffer();

    rv = gethostname(hostname, sizeof(hostname));
    if( rv == -1 ) 
    {  
	fError = ERROR_HOSTNAME;
	SET_DEBUG_STACK;
	return;
    }

    fSocketFD = socket( AF_INET, SOCK_STREAM, 0);
    if (fSocketFD > -1)
    {
	rv = setsockopt(fSocketFD,SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option));
        if ( rv < 0)
        {
	    SET_DEBUG_STACK;
            fError = ERROR_MODIFYING_ATTRIBUTES;
	    close (fSocketFD);
	    fSocketFD = -1;
        }
        else
        {
            fError    = NO_ERROR;  // All is well with the world.
        }
    }
    else
    {
        fError = ERROR_CONNECTING_SOCKET;
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name : constructor
 *
 * Description : Use with below, the socket exists from a 
 * server perspective, make a new object and pass it along to a 
 * thread to handle. 
 *
 * Inputs : SocketFD - from the connect
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
NetIO::NetIO(int SocketFD)
{
    fError = NO_ERROR;
    fNumberOfCurrentConnections = 0;
    fSocketFD = SocketFD;
}

/********************************************************************
 *
 * Function Name : Client
 *
 * Description :
 *
 * Inputs : ServerName - must be in dot format. ie 139.121.0.1
 *          ConnectPort - Server port to connect to. 
 *
 * Returns :
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NetIO::Client(const char *ServerName, const short Port)
{ 
    SET_DEBUG_STACK;
    int rv;
    /* The socket acceptable. Proceed. */
    struct sockaddr_in server; // socket address  

    fError = NO_ERROR;
    if (fSocketFD>=0) 
    {    
	// clear input data.
	bzero(&server,sizeof(struct sockaddr_in));
	// setup server options.
	server.sin_family      = AF_INET;
	server.sin_port        = htons(Port);  	
	inet_pton( AF_INET, ServerName, &server.sin_addr);
	
	// Setup a socket to a specific port and host.
	rv = connect(fSocketFD,(struct sockaddr *) &server, 
		     sizeof(struct sockaddr_in));
	if(rv == -1)
	{
	    fError = ERROR_CONNECT;
	    SET_DEBUG_STACK;
	    return false;
	}
	SET_DEBUG_STACK;
#if 0
	/* 
	 * One more thing to do, open non-blocking.
	 * fcntl returns -1 on error 
	 */
	rv = fcntl(fSocketFD, F_SETFL, O_NONBLOCK);
	
	if (rv == -1)
	{
	    fError = ERROR_MODIFYING_ATTRIBUTES;
	    return false;
	}
#endif
	return true;  // We are ready to do IO with the server. 
    }
    SET_DEBUG_STACK;
    return false;
}

/********************************************************************
 *
 * Function Name : Server
 *
 * Description : This call does not return until the program shuts
 *               down. The server listens to the specified port then
 *               spawns a new thread to handle each connection. 
 *               
 *
 * Inputs : Port - port to listen on for a connection. 
 *
 * Returns : false if the server never got going. 
 *
 * Error Conditions :
 *
 ********************************************************************/
bool NetIO::Server(const short Port)
{
    SET_DEBUG_STACK;
    int rv;
    struct sockaddr_in server;        // socket address  

    fError = NO_ERROR;
    if (fSocketFD == -1)
    {
	SET_DEBUG_STACK;
	fError = NO_SOCKET;
	return false;
    }
    // clear input data.
    memset((char *) &server,0,sizeof(struct sockaddr_in));
    
    // setup server options.
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(Port);         // Server port to listen on.

    // Bind to our local address. so that the client can
    // send to us. 
    rv = bind(fSocketFD, (struct sockaddr *) &server, 
	      sizeof(struct sockaddr_in));
    if( rv == -1)
    {
	fError = ERROR_BIND;
	close(fSocketFD);
	fSocketFD = -1;
	SET_DEBUG_STACK;
	return false;
    }
    
    // Wait and listen 
    rv = listen(fSocketFD, 5);
    if (rv == -1)
    {
	fError = ERROR_LISTEN;
	SET_DEBUG_STACK;
        return false;
    }
    
    SET_DEBUG_STACK;
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
bool NetIO::Accept()
{
    struct sockaddr_in client;
    socklen_t          length;

    fError    = NO_ERROR;
    length    = sizeof(struct sockaddr_in);
    fAcceptFD = accept( fSocketFD, (sockaddr*)&client, &length);

    if (fAcceptFD == -1)
    {
	switch (errno)
	{
	case EAGAIN:
	    fError = DO_AGAIN;
	    break;
	case EINTR:
	    // OK.
	    fError = RC_INTR;
	    break;
	default:
	    fError = ERROR_ACCEPT;
	    return false;
	    break;
	}
    }
    // At this point we need to spawn a process to deal with the client
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
bool NetIO::SendData(void *data, size_t NumberBytes, int fd)
{
    SET_DEBUG_STACK;
    int         fileDescriptor;
    size_t	nleft;
    ssize_t	nwritten;
    const char* ptr;

    if (fd < 0)
    {
	fileDescriptor = fSocketFD;
    }
    else
    {
	fileDescriptor = fAcceptFD;
    }

    fError = NO_ERROR;
    ptr    = (char *) data;
    nleft  = NumberBytes;

    while (nleft > 0) 
    {
	nwritten = write( fileDescriptor, ptr, nleft);
	if ( nwritten <= 0) 
	{
	    if (errno == EINTR)
	    {
		nwritten = 0;		/* and call write() again */
	    }
	    else
	    {
		fError = ERROR_SEND;
		SET_DEBUG_STACK;
		return false;			/* error */
	    }
	}

	nleft -= nwritten;
	ptr   += nwritten;
    }

    SET_DEBUG_STACK;
    return true;
}
/********************************************************************
 *
 * Function Name : Destructor.
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
NetIO::~NetIO()
{
    SET_DEBUG_STACK;
    fError = NO_ERROR;
    // close the socket. 
    close(fSocketFD);
    SET_DEBUG_STACK;
}
/********************************************************************
 *
 * Function Name : Destructor.
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
bool NetIO::ReadData(void *data, size_t NumberBytes, int fd)
{
    int fileDescriptor;
    int count;

    SET_DEBUG_STACK;
    fError = NO_ERROR;
    
    if (fd < 0)
    {
	fileDescriptor = fSocketFD;
    }
    else
    {
	fileDescriptor = fAcceptFD;
    }
    fReadCount = 0;
again:
    fReadCount = read( fileDescriptor, fReadPointer, 
		       sizeof(fReadBuffer));
    if (fReadCount == 0)
    {
	if (errno == EINTR)
	{
	    goto again;
	}
	fError = NO_READDATA;
	SET_DEBUG_STACK;
	return false;
    }
    else
    {
	if (data != NULL)
	{
	    if (fReadCount < NumberBytes)
	    {
		count = fReadCount;
	    }
	    else
	    {
		count = NumberBytes; 
	    }
	    memcpy( data, fReadPointer, count);
	}
	fReadPointer += fReadCount;
    }
    SET_DEBUG_STACK;
    return true;
}
/**
 ******************************************************************
 *
 * Function Name : NetIO::ClearReadBuffer
 *
 * Description : clear the read buffer and set the count to zero.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 27-Sep-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void NetIO::ClearReadBuffer() 
{
    SET_DEBUG_STACK;
    memset( fReadBuffer, 0, sizeof(fReadBuffer));
    fReadCount   = 0;
    fReadPointer = fReadBuffer;
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name : NetIO::GetErrorString 
 *
 * Description : clear the read buffer and set the count to zero.
 *
 * Inputs : none
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 27-Sep-08
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
const char *NetIO::GetErrorString () const
{
    SET_DEBUG_STACK;
    char *ptr = NULL;
    switch(fError)
    {
    case NO_ERROR:
	ptr = (char*) "NO ERROR!";
	break;
    case NO_SOCKET:
	ptr = (char*) "No socket is open for I/O";
	break;
    case ERROR_NAME_RESOLUTION:
	break;
    case ERROR_OPENING_SOCKET:
	break;
    case ERROR_CONNECTING_SOCKET:
	ptr = (char*) "Could not connect to the socket.";
	break;
    case ERROR_MODIFYING_ATTRIBUTES:
	ptr = (char*) "Error setting socket options.";
	break;
    case ERROR_READ_EXCEEDS_BUFFER:
	break;
    case HOST_CLOSED:
	break;
    case ERROR_CONNECT:
    case ERROR_HOSTNAME:
    case ERROR_BIND:
    case ERROR_LISTEN:
    case ERROR_ACCEPT:
    case ERROR_SEND:
    case ERROR_READ:
	ptr = strerror(errno);
	break;
    case NO_READDATA:
	ptr = (char*) "No data to be read.";
	break;
    case DO_AGAIN:
	ptr = (char*) "Try Accept again.";
	break;
    case RC_INTR:
	ptr = (char*) "Intr. Acceptable error.";
	break;
    default:
	ptr = (char*) "UNKNOWN!";
	break;
    }
    SET_DEBUG_STACK;
    return ptr;
}
