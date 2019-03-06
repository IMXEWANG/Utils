#ifndef _QSOCKET_H
#define _QSOCKET_H
#include "Global.h"
#include <string>
using namespace std;

#define SOCKET_DEBUG 1

#if SOCKET_DEBUG
#define DEBUG_PRINT0(value) printf(value)
#define DEBUG_PRINT1(format, value) printf(format, value)
#else
#define DEBUG_PRINT0(value) 
#define DEBUG_PRINT1(format, value)
#endif

namespace utility {

	enum SELECT_STATE{
		TIMEOUT = 0,
		WRITABLE = 1,
		READABLE = 2,
		WRABLE = 3,
		EXCEPT = 4,        
	};

	class COMMON_API QSocket {
	public:
		static int startUp(); //pass

		static int cleanUp(); //pass

		QSocket(); //pass

		int select(int timeout);

		//recvOpt: false:send true:receiver
		void SetBuffer(bool recvOpt, int size);//pass

		void setNoBlock(); //pass

#ifdef _MSC_VER
		SOCKET GetSocketHandle() { return socket_; }
#elif defined __GNUG__
		int GetSocketHandle() { return socket_; }
#endif

	protected:
		static void commonErrCodeHandle(int err_code, string sock_api_name, string code_contex); //pass  

		bool isNoBlock;
#ifdef _MSC_VER
		SOCKET socket_;
#elif defined __GNUG__
		int socket_; 
#endif  
	};
}
#endif
