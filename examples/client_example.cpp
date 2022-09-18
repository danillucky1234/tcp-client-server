#include "../include/tcp_client.h"

int getUserInput();
void showMenu();

int main() {
	Tcp_Client *client = new Tcp_Client("127.0.0.1", 65123);
	
	int choice;
	std::string msg;
	bool isNeedTerminate = false;

	while(!isNeedTerminate) {
		// get input from user
		showMenu();
		choice = getUserInput();

		switch(choice) {
			case 1: // send message to the client
				std::cout << "Enter the message to the server:" << std::endl;
				std::cin.ignore();
				std::getline(std::cin, msg);
				client->sendMessage(msg);
				break;
			case 2: // exit
				isNeedTerminate = true;
				break;
			default:
				std::cout << "Invalid option";
				break;
		}
	}

	delete client;
	return 0;
}


void showMenu() {
	std::cout << "Pick one of the following items:" << std::endl
			  << "  1. Send message to the server" << std::endl
			  << "  2. Exit" << std::endl;
}

int getUserInput() {
	int minBound = 1;
	int maxBound = 2;
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

