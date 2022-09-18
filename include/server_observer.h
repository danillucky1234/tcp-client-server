#pragma once

#include "pch.h"

class Server_Observer {
public:
	std::function<void (const std::string &clientName, const std::string &clientIp)> connectionHandler;
	std::function<void (const std::string &clientName, const std::string &clientIp)> disconnectionHandler;
	std::function<void (const std::string &clientName, const std::string &clientIp, const std::string &msg)> incomingMessageHandler;
};
