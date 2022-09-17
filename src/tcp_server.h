#pragma once

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "fileDescriptor.hpp"

class Tcp_Server {
private:
	FileDescriptor _socket_fd;
	size_t _port;
	int _client_ip;
	bool _is_running;
	FileDescriptor _client_fd;
	
public:
	Tcp_Server(const std::string &ip, size_t port);
	~Tcp_Server();

	int send(const std::string &msg) const;
	int read(std::string *msg) const;

private:
	Tcp_Server() {}
	void createSocket(int domain, int type, int protocol);
	void bindSocket(const std::string &ip, size_t port);
	void listenSocket(int max_queue = 5);
	void acceptClient();
	void closeSocket();
};
