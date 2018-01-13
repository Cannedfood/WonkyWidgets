#include "Chat.hpp"

#include <iostream>
#include <string>

int main(int argc, char const** argv) {

	if(argc < 2) {
		printf("Usage:  %s <myport> [<connect to> <port>]\n", argv[0]);
		return -1;
	}

	uint16_t myport = strtol(argv[1], nullptr, 10);

	Chat client;
	client.onReceive([](const char* user, const char* msg) {
		printf("[%s] %s\n", user, msg);
	});

	client.listen(myport);

	if(argc == 4) {
		client.connect(argv[2], argv[3]);
	}

	std::string line;
	while(std::getline(std::cin, line)) {
		client.send(line);
	}

	return 0;
}
