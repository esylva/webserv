#include "Webserv.hpp"

Webserv::Webserv(const std::string& cfgPath) {
	Logger::info("Init webserv");
	this->_serverFdMax = 0;
	FD_ZERO(&this->_serverFdSet);
	this->parseConfig(cfgPath);
	Logger::info("Start webserv");
}

Webserv::~Webserv() {
	Logger::info("Destroy webserv");
}

void Webserv::parseConfig(const std::string& cfgPath) {
	//	may be need to set default config
	if(!checkFile(cfgPath)) {
		Logger::exitMessage("Can't read config file");
	}
	else {
		// тут cfg_path бьется на отдельные cfg и передается дальше
		_servers = makeServers(cfgPath);

	}

	for (std::vector<Server*>::iterator i = _servers.begin(); i != _servers.end(); i++) {

		(*i)->start();
		this->addServer((*i));
	}


}

void Webserv::addServer(Server* server) {
	int serverDescriptor = server->getDescriptor();

	FD_SET(serverDescriptor, &this->_serverFdSet);
	if (serverDescriptor > this->_serverFdMax) {
		this->_serverFdMax = serverDescriptor;
	}
// checking code
	// std::cout << server->getName() << std::endl;
	// std::vector<Location*> loc = server->getLocation();
	// for (std::vector<Location*>::iterator i = loc.begin(); i != loc.end(); i++) {
	// std::cout << "root " << (*i)->getRoot() << std::endl;
	// std::cout << "path " << (*i)->getPath() << std::endl;
	
	// std::vector<std::string> tokens = (*i)->getPathTokens();
	// 	for (std::vector<std::string>::iterator j = tokens.begin(); j != tokens.end(); j++) {
	// 		std::cout << "token " << (*j) << std::endl;
	// 	}
	// }
//end of code	
}

void Webserv::walkLoop() {
	int fdMax = this->_serverFdMax;
	fd_set readFdSet = this->_serverFdSet;

//	Добавляю дескрипторы открытых соединений в список на прослушивание
	for (std::vector<Server*>::iterator iter = this->_servers.begin(); iter < this->_servers.end(); ++iter) {
		int connectionFdMax = (*iter)->appendConnectionDescriptors(&readFdSet);
		if (connectionFdMax > fdMax)
			fdMax = connectionFdMax;
	}
	fd_set writeSet = readFdSet;

//	Жду пока не появится активный дескриптор (или несколько)
	if (select(fdMax + 1, &readFdSet, &writeSet, NULL, NULL) < 0) {
		Logger::error("Select error");
	}

	for (std::vector<Server*>::iterator iter = this->_servers.begin(); iter < this->_servers.end(); ++iter) {
//		Если дескиптор сервера остался в readFdSet, значит это новый клиент ждет соединения
		if (FD_ISSET((*iter)->getDescriptor(), &readFdSet)) {
			Logger::info("New connection at socket %d", (*iter)->getDescriptor());
			(*iter)->addConnection();
		}

//		Обрабатываю дескрипторы соединений
		(*iter)->processConnections(&readFdSet, &writeSet);
	}
}

bool Webserv::checkFile(const std::string& cfgPath) {
	
	std::ifstream fin(cfgPath.c_str());
	if (!fin.is_open()) {
		fin.close();
		return (false);
	}
	fin.close();
	return (true);
}

std::vector<Server*> Webserv::makeServers(const std::string& cfgPath) {
	
	std::vector<Server*> servers;
	std::string	buffer;
	std::string line;
	std::ifstream fin(cfgPath.c_str());
	Lexer *lexer = new Lexer();

	line.clear();
	while (!fin.eof()) {
		getline(fin, buffer);
		line.append(buffer + '\n');
	}
	fin.close();
	
	lexer->handleLine(line);
	servers = lexer->parse_server();	
	delete (lexer);
	
	return (servers);
}