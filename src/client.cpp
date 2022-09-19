#include "../include/client.h"

Client::Client() {
	this->_fd.set(-1);
	this->_addr = {};
	this->setConnected(false);
	this->setName("John Doe");
}

Client::Client(int fd, const struct sockaddr_in &addr) {
	this->_fd.set(fd);
	this->_addr = addr;
	this->setConnected(false);
	this->setName("John Doe");
}

int Client::getFd() const {
	return this->_fd.get();
}

bool Client::isConnected() const {
	return this->_isConnected;
}

void Client::setConnected(bool flag) {
	this->_isConnected = flag;
}

std::string Client::getIp() const {
	return inet_ntoa(this->_addr.sin_addr);
}

void Client::setName(const std::string &name) {
	this->_name = name;
}

std::string Client::getName() const {
	return this->_name;
}

void Client::startListen() {
	this->setConnected(true);
	std::thread t1([&] { this->receiveTask(); });
	t1.detach();
}

void Client::setEventHandler(const client_event_handler &handler) {
	this->_eventHandlerCallback = handler;
}

void Client::receiveTask() {
	char buffer[MAX_PACKET_SIZE] = { 0 };
	char command_prefix[sizeof(server_commands::SEND_MESSAGE)];
	int msg_size;
	std::string messageFromTheClient = "";
	int readMessageSize = 0;
	bool isReadAgain = false;

	while(this->isConnected()) {
		int bytesRead = read(this->getFd(), buffer, MAX_PACKET_SIZE);
		if (bytesRead == -1) { // reading error
			throw std::runtime_error("Reading from socket error");
		} else if (bytesRead == 0) { // client disconnected
			std::string disconnectionMsg = "Client " + this->getName() + " closed connection";
			this->_eventHandlerCallback(*this, ClientEvent::DISCONNECTED, disconnectionMsg);
			this->setConnected(false);
		} else {
			if (isReadAgain) {
				// at this moment bytesRead is equal to message size
				if (readMessageSize + bytesRead < msg_size) {
					messageFromTheClient += buffer;
					readMessageSize += messageFromTheClient.size(); // remember the first cutted message size
					std::memset(buffer, 0, sizeof(buffer)); // zero buffer
					continue;
				}
				messageFromTheClient += buffer;
				// at this moment the message is fully written to the variable messageFromTheClient
				isReadAgain = false;
				readMessageSize = 0;
				this->handleReadMessage(command_prefix, messageFromTheClient);
				std::memset(buffer, 0, sizeof(buffer)); // zero buffer
				continue;
			}

			// we can be here only if we read from socket the first time
			std::strcpy(command_prefix, std::strtok(buffer, REQUEST_SEPARATOR));
			msg_size = std::atoi(std::strtok(nullptr, REQUEST_SEPARATOR));
			messageFromTheClient = std::strtok(nullptr, "\n"); // remember the first cutted message from the socket

			if (bytesRead < msg_size) {
				isReadAgain = true;
				readMessageSize = messageFromTheClient.size(); // remember the first cutted message size
				std::memset(buffer, 0, sizeof(buffer)); // zero buffer
				continue; // read from the socket again
			}

			this->handleReadMessage(command_prefix, messageFromTheClient);
			std::memset(buffer, 0, sizeof(buffer));
		}
	}
}

void Client::handleReadMessage(const char *command_prefix, const std::string &messageFromTheClient) {
	if (strcmp(command_prefix, server_commands::SEND_MESSAGE) == 0) {
		this->_eventHandlerCallback(*this, ClientEvent::INCOMING_MSG, messageFromTheClient);
	} else if (strcmp(command_prefix, server_commands::NICK_CHANGE) == 0) {
		std::string oldNickname = this->getName();
		this->setName(messageFromTheClient);
		this->_eventHandlerCallback(*this, ClientEvent::NICK_CHANGED, oldNickname);
	}
}
