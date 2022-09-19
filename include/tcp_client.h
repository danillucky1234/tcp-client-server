#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"
#include "client_observer.h"

class Tcp_Client {
private:
	FileDescriptor _socket_fd;
	size_t _port;
	bool _isConnected;
	std::vector<Client_Observer*> _observers;
	
public:
	Tcp_Client();
	~Tcp_Client();

	void registerObserver(Client_Observer *observer);
	bool connectTo(const std::string &ip, size_t port);
	bool sendMessage(const std::string &msg) const;
	bool changeNickname(const std::string &msg) const;

private:
	void createSocket(int domain, int type, int protocol);
	void connectSocket(const std::string &ip, size_t port);
	void closeSocket();
	void receiveTask();
	bool writeMessage(const std::string &msg) const;

	void notifyConnection() const;
	void notifyServerDisconnection() const;
	void notifyIncomingMessage(const std::string &msg) const;
};

