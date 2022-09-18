#include "../include/tcp_server.h"

template<typename... Args>
void log(Args... args)
{
#ifdef DEBUG
    ([&]{
        std::cout << args << " ";
        }(), ...);
    std::cout << std::endl;
#endif
}

Tcp_Server::Tcp_Server() {
	this->_clients.reserve(5); // Reserve space for 5 clients
}

void Tcp_Server::start(const std::string &ip, size_t port) {

	log("Ip: " + ip);
	log("Port: ", port);
	try {
		this->createSocket(
				AF_INET,     // IPv4
				SOCK_STREAM, // Full-duplex connection
				IPPROTO_TCP  // TCP
				);
		this->bindSocket(ip, port);
		this->listenSocket();

		std::thread t1([&]{ this->acceptClient();});
		t1.detach();
	} catch(const std::runtime_error &ex) {
		std::cout << "Error code: " << errno << std::endl;
		perror(ex.what());
	}
}

Tcp_Server::~Tcp_Server() {
	this->closeSocket();
}

void Tcp_Server::sendMessage(const std::string &msg, int client_id) const {
	if (client_id == -1) {
		// send to all
		for (size_t i = 0; i < this->_clients.size(); ++i) {
			this->sendMessage(msg, i);
		}
	} else if (client_id >= 0 && (size_t)client_id < this->_clients.size()) {
		int send_result = write(this->_clients[client_id]->getFd(), msg.c_str(), msg.size());
		if (send_result == -1) {
			throw std::runtime_error("Sending the message to client error");
		}
		if (send_result > 0) {
			log("Sending message to the ", client_id, " client - OK");
		}
		log("Send to the ", client_id, " client: ", msg);
	}
}

void Tcp_Server::createSocket(int domain, int type, int protocol) {
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd == -1) {
		throw std::runtime_error("Socket creation failed!");
	}
	this->_socket_fd.set(sock_fd);
	log("Socket creation - OK");
	log("Socket file descriptor - ", sock_fd);
}

void Tcp_Server::bindSocket(const std::string &ip, size_t port) {
	struct sockaddr_in addr;
	
	// Set options for socket
	int optional_value = 1;
	if (setsockopt(this->_socket_fd.get(), // Socket fd
				   SOL_SOCKET /*maybe use IPPROTO_TCP*/, // Manipulate at the sockets API level
                   SO_REUSEADDR | SO_REUSEPORT, // 
				   &optional_value,
                   sizeof(optional_value))
			== -1) {
		throw std::runtime_error("setsockopt error");
    }

	addr.sin_family = AF_INET; // IPv4
	addr.sin_port = htons(port); // Convert from host byte order to network byte order
	int ip_convertation_result = inet_aton(
			ip.c_str(),
			&addr.sin_addr);
	if (ip_convertation_result == 0) {
		throw std::runtime_error("Convertation ip to network byte order failed!");
	}
	log("NBO Port:", addr.sin_port);


	int bindResult = bind(this->_socket_fd.get(), (struct sockaddr*) &addr, sizeof(addr));
	if (bindResult == -1) {
		throw std::runtime_error("Binding socket error");
	}
	log("Binding socket to port - OK");
}

void Tcp_Server::listenSocket(int max_queue) {
	int listen_result = listen(this->_socket_fd.get(), max_queue);
	if (listen_result == -1) {
		throw std::runtime_error("Listening socket error");
	}
	log("Listening socket - OK");
}

void Tcp_Server::acceptClient() {
	while(true) {
		struct sockaddr_in client_addr;
		socklen_t client_addr_size = sizeof(client_addr);
		int new_socket_fd = accept(this->_socket_fd.get(),
								   (struct sockaddr*) &client_addr,
								   &client_addr_size);
		if (new_socket_fd == -1) {
			throw std::runtime_error("Accepting socket error");
		}

		this->_mutex.lock();
		this->_clients.push_back(std::make_unique<Client>(new_socket_fd, client_addr));
		this->_clients.back()->setName("Client" + std::to_string(this->_clients.size()));
		this->_clients.back()->setEventHandler(std::bind(&Tcp_Server::clientEventsHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		this->_clients.back()->startListen();
		this->clientEventsHandler(*this->_clients.back(), ClientEvent::CONNECTED, "");
		this->_mutex.unlock();

		log("Accepting client - OK");
		log("Client IP: ", inet_ntoa(client_addr.sin_addr));
		log("Client PORT: ", ntohs(client_addr.sin_port));
		log("Client file descriptor: ", new_socket_fd);
		log("Client name: ", this->_clients.back()->getName());
	}
}

void Tcp_Server::closeSocket() {
	// close the socket
	if(close(this->_socket_fd.get()) == -1) {
		throw std::runtime_error("Closing the socket error");
	}
	log("Close the socket - OK");
}

std::vector<std::string> Tcp_Server::getClientsIp() {
	this->removeDeadClients();
	if (this->_clients.size() == 0) {
		return {};
	}
	log("Clients:", this->_clients.size());
	
	std::vector<std::string> clients_ip;
	this->_mutex.lock();
	for (size_t i = 0; i < this->_clients.size(); ++i) {
		clients_ip.push_back(this->_clients[i]->getIp());
		log("clients_ip[",i,"]:", clients_ip[i]);
	}
	this->_mutex.unlock();
	return clients_ip;
}

void Tcp_Server::removeDeadClients() {
	this->_mutex.lock();
	for (size_t i = 0; i < this->_clients.size(); ++i) {
		if (!this->_clients[i]->isConnected()) {
			log("the client", i, " is disconnected");
			this->_clients.erase(this->_clients.begin() + i);
		}
	}
	this->_mutex.unlock();
}

void Tcp_Server::clientEventsHandler(const Client &client, ClientEvent type, const std::string &msg) {
	switch(type) {
		case ClientEvent::INCOMING_MSG:
			log("New message from the ", client.getName(), ": ", msg);
			this->notifyIncomingMessage(client.getName(), client.getIp(), msg);
			break;
		case ClientEvent::DISCONNECTED:
			log("Client ", client.getName(), " disconnected");
			this->notifyClientDisconnection(client.getName(), client.getIp());
			break;
		case ClientEvent::CONNECTED:
			log("Client ", client.getName(), " connected");
			this->notifyClientConnection(client.getName(), client.getIp());
			break;
		default:
			log("Unexpected event was handled");
			break;
	}
}

void Tcp_Server::registerObserver(Server_Observer *observer) {
	this->_observers.push_back(observer);
}

void Tcp_Server::notifyClientConnection(const std::string &clientName, const std::string &clientIp) const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->connectionHandler) {
			this->_observers[i]->connectionHandler(clientName, clientIp);
		}
	}
}

void Tcp_Server::notifyClientDisconnection(const std::string &clientName, const std::string &clientIp) const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->disconnectionHandler) {
			this->_observers[i]->disconnectionHandler(clientName, clientIp);
		}
	}
}

void Tcp_Server::notifyIncomingMessage(const std::string &clientName, const std::string &clientIp, const std::string &msg) const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->incomingMessageHandler) {
			this->_observers[i]->incomingMessageHandler(clientName, clientIp, msg);
		}
	}
}
