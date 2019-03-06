#ifndef _QSERVERSOCKET_H
#define _QSERVERSOCKET_H

#ifdef _MSC_VER
#ifndef WIN_SOCKET_INCLUDE
#define WIN_SOCKET_INCLUDE
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma  comment(lib, "Ws2_32.lib")
#endif
#elif defined __GNUG__
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "QSocket.h"

namespace utility {
	struct COMMON_API QAddress;
	class COMMON_API QClientSocket;

	class COMMON_API QServerSocket : public QSocket {
	public:
		enum SERVER_SOCK_STATE{
			eINITIAL,
			eBIND,
			eLISTEN,
			eINVALID,
		};
		int bindingAddr(const QAddress & addr, bool isUDP = false); //pass

		int listenConnection(int conn_num); //pass

		QClientSocket* acceptClient(); //pass

		QServerSocket(); //pass

		~QServerSocket();

	private:    
		SERVER_SOCK_STATE state_;
	};
}
#endif
