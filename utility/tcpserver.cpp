
#include "tcpserver.h"

namespace utility 
{

// Default server max connection client count.[1024]
unsigned int TCPServer::DefaultMaxClient = 1024;
	
// Default buffer size [64*1024]
unsigned int TCPServer::DefaultBufferSize = 64 * 1024;
	
// Max datagrame size [64*1024]
unsigned int TCPServer::MaxDatagramSize = 64 * 1024;

TCPServer::TCPServer(const string& hostname, 
					 const string& port) :
	hostname_(hostname),
	port_(port),
	max_client_(DefaultMaxClient),
	is_run_(false) 
{
	//windows used
	QSocket::startUp();
}

TCPServer::TCPServer(const string& hostname, 
					 const string& port,
					 unsigned int max_client) :
	hostname_(hostname),
	port_(port),
	max_client_(max_client),
	is_run_(false) 
{
	//windows used
	QSocket::startUp();
}

void TCPServer::Start() {
	if (is_run_) {
		return;
	}
	is_run_ = true;

	// recv_data_buffer_ = new char[DefaultBufferSize]; 
	server_socket_ = new QServerSocket();
	QAddress addr;
	addr.host_name = hostname_; 
	addr.port = port_;
	server_socket_->bindingAddr(addr);
	server_socket_->listenConnection(5);
	//server_socket_->setNoBlock();
	//server_socket_->select(100);

	AcceptConn();
}

void TCPServer::Stop() {
	if (!is_run_) {
		return;
	}

	is_run_ = false;
	// Close all client session.
	CloseAllClient();

	delete server_socket_;
}

void TCPServer::CloseAllClient() {
	map<SessionId, Session*>::iterator iter = session_map_.begin();

	for (; iter != session_map_.end(); ++iter) {
		delete iter->second;
	}

	client_count_ = 0;
	session_map_.clear();
}

void TCPServer::CloseClient(Session* session) {
	if (session == NULL) {
		return;
	}
	MutexLocker locker(&mutex_);
	// Remove session.
	session_map_.erase(session->GetSessionID());
	--client_count_;
	
	//delete session;

}

void TCPServer::AcceptConn() {
	// If server stop server, stop receive new client session
 	if (!is_run_) return;

	// Loop accept connection
	QClientSocket* client = server_socket_->acceptClient();
	if (client == NULL) { 
		return;
	}

	// Check connection has been max count.
	if (client_count_ == max_client_) {
		client->sendData(string("Server Full"));
		delete client;
		return;
	}

	client->SetBuffer(false, 10240000);
	client->SetBuffer(true, 102400);
	Session* session = new Session(client, this);
	session_map_[session->GetSessionID()] = session;
	++client_count_;

	QCThreadManager::createThread(*session);
	//client->sendData(string("Connection Successful|"));
	
	q_sleep(1000);
	AcceptConn();
}

void TCPServer::SendAllClient(const string& datagrame) {
	MutexLocker locker(&mutex_);
	map<SessionId, Session*>::iterator iter = session_map_.begin();
	for (; iter != session_map_.end(); ++iter) {
		iter->second->Send(datagrame);
	}
}

void TCPServer::Send(Session* session, const string& datagrame) {
	if (session == NULL) {
	}

	session->Send(datagrame);
}

} 
