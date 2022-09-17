#include "tcp_server.h"

template<typename... Args>
void log(Args... args)
{
#ifdef DEBUG
    ([&]{
        std::cout << args << " ";
        }(), ...);
    std::cout << std::endl;
#endif
}

Tcp_Server::Tcp_Server(const std::string &ip, size_t port) {
	this->_is_running = false;
	log("Ip: " + ip);
	log("Port: ", port);
	try {
		this->createSocket(
				AF_INET,     // IPv4
				SOCK_STREAM, // Full-duplex connection
				IPPROTO_TCP  // TCP
				);
		this->bindSocket(ip, port);
		this->listenSocket();
		/* this->acceptClient(); */
	} catch(const std::runtime_error &ex) {
		std::cout << "Error code: " << errno << std::endl;
		perror(ex.what());
	}
}

Tcp_Server::~Tcp_Server() {
	this->closeSocket();
}

void Tcp_Server::sendMessage(const std::string &msg) const {
	int send_result = write(this->_client_fd.get(), msg.c_str(), msg.size());
	if (send_result == -1) {
		throw std::runtime_error("Sending the message to client error");
	}
	if (send_result > 0) {
		log("Sending message to the client - OK");
	}
	log("Send to the client: ", msg);
}

void Tcp_Server::readMessage(char *buffer) const {
	log("Before reading the message");
	int read_result = read(this->_client_fd.get(), buffer, 1024);
	if (read_result == -1) {
		throw std::runtime_error("Reading from socket error");
	}
	log("Read from client: ", *buffer);
}

void Tcp_Server::createSocket(int domain, int type, int protocol) {
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd == -1) {
		throw std::runtime_error("Socket creation failed!");
	}
	this->_socket_fd.set(sock_fd);
	log("Socket creation - OK");
	log("Socket file descriptor - ", sock_fd);
}

void Tcp_Server::bindSocket(const std::string &ip, size_t port) {
	struct sockaddr_in addr;
	
	// Set options for socket
	int optional_value = 1;
	if (setsockopt(this->_socket_fd.get(), // Socket fd
				   SOL_SOCKET /*maybe use IPPROTO_TCP*/, // Manipulate at the sockets API level
                   SO_REUSEADDR | SO_REUSEPORT, // 
				   &optional_value,
                   sizeof(optional_value))
			== -1) {
		throw std::runtime_error("setsockopt error");
    }

	addr.sin_family = AF_INET; // IPv4
	addr.sin_port = htons(port); // Convert from host byte order to network byte order
	int ip_convertation_result = inet_aton(
			ip.c_str(),
			&addr.sin_addr);
	if (ip_convertation_result == 0) {
		throw std::runtime_error("Convertation ip to network byte order failed!");
	}
	log("NBO Port:", addr.sin_port);


	int bindResult = bind(this->_socket_fd.get(), (struct sockaddr*) &addr, sizeof(addr));
	if (bindResult == -1) {
		throw std::runtime_error("Binding socket error");
	}
	log("Binding socket to port - OK");
}

void Tcp_Server::listenSocket(int max_queue) {
	int listen_result = listen(this->_socket_fd.get(), max_queue);
	if (listen_result == -1) {
		throw std::runtime_error("Listening socket error");
	}
	this->_is_running = true;
	log("Listening socket - OK");
}

void Tcp_Server::acceptClient() {
	// If no pending connections and the socket is not marked as nonblocking,
	// accept() blocks the caller until a connection is present
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	int new_socket_fd = accept(this->_socket_fd.get(),
			                   (struct sockaddr*) &client_addr,
							   &client_addr_size);
	if (new_socket_fd == -1) {
		throw std::runtime_error("Accepting socket error");
	}

	_client_fd.set(new_socket_fd);
	log("Accepting client - OK");
	log("Client IP: ", inet_ntoa(client_addr.sin_addr));
	log("Client PORT: ", ntohs(client_addr.sin_port));
	log("Client file descriptor: ", _client_fd.get());
}

void Tcp_Server::closeSocket() {
	// close the socket
	if(close(this->_socket_fd.get()) == -1) {
		throw std::runtime_error("Closing the socket error");
	}
	log("Close the socket - OK");
	this->_is_running = false;
}
