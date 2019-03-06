
#include "QServerSocket.h"
#include "QAddress.h"
#include "QClientSocket.h"
#include "Applog.h"
#include <string.h>
#ifdef _MSC_VER
#elif defined __GNUG__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#endif
namespace utility {

int QServerSocket::bindingAddr(const QAddress & addr, bool isUDP) 
{
    if(state_ != eINITIAL) {
        return -1;
    }
#ifdef _MSC_VER
    struct addrinfo *result = NULL, *ptr = NULL,hints;    

    int iResult = 0;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;

	//set addinfo according protocol type
	if (isUDP)
	{
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
	}
	else
	{
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
	}

	hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, addr.port.c_str(), &hints, &result);
    if (iResult != 0) {
        state_ = eINVALID;       
        DEBUG_PRINT1("getaddrinfo failed: %d\n", iResult);
        commonErrCodeHandle(iResult, "getaddrinfo", __FUNCTION__);
        return -1;
    }    

    socket_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (socket_ == INVALID_SOCKET) {
        state_ = eINVALID;       
        DEBUG_PRINT1("Error at socket(): %ld\n", WSAGetLastError());
        commonErrCodeHandle(WSAGetLastError(), "socket", __FUNCTION__);
        freeaddrinfo(result);
        return -1;
    }

	int size = 1;
	if (!isUDP) {
		iResult = setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*) &size, (socklen_t)sizeof(size));
		if (iResult == -1)
		{      
			commonErrCodeHandle(errno, "setsockopt", __FUNCTION__);
		}
	}
    // Setup the TCP listening socket
    iResult = bind( socket_, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        state_ = eINVALID;       
        int err_code = WSAGetLastError();
        DEBUG_PRINT1("bind failed with error: %d\n", err_code);
        commonErrCodeHandle(err_code, "bind", __FUNCTION__);
        freeaddrinfo(result);
        closesocket(socket_);
        return -1;
    }
    freeaddrinfo(result);
#elif defined __GNUG__
    struct sockaddr_in serv_addr;
	if (isUDP) {
		socket_ = socket(AF_INET, SOCK_DGRAM, 0);
	} else {
		socket_ = socket(AF_INET, SOCK_STREAM, 0);
	}
    
	if(socket_ < 0) {
       state_ = eINVALID;       
       commonErrCodeHandle(h_errno, "bind", __FUNCTION__); 
       return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    int portno = atoi(addr.port.c_str());
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(socket_, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) {
       state_ = eINVALID;       
       commonErrCodeHandle(h_errno, "bind", __FUNCTION__);
       return -1;
    }
#endif
    state_ = eBIND;
    return 0;
}

int QServerSocket::listenConnection(int conn_num) {
    if(state_ != eBIND && state_ != eLISTEN) {
        return -1;
    }
#ifdef _MSC_VER
    if(listen( socket_, conn_num ) == SOCKET_ERROR) {
        state_ = eINVALID; 
        //DEBUG_PRINT1( "Listen failed with error: %ld\n", WSAGetLastError() );
        commonErrCodeHandle(WSAGetLastError(), "listen", __FUNCTION__);
        closesocket(socket_);
        //WSACleanup();
        return -1;
    }
#elif defined __GNUG__
    if(listen(socket_, conn_num )) {
        state_ = eINVALID; 
        commonErrCodeHandle(h_errno, "listen", __FUNCTION__);
		//修改说明，不用shutdown无法关闭阻塞的socket
		shutdown(socket_, SHUT_RDWR);
		//close(socket_);
        return -1;
    }
#endif
    state_ = eLISTEN; 
    return 0;
}

QClientSocket* QServerSocket::acceptClient() {
	if(state_ != eLISTEN) { 
       return 0;
    }
#ifdef _MSC_VER
    SOCKET ClientSocket;
    ClientSocket = INVALID_SOCKET;
    // Accept a client socket
    ClientSocket = accept(socket_, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        int err_code = WSAGetLastError();
        DEBUG_PRINT1("accept failed: %d\n", err_code);
		//Applog::log(Applog::LOG_CRITICAL, __FUNCTION__, "accept failed"); 
		APP_LOG(Applog::LOG_CRITICAL) << __FUNCTION__ << "accept failed";
        commonErrCodeHandle(err_code, "accept", __FUNCTION__);
        closesocket(socket_);
        return 0;
    } else {
        QClientSocket* cl = new QClientSocket();
        cl->socket_ = ClientSocket;
        cl->state_ = QClientSocket::eCONNECT;

		fd_set read_set;
		FD_ZERO(&read_set);  
        FD_SET(ClientSocket, &read_set);

        return cl;
    }
#elif defined __GNUG__
   struct sockaddr_in cli_addr;
   socklen_t client;
   int new_socket_ = accept(socket_, (struct sockaddr*) &cli_addr, & client);
   if(new_socket_<0) {
        commonErrCodeHandle(h_errno, "accept", __FUNCTION__);
        return 0;
   } else {
        QClientSocket* cl = new QClientSocket();
        cl->socket_ = new_socket_;
        cl->state_ = QClientSocket::eCONNECT;

		fd_set read_set;
		FD_ZERO(&read_set);  
        FD_SET(new_socket_, &read_set);

        return cl;
   } 
#endif
}

QServerSocket::QServerSocket() {
    state_ = eINITIAL;
#ifdef _MSC_VER
    socket_ = INVALID_SOCKET;
#endif
}

QServerSocket::~QServerSocket() {
#ifdef _MSC_VER
    closesocket(socket_);
#elif defined __GNUG__
    close(socket_);
#endif
}
}

