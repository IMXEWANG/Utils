#ifndef _QCLIENTSOCKET_H
#define _QCLIENTSOCKET_H

#ifdef _MSC_VER
#ifndef WIN_SOCKET_INCLUDE
#define WIN_SOCKET_INCLUDE
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma  comment(lib, "Ws2_32.lib")
#endif
#elif defined __GNUG__

#endif

#include "QSocket.h"
#define MAX_REV_BUF_SIZE 1024

namespace utility {

	struct COMMON_API QAddress;

	class COMMON_API QClientSocket 
		: public QSocket {
	public:
		enum CLIENT_SOCK_STATE{
			eINITIAL,
			eCONNECT,
			eCLOSE,
			eINVALID,
		};
		friend class QServerSocket;
		QClientSocket(); //pass
		~QClientSocket();

		CLIENT_SOCK_STATE State() const;
		int connectServer(const QAddress & addr);
		int sendData(const string& data); //pass
		int sendrawdata( const char* ptrData, int nDataBytes ); //pass unused

		int receiveData(string& output_str);   
	private:    
		CLIENT_SOCK_STATE state_;

	};
}
#endif
