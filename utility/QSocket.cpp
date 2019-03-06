#ifdef _MSC_VER
#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma  comment(lib, "Ws2_32.lib")
#endif

#ifdef __GNUG__
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#include "Applog.h"
#include "QSocket.h"

namespace utility {

	int QSocket::startUp()
	{
#ifdef _MSC_VER
		int err;
		WORD wVer;
		WSADATA wsaData;
		wVer = MAKEWORD(1, 1);

		err = WSAStartup(wVer, &wsaData);

		if(err != 0)
		{
			return 0;
		}
#else    
#endif
		return 0;
	}

	int QSocket::cleanUp()
	{

#ifdef _MSC_VER
		WSACleanup();
#endif
		return 0;
	}

	QSocket::QSocket():isNoBlock(false)
	{
	}

	int QSocket::select(int timeout)
	{
		fd_set read_set;
		fd_set write_set;
		fd_set except_set;

		FD_ZERO(&read_set);
		FD_ZERO(&write_set);
		FD_ZERO(&except_set);

		FD_SET(socket_, &read_set);
		FD_SET(socket_, &write_set);
		FD_SET(socket_, &except_set);

		int state = 0;

		timeval  time_v;
		time_v.tv_sec = timeout/1000;
		time_v.tv_usec = (timeout%1000)*1000;
#ifdef __GNUG__
		if(::select(socket_+1, &read_set, &write_set, &except_set, &time_v) >0)
#else
		if(::select(0, &read_set, &write_set, &except_set, &time_v) >0)
#endif
		{
			if(FD_ISSET(socket_, &read_set))
			{
				state = state | READABLE;
			}

			if(FD_ISSET(socket_,&write_set))
			{
				state = state | WRITABLE;
			}

			if(FD_ISSET(socket_,&except_set))
			{
				state = state | EXCEPT;
			}
		}   

		return state;
	}

	void QSocket::SetBuffer(bool recvOpt, int size)
	{
		//get default size
		int udpbuf = 0;
		int len = sizeof(udpbuf);
		int ret1 = 0;
#ifdef _MSC_VER
		if (recvOpt) {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*) &udpbuf, &len);
		}
		else {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*) &udpbuf, &len);
		}
		if (ret1 == SOCKET_ERROR)
		{      
			int err_code = WSAGetLastError();
			DEBUG_PRINT1("recvfrom failed with error: %d\n", err_code);
			commonErrCodeHandle(err_code, "getsockopt", __FUNCTION__);
		}
		APP_LOG(Applog::LOG_DEBUG_LIB) << "Current UDP rev buffer size: " << udpbuf;

		//set new size
		APP_LOG(Applog::LOG_DEBUG_LIB) << "Setting UDP rev buffer size: " << size;
		if (recvOpt) {
			ret1 = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*) &size, sizeof(size));
		}
		else {
			ret1 = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*) &size, sizeof(size));
		}
		if (ret1 == SOCKET_ERROR)
		{      
			int err_code = WSAGetLastError();
			DEBUG_PRINT1("recvfrom failed with error: %d\n", err_code);
			commonErrCodeHandle(err_code, "recvfrom", __FUNCTION__);
		}

		if (recvOpt) { 
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*) &udpbuf, &len);
		}
		else {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*) &udpbuf, &len);
		}

		if (ret1 == SOCKET_ERROR)
		{      
			int err_code = WSAGetLastError();
			DEBUG_PRINT1("recvfrom failed with error: %d\n", err_code);
			commonErrCodeHandle(err_code, "recvfrom", __FUNCTION__);
		}
#elif defined __GNUG__
		if (recvOpt) {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (void*) &udpbuf, (socklen_t*)&len);
		}
		else {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (void*) &udpbuf, (socklen_t*)&len);
		}

		if (ret1 == -1) {      
			commonErrCodeHandle(errno, "getsockopt", __FUNCTION__);
		}
		APP_LOG(Applog::LOG_DEBUG_LIB) << "Current UDP rev buffer size: " << udpbuf;

		//set new size
		APP_LOG(Applog::LOG_DEBUG_LIB) << "Setting UDP rev buffer size: " << size;
		if (recvOpt) {
			ret1 = setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (const void*) &size, (socklen_t)sizeof(size));
		} else {
			ret1 = setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (const void*) &size, (socklen_t)sizeof(size));
		}
		if (ret1 == -1) {      
			commonErrCodeHandle(errno, "setsockopt", __FUNCTION__);
		}
		if (recvOpt) {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (void*) &udpbuf, (socklen_t*)&len);
		} else {
			ret1 = getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (void*) &udpbuf, (socklen_t*)&len);
		}
		if (ret1 == -1)
		{      
			commonErrCodeHandle(errno, "getsockopt", __FUNCTION__);
		}
#endif

		APP_LOG(Applog::LOG_DEBUG_LIB) <<  "Now new UDP rev buffer size: " << udpbuf;
	}

	void QSocket::setNoBlock()
	{
#ifdef _MSC_VER
		u_long iMode=1;
		ioctlsocket(socket_,FIONBIO,&iMode);
#elif defined __GNUG__
		int flags; 
		flags = fcntl(socket_, F_GETFL, 0); 
		if (flags < 0)  { 
			return;
		} 
		if (fcntl(socket_, F_SETFL, flags | O_NONBLOCK) < 0) {} 
#endif
		isNoBlock = true;
	}

	void QSocket::commonErrCodeHandle(int err_code, string sock_api_name, string code_contex)
	{
		string info;
#ifdef _MSC_VER
		switch(err_code)
		{
		case WSANOTINITIALISED:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"A successful WSAStartup call must occur before using this function.";
				break;
			}
		case WSANO_RECOVERY:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"a nonrecoverable failure";            
				break;
			}
		case WSAEAFNOSUPPORT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"ai family parameter is not support";            
				break;
			}
		case WSA_NOT_ENOUGH_MEMORY:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"not enough memory";            
				break;
			}
		case WSATYPE_NOT_FOUND:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The host not found";
				break;
			}
		case WSAESOCKTNOSUPPORT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket type is not supported";            
				break;
			}
		case WSAENETDOWN:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The network subsystem has failed!";            
				break;
			}
		case WSAEACCES:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The requested address is a broadcast address, but the appropriate flag was not set!";            
				break;
			}
		case WSAEINTR:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The blocking call is canceled!";
				break;
			}
		case WSAEINPROGRESS:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket is busy in other progress/thread!";
				break;
			}
		case WSAEFAULT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The buf parameter is not completely contained in a valid part of the user address space!";
				break;
			}
		case WSAENETRESET:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress";
				break;
			}
		case WSAENOBUFS:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"No buffer space is available";
				break;
			}
		case WSAENOTCONN:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket is not connected!";
				break;
			}
		case WSAENOTSOCK:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The descriptor is not a socket!";
				break;
			}
		case WSAEOPNOTSUPP:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM";
				break;
			}
		case WSAESHUTDOWN:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket has been shutdown!";
				break;
			}
		case WSAEWOULDBLOCK:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket is marked as nonblocking and the requested operation would block!";
				break;
			}
		case WSAEMSGSIZE:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket is message oriented, and the message is larger than the maximum supported by the underlying transport!";
				break;
			}
		case WSAEHOSTUNREACH:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The remote host cannot be reached from this host at this time.";
				break;
			}
		case WSAEINVAL:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket has not been bound with bind, or an unknown flag was specified";
				break;
			}
		case WSAECONNABORTED:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The virtual circuit was terminated due to a time-out or other failure.";
				break;
			}
		case WSAECONNRESET:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The virtual circuit was reset by the remote side executing a hard or abortive close.";
				break;
			}
		case WSAETIMEDOUT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The connection has been dropped, because of a network failure or because the system on the other end went down without notice";
				break;
			}
		case WSAEPROTONOSUPPORT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The specified protocol is not supported";
				break;
			}
		case WSAEPROTOTYPE:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The specified protocol is the wrong type for this socket.";
				break;
			}
		case WSAEPROVIDERFAILEDINIT:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The service provider failed to initialize.";
				break;
			}

		case WSAEMFILE:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"No more socket descriptors are available.";
				break;
			}
		case WSAEADDRINUSE:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The socket's local address is already in use and socket was not market to allow address reuse.";
				break;
			}
		case WSAEADDRNOTAVAIL:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The remote address is not valid address.";
				break;
			}
		case WSAECONNREFUSED:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The attempt to connect was forcefully rejected.";
				break;
			}
		case WSAEISCONN:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"The connection is already connected!";
				break;
			}   


		default:
			{
				info = code_contex + " : "+ sock_api_name+ " : " +"";
				break;
			}
		}
#endif
		APP_LOG(Applog::LOG_ERROR) << info << err_code;
	}
}
