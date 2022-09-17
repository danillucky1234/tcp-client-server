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

class Tcp_Client {
private:
	FileDescriptor _socket_fd;
	size_t _port;
	int _ip;
	int _client_ip;
	
public:
	Tcp_Client(const std::string &ip, size_t port);
	~Tcp_Client();

	void sendMessage(const std::string &msg) const;
	void readMessage(char *buffer) const;

private:
	Tcp_Client() {}
	void createSocket(int domain, int type, int protocol);
	void connectSocket(const std::string &ip, size_t port);
	void closeSocket();
};

