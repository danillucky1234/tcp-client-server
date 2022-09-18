#include "../include/client.h"

Client::Client() {
	this->_fd.set(-1);
	this->_addr = {};
	this->setConnected(false);
}

Client::Client(int fd, const struct sockaddr_in &addr) {
	this->_fd.set(fd);
	this->_addr = addr;
}

void Client::setFileDescriptor(int fd) {
	this->_fd.set(fd);
	this->setConnected(true);
}

void Client::setFileDescriptor(const FileDescriptor &fd) {
	this->_fd = fd;
}

void Client::setAddressInfo(const struct sockaddr_in &addr) {
	this->_addr = addr;
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
	char buffer[1024] = { 0 };
	while(this->isConnected()) {
		int read_result = read(this->getFd(), buffer, 1024);
		if (read_result == -1) { // reading error
			throw std::runtime_error("Reading from socket error");
		} else if (read_result == 0) { // client disconnected
			std::string disconnectionMsg = "Client " + this->getName() + " closed connection";
			this->_eventHandlerCallback(*this, ClientEvent::DISCONNECTED, disconnectionMsg);
			this->setConnected(false);
		} else {
			this->_eventHandlerCallback(*this, ClientEvent::INCOMING_MSG, std::string(buffer));
		}
	}
}
