
#include "QClientSocket.h"
#include "QAddress.h"
#include "Applog.h"

#ifdef _MSC_VER
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined __GNUG__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#endif

namespace utility {

#ifdef _MSC_VER
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

	//DO NOT MODIFY
#define  REC_LEN_MAX 16


	int QClientSocket::connectServer(const QAddress & addr) 
	{
		if(state_ != eINITIAL) { 
			return -1;
		}
#ifdef _MSC_VER

		struct	addrinfo *result = NULL,
			*ptr = NULL,
			hints;

		int iResult=0;
		int err_code =0;

		ZeroMemory( &hints, sizeof(hints) );
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(addr.host_name.c_str(), addr.port.c_str(), &hints, &result);

#define RETRY_TIMES  3
		if ( iResult != 0 ) {  
			state_ = eINVALID;
			err_code = WSAGetLastError();
			switch(err_code)
			{
			case WSATRY_AGAIN:
				{
					int try_count  =1;
					while(try_count++ <= RETRY_TIMES && iResult && err_code == WSATRY_AGAIN)
					{
						//Applog::log(Applog::LOG_DEBUG_LIB, "QClientSocket::connectServer, getaddrinfo retry: ", try_count);

						iResult= getaddrinfo(addr.host_name.c_str(), addr.port.c_str(), &hints, &result);
						if(iResult)
						{
							err_code = WSAGetLastError();
						}else
						{
							break;
						}
					}
					if(iResult)
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, getaddrinfo: failed after retry, err_code:", err_code);
					}
					break;
				}
			case WSAEINVAL:
				{
					//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer,getaddrinfo: invalid ai flags! err_code:", err_code);				
					break;
				}

			default:
				{
					//Error is handled by  commonErrCodeHandle()
					commonErrCodeHandle(err_code, "getaddrinfo", "QClientSocket::connectServer");
				}
			}

			return -1;
		}

		// Attempt to connect to an address until one succeeds
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

			// Create a SOCKET for connecting to server
			socket_ = socket(ptr->ai_family, ptr->ai_socktype, 
				ptr->ai_protocol);
			if (socket_ == INVALID_SOCKET) {
				err_code = WSAGetLastError();
				//Applog::log(Applog::LOG_ERROR, "socket() failed with error code: ", err_code);
				switch(err_code)
				{			
				case WSAEAFNOSUPPORT:
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, socket()", 
						//    "The specified address is not supported.");
						break;
					}			

				case WSAEINVAL:
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, socket()", 
						//    "invalid argument was supplied.");
						break;
					}	

				default:
					{
						//Error is handled by  commonErrCodeHandle()
						commonErrCodeHandle(err_code, "socket", "QClientSocket::connectServer");
					}
				}
				freeaddrinfo(result);
				return -1;
			}

			// Connect to server.
			iResult = connect( socket_, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				err_code = WSAGetLastError();
				switch(err_code)
				{
				case WSAEAFNOSUPPORT:
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, connect()", 
						//    "Addresses in the specified family cannot be used with this socket.");
						break;
					}
				case WSAEFAULT:
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, connect()", 
						//    "Addresses in the specified family cannot be used with this socket.");
						break;
					}
				case WSAEINVAL:
					{
						//Applog::log(Applog::LOG_ERROR, "QClientSocket::connectServer, connect()", 
						//    "The parameter is a listening socket.");
						break;
					}
				default:
					{
						//Error is handled by  commonErrCodeHandle()
						commonErrCodeHandle(err_code, "connect", "QClientSocket::connectServer");
					}
				}
				closesocket(socket_);
				socket_ = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (socket_ == INVALID_SOCKET) {
			DEBUG_PRINT0("Unable to connect to server!\n");
			state_ = eINVALID;
			return -1;
		}
#elif defined __GNUG__
		int portno;
		struct sockaddr_in serv_addr;
		struct hostent *server;

		portno = atoi(addr.port.c_str());
		socket_ = socket(AF_INET, SOCK_STREAM, 0);

		if (socket_ < 0) 
		{
			state_ = eINVALID;
			commonErrCodeHandle(h_errno, "connect", "QClientSocket::connectServer");
			return -1;
		}
		server = gethostbyname(addr.host_name.c_str());
		if (server == NULL) {
			state_ = eINVALID;
			commonErrCodeHandle(h_errno, "connect", "QClientSocket::connectServer");
			return -1;
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
		serv_addr.sin_port = htons(portno);
		int err;
		if ((err = connect(socket_,(struct sockaddr *) &serv_addr,sizeof(serv_addr))) < 0) 
		{
			state_ = eINVALID;
			APP_LOG(Applog::LOG_ERROR) <<  "connection error code" << err;
			return -1;
		}
#endif
		state_ = eCONNECT;
		return 0;

	}

	int QClientSocket::sendData(const string& data) {
		if(state_ != eCONNECT) {
			return -1;
		}       

		//add length at the head
		char LEN_HEAD[REC_LEN_MAX+1] = {0};
		sprintf(LEN_HEAD, "%16d", data.length());
		string datawithlen = LEN_HEAD;
		datawithlen += data;

#ifdef _MSC_VER
		int iResult = send( socket_, datawithlen.c_str(), datawithlen.size(), 0 );
#elif defined __GNUG__
		int iResult = write(socket_, datawithlen.c_str(), datawithlen.size()); 
#endif

#ifdef _MSC_VER
		if (iResult == SOCKET_ERROR) {
			int err_code = WSAGetLastError();
			closesocket(socket_);
#elif defined __GNUG__
		if (iResult < 0) {
			int err_code = h_errno;      
			close(socket_);
#endif
			state_ = eINVALID;
			DEBUG_PRINT1("send failed with error: %d\n", err_code);
			commonErrCodeHandle(err_code, "send", "QClientSocket::sendData");
			return -1;
		}           

		return 0;
	}

	int QClientSocket::sendrawdata( const char* ptrData, int nDataBytes )
	{
		if(state_ != eCONNECT) {
			return -1;
		}       

#ifdef _MSC_VER
		int iResult = send( socket_, ptrData, nDataBytes, 0 );
#elif defined __GNUG__
		int iResult = write(socket_, ptrData, nDataBytes); 
#endif

#ifdef _MSC_VER
		if (iResult == SOCKET_ERROR) {
			int err_code = WSAGetLastError();
			closesocket(socket_);
#elif defined __GNUG__
		if (iResult < 0) {
			int err_code = h_errno;      
			close(socket_);
#endif
			state_ = eINVALID;
			DEBUG_PRINT1("send failed with error: %d\n", err_code);
			commonErrCodeHandle(err_code, "send", "QClientSocket::sendData");
			return -1;
		}           

		return iResult;
	}

	int QClientSocket::receiveData(string& output_str) {    
		if(state_ != eCONNECT) {
			return -1;
		}
		int err_code = 0;
		output_str.clear();
		bool firstRecv = true;
		int nextRecvLen = REC_LEN_MAX;
		char rev_buf[MAX_REV_BUF_SIZE];
		int n = MAX_REV_BUF_SIZE -1;
		int targetLen = 0;
		int rev_count = 0;
			
		do {
			memset(rev_buf, 0,sizeof(char)*MAX_REV_BUF_SIZE);
			if (!firstRecv) {
				nextRecvLen = min(targetLen + REC_LEN_MAX - rev_count, MAX_REV_BUF_SIZE-1);
			}
#ifdef _MSC_VER
			n = recv(socket_, rev_buf, nextRecvLen, 0);
#elif defined __GNUG__
			n = read(socket_, rev_buf, nextRecvLen);
#endif
			if ( n > 0 ) {
				//DEBUG_PRINT1("Bytes received: %d\n", n);
			} else if ( n == 0 ) {
				DEBUG_PRINT0("Connection closed\n");
				return -1;
			} else {
#ifdef _MSC_VER
				err_code = WSAGetLastError();
#elif defined __GNUG__
				err_code = h_errno;
#endif
				state_ = eINVALID;
				DEBUG_PRINT1("recv failed with error: %d\n", err_code);
				commonErrCodeHandle(err_code, "recv", "QClientSocket::receiveData");
				return -1;

			}
			if (firstRecv) {
				int ret = n;
				while(n < REC_LEN_MAX) {
#ifdef _MSC_VER
					ret = recv(socket_, rev_buf+n, nextRecvLen-n, 0);
#elif defined __GNUG__
					ret = read(socket_, rev_buf+n, nextRecvLen-n);
#endif
					if ( ret > 0 ) {
						//DEBUG_PRINT1("Bytes received: %d\n", ret);
					} else if ( ret == 0 ) {
						DEBUG_PRINT0("Connection closed\n");
						return -1;
					}
					n += ret;
					APP_LOG(Applog::LOG_ERROR)<<"first Recv n < REC_LEN_MAX !!!!!!!!!!!!!!!!!!!![%d][%d]"<<n<<ret;
				}
				targetLen = atoi(rev_buf);
				firstRecv = false;
			} else {
				output_str.append(rev_buf);
			}

			rev_count = rev_count + n;
		} while(rev_count < targetLen + REC_LEN_MAX);

		return rev_count-REC_LEN_MAX;
	}

	QClientSocket::CLIENT_SOCK_STATE QClientSocket::State() const { return state_;}

	QClientSocket::QClientSocket() 
	{
		state_ = eINITIAL;
#ifdef _MSC_VER
		socket_ = INVALID_SOCKET;
#elif defined __GNUG__
		socket_ = -1; 
#endif
	}

	QClientSocket::~QClientSocket() 
	{
		if(state_ == eCONNECT) {
#ifdef _MSC_VER
			int iResult = shutdown(socket_, SD_SEND);

			if (iResult == SOCKET_ERROR) {
				DEBUG_PRINT1("shutdown failed: %d\n", WSAGetLastError());
				closesocket(socket_);           
				return;
			}
			closesocket(socket_);
#elif defined __GNUG__
			close(socket_);
#endif
		}
	}
}
