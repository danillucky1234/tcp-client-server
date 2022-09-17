#include "tcp_server.h"

int main() {
	Tcp_Server *server = new Tcp_Server("127.0.0.1", 65123);

	const std::string msg = "Hello from server";

	/* while(true) { */
		server->acceptClient();
		server->sendMessage(msg);
	/* } */
	char buffer[1024] = { 0 };
	server->readMessage(buffer);
	std::cout << "Client: " << buffer << std::endl; 

	delete server;
	return 0;
}
