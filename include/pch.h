#pragma once

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <sstream>
#include <cstdint>

namespace server_commands { // commands that server will parse. All values shoul have the same size
	const char SEND_MESSAGE [8] = "message";
	const char NICK_CHANGE  [8] = "nicknam";
};

#define MAX_PACKET_SIZE 1024

#define REQUEST_SEPARATOR ":"
