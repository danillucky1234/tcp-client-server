#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"

class Tcp_Client {
private:
	FileDescriptor _socket_fd;
	size_t _port;
	bool _isConnected;
	
public:
	Tcp_Client(const std::string &ip, size_t port);
	~Tcp_Client();

	void sendMessage(const std::string &msg) const;

private:
	Tcp_Client() {}
	void createSocket(int domain, int type, int protocol);
	void connectSocket(const std::string &ip, size_t port);
	void closeSocket();
	void receiveTask();
};

