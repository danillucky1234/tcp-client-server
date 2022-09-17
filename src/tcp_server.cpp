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
		this->acceptClient();
	} catch(const std::runtime_error &ex) {
		std::cout << "Error code: " << errno << std::endl;
		perror(ex.what());
	}
}

Tcp_Server::~Tcp_Server() {
	this->closeSocket();
}

int Tcp_Server::send(const std::string &msg) const {
	// if client exists
	// send message
	// else return -1
	return 0;
}

int Tcp_Server::read(std::string *msg) const {
	// if client exists
	// read message
	// else return -1
	return 0;
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

	log("Created pointer to sockaddr_in struct ");

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
	addr.sin_port = ntohl(port); // Convert from network byte order to host byte order
	addr.sin_addr.s_addr = INADDR_ANY;

	log("addr struct fully initialized");

	int bindResult = bind(this->_socket_fd.get(), (struct sockaddr*) &addr, sizeof(addr));
	log("bindingResult: ", bindResult);
	if (bindResult == -1) {
		throw std::runtime_error("Binding socket error");
	}
	if (bindResult == 0) {
		log("Binding socket to port - OK");
	}
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
		throw std::runtime_error("Acceping socket error");
	}

	_client_fd.set(new_socket_fd);
	log("Accepting client - OK");
	log("Client file descriptor: ", _client_fd.get());
}

void Tcp_Server::closeSocket() {
	// close the socket
	if(close(this->_socket_fd.get()) == -1) {
		throw std::runtime_error("Closing the socket error");
	}
	this->_is_running = false;
}
