#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"
#include "client.h"
#include "server_observer.h"

class Tcp_Server {
private:
	FileDescriptor _socket_fd;
	size_t _port;

	std::vector<std::unique_ptr<Client>> _clients;
	std::vector<Server_Observer*> _observers;
	std::mutex _mutex;
	
public:
	Tcp_Server();
	~Tcp_Server();

	std::vector<std::string> getClientsIp();
	void start(const std::string &ip, size_t port);
	void sendMessage(const std::string &msg, int client_id = -1) const;
	void registerObserver(Server_Observer *observer);
	std::vector<std::pair<std::string, std::string>> readMessages(int client_id = -1) const;

private:
	void createSocket(int domain, int type, int protocol);
	void bindSocket(const std::string &ip, size_t port);
	void listenSocket(int max_queue = 5);
	void readMessage(char *buffer, int client_id = -1) const;
	void removeDeadClients();
	void acceptClient();
	void closeSocket();

	void clientEventsHandler(const Client &client, ClientEvent type, const std::string &msg);
	void notifyClientConnection(const std::string &clientName, const std::string &clientIp) const;
	void notifyClientDisconnection(const std::string &clientName, const std::string &clientIp) const;
	void notifyIncomingMessage(const std::string &clientName, const std::string &clientIp, const std::string &msg) const;
};
