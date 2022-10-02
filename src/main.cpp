#include "Logger.hpp"
#include "Webserv.hpp"

int main (int argc, char** argv) {
	if (argc != 2)
		Logger::exitMessage("Wrong number of arguments\nUsage:\nwebserv <config_file>");

	Webserv webserv(argv[1]);

	while(true) {
		webserv.walkLoop();
	}
}
