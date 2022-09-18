#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"

enum ClientEvent {
	INCOMING_MSG = 0,
	CONNECTED,
	DISCONNECTED
};

class Client {
private:
    using client_event_handler = std::function<void(const Client&, ClientEvent, const std::string&)>;

	FileDescriptor _fd;
	struct sockaddr_in _addr;
	bool _isConnected;
	std::string _name;
	client_event_handler _eventHandlerCallback;

public:
	Client();
	Client(int fd, const struct sockaddr_in &addr);

	void setFileDescriptor(int fd);
	void setFileDescriptor(const FileDescriptor &fd);
	void setAddressInfo(const struct sockaddr_in &addr);
	int getFd() const;
	/* struct sockaddr_in getAddr() const; */
	std::string getIp() const;
	std::string getName() const;
	void setName(const std::string &fd);
	bool isConnected() const;
	void setEventHandler(const client_event_handler &handler);
	void startListen();

private:
	void setConnected(bool flag);
	void receiveTask();
};
