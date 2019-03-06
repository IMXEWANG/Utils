//////////////////////////////////////////////////////////////////////////////////
/// session.h
/// @version: 1.0.0
/// @author: Eric
/// @date: 2016/10/21
//////////////////////////////////////////////////////////////////////////////////
#ifndef FUANDA_DSCORE_SESSION_H_
#define FUANDA_DSCORE_SESSION_H_

#include "session_id.h"
#include "QClientSocket.h"
#include "QThread.h"

#include <iostream>

namespace utility 
{

class COMMON_API TCPServer;

// Client and Server session description.
class COMMON_API Session 
	: public IThreadRunner 
{
public:
	// Exit type enumerator defined
	enum ExitType {
		NormalExit,
		ExceptionExit
	};

	Session();

	// Constructor client and server session.
	// @param cliSocket: client socket
	Session(QClientSocket* cli_socket, TCPServer* tcpserver);
	~Session();

	// Assginment operator
	Session& operator=(const Session& other);

	// Get current session's session id.
	SessionId GetSessionID() const { return id_; }

	// Client datagram
	string Datagram() const { return datagram_; }
	void Datagram(const string& datagram) { datagram_ = datagram; }

	// Client exit type
	ExitType TypeOfExit() { return exit_type_; }
	void TypeOfExit(ExitType exit_type) { exit_type_ = exit_type; }

	QClientSocket* GetClientSocket() const { return client_socket_; }

	// Exceute session and receive client data.
	virtual void run();

	// Send datagrame to client.
	// @param datagrame: Send message datagrame.
	virtual void Send(const string& datagrame);

	// Send datagrame to all client session
	// @param datagrame: Send message datagrame
	virtual void SendAllSession(const string& datagrame);

private:
	// TCP server
	TCPServer* tcpserver_;

	// Session identify
	SessionId id_;
	// Client send to server datagrame
	// Note: sometimes datagrame maybe no complete
	string datagram_;
	// Client's socket
	QClientSocket* client_socket_;
	// Client exit type
	ExitType exit_type_;
};

// Overload output stream
COMMON_API ostream& operator<<(ostream& output, const Session& session);

// Overload operator==
COMMON_API bool operator==(const Session& rhs, const Session& lhs);

}

#endif
