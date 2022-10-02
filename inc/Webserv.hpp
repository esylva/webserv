#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Server.hpp"
#include "Logger.hpp"
#include "Lexer.hpp"


class Webserv {
private:
	std::vector<Server*> _servers;
	fd_set _serverFdSet;
	int _serverFdMax;

	bool checkFile(const std::string& cfgPath);
	std::vector<Server*> makeServers(const std::string& cfgPath);

public:
	Webserv(const std::string& cfgPath);
	~Webserv();

	void parseConfig(const std::string& cfgPath);
	// Server* addServer();
	void addServer(Server* server);
	 

	void walkLoop();
};
