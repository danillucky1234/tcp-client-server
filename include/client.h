#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"

enum ClientEvent {
	INCOMING_MSG = 0,
	CONNECTED,
	DISCONNECTED,
	NICK_CHANGED
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

	int getFd() const;
	std::string getIp() const;
	std::string getName() const;
	bool isConnected() const;
	void setEventHandler(const client_event_handler &handler);
	void startListen();

private:
	void setName(const std::string &fd);
	void setConnected(bool flag);
	void receiveTask();
	void handleReadMessage(const char *command_prefix, const std::string &messageFromTheClient);
};
