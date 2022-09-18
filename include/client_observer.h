#pragma once

#include "pch.h"

class Client_Observer {
public:
	std::function<void ()> connectionHandler;
	std::function<void ()> disconnectionHandler;
	std::function<void (const std::string &msg)> incomingMessageHandler;
};
