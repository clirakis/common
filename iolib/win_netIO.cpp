/********************************************************************
 *
 * Module Name : netIO.cpp
 *
 * Author/Date : C.B. Lirakis / 07-Jun-00
 *
 * Description : I am the server. Each time a host tries to make a connection
 *               spawn another thread to take care of it. 
 *               NOTE!!! SocketID is a void pointer in the class definition so
 *               I don't have to include socket stuff all over the place. That
 *               is why I do the wierd stuff with the cast. 
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
 *
 * Copyright (C) BBNT Solutions LLC  2000
 ********************************************************************/

#ifndef lint
static char rcsid[]="$Header$";
#endif
#include <afxwin.h>
// System includes.
#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <list>
#include <io.h>
#include <winsock2.h>
#include <process.h>

// Local Includes.
#include "netIO.h"

struct ConnectionS {
    NetIO *parent;
    SOCKET ChildSocket;
    struct sockaddr_in client;
};


/********************************************************************
 *
 * Function Name : Constructor
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
NetIO::NetIO()
{
    // MS Windows Specific setup here.
	/* Code snippet stolen from help files. */
    WORD wVersionRequested;
    WSADATA wsaData;

    Initialized = false;
    NumberOfCurrentConnections = 0;
    wVersionRequested = MAKEWORD( 2, 0 );
 
    MyErrorCode = WSAStartup( wVersionRequested, &wsaData );
    if ( MyErrorCode != 0 ) 
	{
        // Tell the user that we couldn't find a usable 
        // WinSock DLL.                                 
        return;
	}
 
    /* Confirm that the WinSock DLL supports 2.0.        */
    /* Note that if the DLL supports versions greater    */
    /* than 2.0 in addition to 2.0, it will still return */
    /* 2.0 in wVersion since that is the version we      */
    /* requested.                                        */
 
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
           HIBYTE( wsaData.wVersion ) != 0 ) {
        /* Tell the user that we couldn't find a usable */
        /* WinSock DLL.                                 */
        WSACleanup( );
        MyErrorCode = -1;
        return; 
    }

    // Common code follows. 
    // Continue with the common client and server setup. 
	int option = 16535;          // Junk for the moment, see Stevens, p 315
    SocketFD   = (void *) new SOCKET;     // Nothing Yet. 
    *((SOCKET*) SocketFD)  =  0;

	// Request a TCP socket. Non-zero return good.
	if((*((SOCKET*) SocketFD) = socket(AF_INET,SOCK_STREAM,0)))
	{
        if (setsockopt(*((SOCKET*) SocketFD),SOL_SOCKET,SO_REUSEADDR,(char *) &option,sizeof(option)) < 0)
        {
            MyErrorCode = -3;
        }
        else
        {
            MyErrorCode = 0;  // All is well with the world.
            Initialized = true;
            ServerRunning = false;
        }
    }
    else
    {
        MyErrorCode = -2;
    }
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
bool NetIO::Client(const char *ServerName, const short ConnectPort)
{ 
    /* The WinSock DLL is acceptable. Proceed. */
    struct sockaddr_in server, client; // socket address  
	char hostname[256];                // host name

    if (!Initialized) 
    {
        return false;
    }
    
    if( (MyErrorCode = gethostname(hostname, sizeof(hostname))) != 0 ) 
    {  
        CheckNetErrors("gethostbyname");
        return false;
    }
    
    // clear input data.
    memset((char *) &server,0,sizeof(struct sockaddr_in));
    memset((char *) &client,0,sizeof(struct sockaddr_in));
    
    // setup server options.
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(ServerName);
    server.sin_port        = htons(ConnectPort);  // Server port that we are looking for.
    
    // setup client.
    unsigned short local_port  = 0;
    client.sin_port        = htons(local_port);
    client.sin_family      = AF_INET;
    client.sin_addr.s_addr = inet_addr(hostname);
    
    // Bind to client socket. 
    if( (MyErrorCode = bind(*((SOCKET*) SocketFD), 	
        (struct sockaddr *) &client, sizeof(struct sockaddr_in))) != 0)
    {
        CheckNetErrors("bind");
        return false;
    }
    
    // Setup a socket to a specific port and host.
    if((MyErrorCode = connect(*((SOCKET*) SocketFD), 
        (struct sockaddr *) &server, sizeof(struct sockaddr_in))) != 0)
    {
        CheckNetErrors("Connect");
        return false;
    }
    return true;  // We are ready to do IO with the server. 
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
static UINT HandleConnection(LPVOID pParam)
{
    struct ConnectionS *toPass = (struct ConnectionS *) pParam;
    struct sockaddr_in clientData = toPass->client;
    bool IamRunning = true;
    // While we are still connected, loop and do the thing. 
    while (IamRunning)
    {
        // Wait here for some signal to either transmit or receive.
        Sleep(5000);
        if (toPass->parent->ServerEntryPoint != NULL) {
            toPass->parent->ServerEntryPoint((void *) &toPass->ChildSocket);
        }
    }

    // When we die decrement the number of connections. 
    toPass->parent->DecrementConnectionNumber();
    AfxEndThread(0);
    return 0;
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
    struct sockaddr_in server; // socket address  
	char hostname[256];                // host name
    int struct_size = sizeof(struct sockaddr_in);
    struct ConnectionS toPass;

    if (!Initialized) 
    {
        return false;     // Something did not go well above.
    }

    // We got this far, so we have a valid socket.
    // Who am I??
    if( (MyErrorCode = gethostname(hostname, sizeof(hostname))) != 0 ) 
    {  
        CheckNetErrors("gethostbyname");
        return false;
    }
    
    
    // clear input data.
    memset((char *) &server,0,sizeof(struct sockaddr_in));
    
    // setup server options.
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(hostname); // I am Server.
    server.sin_port        = htons(Port);         // Server port to listen on.

    // Bind to our local address. so that the client can
    // send to us. 
    if( (MyErrorCode = bind(*((SOCKET*) SocketFD), 	
        (struct sockaddr *) &server, sizeof(struct sockaddr_in))) != 0)
    {
        CheckNetErrors("bind");
        return false;
    }
    
    // Wait and listen 
    if ((MyErrorCode = listen (*((SOCKET*) SocketFD), 5)) != 0 )
    {
        CheckNetErrors("listen");
        return false;
    }

    ServerRunning = true;
    toPass.parent = this;
    while (ServerRunning) 
    {
        if ( NumberOfCurrentConnections < MAX_CONNECTIONS) 
        {
            memset(&toPass.client,0, struct_size);
            // Wait here for a connection from a client process. 
            toPass.ChildSocket = 
                accept(*((SOCKET*) SocketFD), (struct sockaddr *)&toPass.client, &struct_size);
        
            // Now we want to start a thread. 
            // We may need to close the parent socket. 
            AfxBeginThread(HandleConnection, // Points to the controlling function for the worker thread.
                (LPVOID) &toPass,            // Data to pass to thread. 
                THREAD_PRIORITY_NORMAL,      // Operational priority. 
                0,                           // Zero, means use same stack size as parent. 
                0,                           // Thread Creation, 0 - start running now!
                NULL );                      // Security Attributes, We don't need no stinking security.

            NumberOfCurrentConnections++;
        }
        else
        {
            // Go to sleep until someone gives up a socket.
            // Please note that this is a MS Windows specific function call. 
            Sleep(15000); // 15 seconds. 
        }

    }
    close (*((SOCKET*) SocketFD));
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
bool NetIO::CheckNetErrors(char *CalledBy)
{
	int err;
	char *text;
	char badboy[256];

	err = WSAGetLastError();
	switch (err)
	{
	case WSAEHOSTDOWN:
		text = "Host is down.";
		break;
	case WSAEHOSTUNREACH:
		text = "Host not reachable.";
		break;
	case WSAEINPROGRESS:
		text = "Operation in progress.";
		break;
	case WSAEINTR:
		text = "Function Interrupted!";
		break;
	case WSAEINVAL:
		text = "Invalid Argument.";
		break;
	case WSAEISCONN:
		text = "Socket already connected.";
		break;
	case WSAEMFILE:
		text = "Too many files open.";
		break;
	case WSAEMSGSIZE:
		text = "Message too long.";
		break;
	case WSAENETDOWN:
		text = "Network DOWN!.";
		break;
	case WSAENETRESET:
		text = "Network RESET!.";
		break;
	case WSAENETUNREACH:
		text = "Network Unreachable!";
		break;
	case WSAENOBUFS:
		text = "No buffer space available.";
		break;
	case WSAENOPROTOOPT:
		text = "Bad protocol option.";
		break;
	case WSAENOTCONN:
		text = "Socket Not Connected.";
		break;
	case WSAENOTSOCK:
		text = "Socket operation on non-socket.";
		break;
	case WSAEOPNOTSUPP:
		text = "Operation not supported.";
		break;
	case WSAEPFNOSUPPORT:
		text = "Protocol family not supported.";
		break;
	case WSAEPROCLIM:
		text = "TOO MANY PROCESSES!!!.";
		break;
	case WSAEPROTONOSUPPORT:
		text = "Protocol not supported.";
		break;
	case WSAEPROTOTYPE:
		text = "Protocol wrong type for socket.";
		break;
	case WSAESHUTDOWN:
		text = "Cannot send after socket shutdown.";
		break;
	case WSAESOCKTNOSUPPORT:
		text = "Socket type not supported.";
		break;
	case WSAETIMEDOUT:
		text = "Connection Timed out.";
		break;
	case WSAEWOULDBLOCK:
		text = "Would block, but resource unavilable.";
		break;
	case WSAHOST_NOT_FOUND:
		text = "Host not found.";
		break;
	case WSA_INVALID_HANDLE:
		text = "The Specified event object handle is invalid.";
		break;
	case WSA_INVALID_PARAMETER:
		text = "One or more parameters are invalid.";
		break;
	case WSANOTINITIALISED :
		text = "Successful WSAStartup not yet performed.";
		break;
	case WSA_NOT_ENOUGH_MEMORY :
		text = "Insufficient memory to complete operation.";
		break;
	case WSA_IO_PENDING:
		text = "Overlapped operations, will complete later.";
		break;
    case WSAEADDRNOTAVAIL:
        text = "Cannot assign requested address.";
		break;
	default:
		text = "ERROR UNKNOWN";
		break;
	}
	sprintf(badboy,"ERROR! %d:\n calling function %s:\n%s",err,CalledBy,text);

    return 0;
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
bool NetIO::SendData(void *data, size_t NumberBytes)
{

    if((MyErrorCode = send(*((SOCKET*) SocketFD),(char *) data,NumberBytes,0)) != 0 ) 
    {
        return false;
    }
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
	// close the socket. 
	close(*((SOCKET*) SocketFD));
    delete SocketFD;
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
bool NetIO::BlockingRead(void *data, size_t NumberBytes)
{
    size_t nread;
    while ( (nread = read (*((SOCKET*) SocketFD), data, NumberBytes)) > 0);
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
bool NetIO::NonBlockRead(void *data, size_t NumberBytes)
{
    return true;
}
