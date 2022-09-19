#include "../include/tcp_server.h"

int getUserInput();
void showMenu();
void onClientConnected(const std::string &clientName, const std::string &clientIp);
void onClientDisconnected(const std::string &clientName, const std::string &clientIp);
void onIncomingMessage(const std::string &clientName, const std::string &clientIp,
					   const std::string &msg);
void onClientNameChanged(const std::string &oldName, const std::string &newName, const std::string &clientIp);
std::string getCurrentTime();

int main() {
	int choice;
	std::string msg;
	bool isNeedTerminate = false;
	std::vector<std::pair<std::string,std::string>> clients_info;

	Server_Observer *observer = new Server_Observer();
	observer->connectionHandler = onClientConnected;
	observer->disconnectionHandler = onClientDisconnected;
	observer->incomingMessageHandler = onIncomingMessage;
	observer->nickChangingHandler = onClientNameChanged;

	Tcp_Server *server = new Tcp_Server();
	server->registerObserver(observer);
	bool isSuccessful = server->start("0.0.0.0", 65123);
	if (!isSuccessful) {
		isNeedTerminate = true;
	}

	while(!isNeedTerminate) {
		// get input from user
		showMenu();
		choice = getUserInput();

		switch(choice) {
			case 1: // get user ip
				clients_info = server->getClientsInfo();
				if (clients_info.size() == 0) {
					std::cout << "No connected clients" << std::endl;
				} else {
					for (size_t i = 0; i < clients_info.size(); ++i) {
						std::cout << clients_info[i].first << ": " << clients_info[i].second << std::endl;
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
	delete observer;
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

void onClientConnected(const std::string &clientName, const std::string &clientIp) {
	std::cout << "[" << getCurrentTime() << "] " << clientName << " [" << clientIp << "] was connected" << std::endl;
}

void onClientDisconnected(const std::string &clientName, const std::string &clientIp) {
	std::cout << "[" << getCurrentTime() << "] " << clientName << " [" << clientIp << "] was disconnected" << std::endl;
}

void onIncomingMessage(const std::string &clientName, const std::string &clientIp,
					   const std::string &msg) {
	std::cout << "[" << getCurrentTime() << "] " << clientName << " [" << clientIp << "]: " << msg << std::endl;
}

std::string getCurrentTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[9]; // 12:12:12 + '\0'
    tstruct = *localtime(&now);
	std::strftime(buf, sizeof(buf), "%X", &tstruct);
    return buf;
}

void onClientNameChanged(const std::string &oldName, const std::string &newName, const std::string &clientIp) {
	std::cout << oldName << " changed his nickname to " << newName << std::endl;
}
