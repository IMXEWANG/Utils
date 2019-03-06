//////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// tcpserver.h
/// Object of TCPServer
///
/// @version: 1.0.0
/// @author: zhengzengyuan
/// @email: zheng.zengyuan@fuanda.co
/// @date: 2012/09/18
/// @version $Id: tcpserver.h 1076 2012-09-19 05:55:17Z zhengzengyuan $ 
//////////////////////////////////////////////////////////////////////////////////

#ifndef EYEGLE_COMMON_TCPSERVER_H_
#define EYEGLE_COMMON_TCPSERVER_H_

#include <map>
#include "QAddress.h"
#include "QServerSocket.h"
#include "QThread.h"
#include "QSharePtr.h"
#include "session.h"
#include "Mutex.h"
namespace utility 
{

// TCP server control entity
class COMMON_API TCPServer 
	: public IThreadRunner 
{
public:
	// Default server max connection client count.[1024]
	static unsigned int DefaultMaxClient;
	
	// Default buffer size [64*1024]
	static unsigned int DefaultBufferSize;
	
	// Max datagrame size [64*1024]
	static unsigned int MaxDatagramSize;

	// Constructor function(Default max client connection count)
	// @param port: Server listener port.
	TCPServer(const string& hostname, 
			  const string& port);

	// Constructor function.
	// @param port: Server listener port
	// @param max_client: Max capacity client count.
	TCPServer(const string& hostname, 
			  const string& port,
			  unsigned int max_client);

	QServerSocket* GetServerSocket() const { return server_socket_; }

	// Max allow client connection count.
	unsigned int Capacity() const { return max_client_; }

	// Current client session count.
	unsigned int SessionCount() const { return client_count_; }

	// Server run state
	bool IsRun() const { return is_run_; }

	// Start server porgrame, start listener client request.
	virtual void Start(); //pass
	// Stop server programe,
	virtual void Stop(); //pass

	// Close all client session.
	virtual void CloseAllClient(); //pass
	
	// Close specific client session.
	// @param session: client session.
	virtual void CloseClient(Session* session); 

	// Send data to all client session
	// @param datagrame: send datagrame 
	virtual void SendAllClient(const string& datagrame);

	// Send data to specific client session.
	// @param session: client session
	// @param datagrame: data datagrame
	virtual void Send(Session* session, const string& datagrame);

	virtual void run() {
		Start();	
	}

private:
	// Accept connection.
	void AcceptConn();

private:
	// Hostname and port
	string hostname_;
	string port_;
	// Server allow max client connection count.
	unsigned int max_client_;
	// Current connection client count
	unsigned int client_count_;
	// Server run state
	bool is_run_;
	// Server use socket
	QServerSocket* server_socket_;
	// All client session map.
	map<SessionId, Session*> session_map_;
	Mutex mutex_;
};

typedef QSharePtr<TCPServer> TCPServerPtr;

} 

#endif
