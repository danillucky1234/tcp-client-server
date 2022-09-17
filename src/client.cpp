#include "tcp_client.h"

int main() {
	Tcp_Client *client = new Tcp_Client("127.0.0.1", 65123);
	
	char buffer[1024] = { 0 };
	client->readMessage(buffer);
	std::cout << "Server: " << buffer << std::endl; 

	const std::string msg = "Hi! This is response from the client!";
	client->sendMessage(msg);

	delete client;
	return 0;
}
