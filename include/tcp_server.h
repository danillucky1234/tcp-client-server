#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"
#include "client.h"

class Tcp_Server {
private:
	FileDescriptor _socket_fd;
	size_t _port;

	std::vector<std::unique_ptr<Client>> _clients;
	std::mutex _mutex;
	
public:
	Tcp_Server(const std::string &ip, size_t port);
	~Tcp_Server();

	std::vector<std::string> getClientsIp();
	void sendMessage(const std::string &msg, int client_id = -1) const;
	std::vector<std::pair<std::string, std::string>> readMessages(int client_id = -1) const;

private:
	Tcp_Server() {}
	void createSocket(int domain, int type, int protocol);
	void bindSocket(const std::string &ip, size_t port);
	void listenSocket(int max_queue = 5);
	void readMessage(char *buffer, int client_id = -1) const;
	void removeDeadClients();
	void acceptClient();
	void closeSocket();

	void clientEventsHandler(const Client &client, ClientEvent type, const std::string &msg);
};
