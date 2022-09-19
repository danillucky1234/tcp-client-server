#pragma once

#include "pch.h"
#include "fileDescriptor.hpp"
#include "client.h"
#include "server_observer.h"

struct client_info {
public:
	std::string name;
	std::string ip;
};

class Tcp_Server {
private:
	FileDescriptor _socket_fd;
	size_t _port;

	std::vector<std::unique_ptr<Client>> _clients;
	std::vector<Server_Observer*> _observers;
	std::mutex _clients_mutex;
	
public:
	Tcp_Server();
	~Tcp_Server();

	std::vector<client_info*> getClientsInfo();
	bool start(const std::string &ip, size_t port);
	bool sendMessage(const std::string &msg, int client_id = -1) const;
	void registerObserver(Server_Observer *observer);

private:
	void createSocket(int domain, int type, int protocol);
	void bindSocket(const std::string &ip, size_t port);
	void listenSocket(int max_queue = 5);
	bool writeMessage(const std::string &msg, int client_id = -1) const;
	void removeDeadClients();
	void acceptClient();
	void closeSocket();

	void clientEventsHandler(const Client &client, ClientEvent type, const std::string &msg);
	void notifyClientConnection(const std::string &clientName, const std::string &clientIp) const;
	void notifyClientDisconnection(const std::string &clientName, const std::string &clientIp) const;
	void notifyIncomingMessage(const std::string &clientName, const std::string &clientIp, const std::string &msg) const;
	void notifyClientNickChanging(const std::string &oldName, const std::string &newName, const std::string &clientIp) const;
};
