
#include "session.h"
#include "tcpserver.h"

#include <sstream>

namespace utility 
{

Session::Session() {
	client_socket_ = NULL;
	tcpserver_ = NULL;
}

Session::Session(QClientSocket* cli_socket, TCPServer* tcpserver){
	// Argument check
	if (cli_socket == NULL) {
		return;
	}

	tcpserver_ = tcpserver;
	client_socket_ = cli_socket;
	id_.Id(cli_socket->GetSocketHandle());

	std::cout << "Session connection successful, SessionId: " 
			  << id_.Id() << std::endl;
}

Session::~Session() {
	delete client_socket_;

	IThreadRunner::afterTerminate();
}

Session& Session::operator=(const Session& other) {
	if (this == &other) {
		return *this;
	}

	id_ = other.id_;
	client_socket_ = other.client_socket_;
	datagram_ = other.datagram_;
	exit_type_ = other.exit_type_;

	return *this;
}

void Session::run() {
	while (true) {
		int res = client_socket_->receiveData(datagram_);
		cout << datagram_ << endl;
		if (client_socket_->State() == QClientSocket::eCLOSE ||
			client_socket_->State() == QClientSocket::eINVALID ||
			res == -1) {
			tcpserver_->CloseClient(this);

			delete client_socket_;
			IThreadRunner::afterTerminate();
			break;
		}

		Send(datagram_);
	}
}

void Session::Send(const string& datagrame) {
	client_socket_->sendData(datagrame);
}

void Session::SendAllSession(const string& datagrame) {
	tcpserver_->SendAllClient(datagrame);
}

ostream& operator<<(ostream& output, const Session& session) {
	output << "Session:" << session.GetSessionID().Id() << endl;

	return output;
}

bool operator==(const Session& rhs, const Session& lhs) {
	return (rhs.GetSessionID() == lhs.GetSessionID());
}



}
