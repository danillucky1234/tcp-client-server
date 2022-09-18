#include <limits>
#include "../include/tcp_client.h"

int getUserInput();
void showMenu();
void onConnected();
void onServerDisconnected();
void onIncomingMessage(const std::string &msg);
std::string getCurrentTime();

int main() {
	int choice;
	std::string msg;
	bool isNeedTerminate = false;

	Client_Observer *observer = new Client_Observer();
	observer->connectionHandler = onConnected;
	observer->disconnectionHandler = onServerDisconnected;
	observer->incomingMessageHandler = onIncomingMessage;

	Tcp_Client *client = new Tcp_Client();
	client->registerObserver(observer);
	client->connectTo("127.0.0.1", 65123);

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
	delete observer;
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

void onConnected() {
	std::cout << "[" << getCurrentTime() << "] " << "Successfully connected to the server" << std::endl;
}

void onServerDisconnected() {
	std::cout << "[" << getCurrentTime() << "] " << "Server closed connection" << std::endl;
}

void onIncomingMessage(const std::string &msg) {
	std::cout << "[" << getCurrentTime() << "] " << "Server: " << msg << std::endl;
}

std::string getCurrentTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[9]; // 12:12:12 + '\0'
    tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}
