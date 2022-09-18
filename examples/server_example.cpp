#include "../include/tcp_server.h"

int getUserInput();
void showMenu();

int main() {
	Tcp_Server *server = new Tcp_Server("127.0.0.1", 65123);

	int choice;
	std::string msg;
	bool isNeedTerminate = false;

	std::vector<std::string> clients_ip;
	while(!isNeedTerminate) {
		// get input from user
		showMenu();
		choice = getUserInput();

		switch(choice) {
			case 1: // get user ip
				clients_ip = server->getClientsIp();
				if (clients_ip.size() == 0) {
					std::cout << "No connected clients" << std::endl;
				} else {
					for (size_t i = 0; i < clients_ip.size(); ++i) {
						std::cout << "Client " << i << ": " << clients_ip[i] << std::endl;
					}
				}
				break;
			case 2: // send message to the client
				std::cout << "Enter the message to the all clients:" << std::endl;
				std::cin.ignore();
				std::getline(std::cin, msg);
				server->sendMessage(msg);
				break;
			case 3: // exit
				isNeedTerminate = true;
				break;
			default:
				std::cout << "Invalid option";
				break;
		}
	}

	delete server;
	return 0;
}

void showMenu() {
	std::cout << "Pick one of the following items:" << std::endl
		      << "  1. Get connected clients ip" << std::endl
			  << "  2. Send message to the all clients" << std::endl
			  << "  3. Exit" << std::endl;
}

int getUserInput() {
	int minBound = 1;
	int maxBound = 3;
	int choice;
	while(true) {
		std::cin >> choice;
		if (choice < minBound || choice > maxBound) {
			std::cout << "Invalid input" << std::endl;
		} else {
			return choice;
		}
	}
}
