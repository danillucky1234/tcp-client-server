#include "../include/tcp_client.h"

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

Tcp_Client::Tcp_Client() {
	this->_isConnected = false;
}

bool Tcp_Client::connectTo(const std::string &ip, size_t port) {
	try {
		this->createSocket(
				AF_INET,     // IPv4
				SOCK_STREAM, // Full-duplex connection
				IPPROTO_TCP  // TCP
				);
		this->connectSocket(ip, port);
		std::thread t1([&]{ this->receiveTask(); });
		t1.detach();
	} catch (const std::runtime_error &ex) {
		std::cout << "Error code: " << errno << std::endl;
		perror(ex.what());
		return false;
	}
	return true;
}

Tcp_Client::~Tcp_Client() {
	this->closeSocket();
}

void Tcp_Client::createSocket(int domain, int type, int protocol) {
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd == -1) {
		throw std::runtime_error("Socket creation failed!");
	}
	this->_socket_fd.set(sock_fd);
	log("Socket creation - OK");
	log("Socket file descriptor:", sock_fd);
}

void Tcp_Client::connectSocket(const std::string &ip, size_t port) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; // IPv4
	addr.sin_port = htons(port); // Convert from host byte order to network byte order
	int ip_convertation_result = inet_aton(
			ip.c_str(),
			&addr.sin_addr);
	if (ip_convertation_result == 0) {
		throw std::runtime_error("Convertation ip to network byte order failed!");
	}
	log("NBO Port:", addr.sin_port);

	int connect_result = connect(this->_socket_fd.get(),
			(struct sockaddr*)&addr,
			sizeof(addr));
	if (connect_result == -1) {
		throw std::runtime_error("Connection to server failed!");
	}
	this->_isConnected = true;
	this->notifyConnection();
	log("Connection - OK");
}

void Tcp_Client::closeSocket() {
	if(close(this->_socket_fd.get()) == -1) {
		throw std::runtime_error("Closing the socket error");
	}
	log("Close the socket - OK");
}

void Tcp_Client::writeMessage(const std::string &msg) const {
	int send_result = write(this->_socket_fd.get(), msg.c_str(), msg.size());
	if (send_result == -1) {
		throw std::runtime_error("Sending the message to server error");
	}

	if (send_result > 0) {
		log("Sending message to the server - OK");
	}
	log("Send to the server: ", msg);
}

void Tcp_Client::receiveTask() {
	char buffer[MAX_PACKET_SIZE] = { 0 };
	int msg_size;
	std::string messageFromTheServer = "";
	int readMessageSize = 0;
	bool isReadAgain = false;

	while(this->_isConnected) {
		int bytesRead = read(this->_socket_fd.get(), buffer, MAX_PACKET_SIZE);
		if (bytesRead == -1) { // reading error
			throw std::runtime_error("Reading from socket error");
		} else if (bytesRead == 0) { // server disconnected
			log("Server closed connection");
			this->_isConnected = false;
			this->notifyServerDisconnection();
		} else {
			log("New message from the server: ", buffer);
			if (isReadAgain) {
				// at this moment bytesRead is equal to message size
				if (readMessageSize + bytesRead < msg_size) {
					messageFromTheServer += buffer;
					readMessageSize += messageFromTheServer.size(); // remember the first cutted message size
					std::memset(buffer, 0, sizeof(buffer)); // zero buffer
					continue;
				}
				messageFromTheServer += buffer;
				// at this moment the message is fully written to the variable messageFromTheServer
				isReadAgain = false;
				readMessageSize = 0;
				this->notifyIncomingMessage(messageFromTheServer);
				std::memset(buffer, 0, sizeof(buffer)); // zero buffer
				continue;
			}

			// we can be here only if we read from socket the first time
			msg_size = std::atoi(std::strtok(buffer, REQUEST_SEPARATOR));
			messageFromTheServer = std::strtok(nullptr, "\n");

			if (bytesRead < msg_size) {
				isReadAgain = true;
				readMessageSize = messageFromTheServer.size(); // remember the first cutted message size
				std::memset(buffer, 0, sizeof(buffer)); // zero buffer
				continue; // read from the socket again
			}

			this->notifyIncomingMessage(messageFromTheServer);
			std::memset(buffer, 0, sizeof(buffer));
		}
	}
}

void Tcp_Client::registerObserver(Client_Observer *observer) {
	this->_observers.push_back(observer);
}

void Tcp_Client::notifyConnection() const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->connectionHandler) {
			this->_observers[i]->connectionHandler();
		}
	}
}

void Tcp_Client::notifyServerDisconnection() const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->disconnectionHandler) {
			this->_observers[i]->disconnectionHandler();
		}
	}
}

void Tcp_Client::notifyIncomingMessage(const std::string &msg) const {
	for (size_t i = 0; i < this->_observers.size(); ++i) {
		if (this->_observers[i]->incomingMessageHandler) {
			this->_observers[i]->incomingMessageHandler(msg);
		}
	}
}

void Tcp_Client::changeNickname(const std::string &msg) const {
	std::stringstream ss;
	ss << server_commands::NICK_CHANGE << REQUEST_SEPARATOR << msg.size() << REQUEST_SEPARATOR << msg;
	this->writeMessage(ss.str());
}

void Tcp_Client::sendMessage(const std::string &msg) const {
	std::stringstream ss;
	ss << server_commands::SEND_MESSAGE << REQUEST_SEPARATOR << msg.size() << REQUEST_SEPARATOR << msg;
	this->writeMessage(ss.str());
}
