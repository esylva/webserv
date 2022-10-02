#include "Server.hpp"

Server::Server() {
	this->_addr = "127.0.0.1";
	this->_port = 80;
	this->_name = "localhost";
}

// Server::Server(std::vector<Location *> *locations) {
// 	this->_locations = *locations;
// }

//непонятная хрень в строке 27, пришлось закомментировать для компиляции
void Server::start() {
	struct sockaddr_in	sockaddr;
	int opt = 1;
	this->_connectionLimit = PENDING_LIMIT;

//	Открыл дескриптор для сетевого сокета
	this->_sd = socket(AF_INET, SOCK_STREAM, 0);
	Logger::info("Create server at socket %d", this->_sd);
	if (this->_sd < 0) {
		Logger::exitFatal("socket creation");
	}
	if (setsockopt(this->_sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) 
	|| setsockopt(this->_sd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) 
	// || setsockopt(this->_sd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&opt, sizeof(opt))
	) {
		Logger::exitFatal("socket options");
	}

//	Связал дескриптор с сетевым адресом
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(_port);
	if (bind(this->_sd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		Logger::exitFatal("socket bind");
	}

//	Настроил дескриптор в режим прослушивания для accept()
	if (listen(this->_sd, this->_connectionLimit) < 0) {
		Logger::exitFatal("socket listen");
	}
}

Server::~Server() {
	Logger::info("Destroy server");
	close(this->_sd);
	for (std::vector<Connection*>::iterator iter = this->_connections.begin(); iter != this->_connections.end(); ++iter) {
		delete *iter;
		iter = this->_connections.erase(iter) - 1;
	}
	for (std::vector<Location*>::iterator iter = this->_locations.begin(); iter != this->_locations.end(); ++iter) {
		delete *iter;
		iter = this->_locations.erase(iter) - 1;
	}
}

std::ostream& operator << (std::ostream& o, const Server& server) {
	char msg[50];
	sprintf(msg, "Server(fd %d)", server.getDescriptor());
	return o << msg;
}

// void Server::addLocation() {
// 	this->_locations.push_back(new Location("/", "test/www"));
// 	this->_locations.push_back(new Location("/src/", "src"));
// 	this->_locations.push_back(new Location("/root/", "."));
// }

void Server::addConnection() {

// std::vector<Location*>::iterator iter;
// for (iter = this->_locations.begin(); iter < this->_locations.end(); ++iter) {
// Logger::debug("Location is %s\n", (*iter)->getRoot().c_str());
// }

	Connection* connection = new Connection(this->_sd, this->_locations);
	if (connection->getDescriptor() < 0)
		delete connection;
	else
		this->_connections.push_back(connection);
}

int Server::getDescriptor() const {
	return this->_sd;
}

int Server::appendConnectionDescriptors(fd_set *fdSet) {
	int connectionFd;
	int connectionFdMax = 0;

	for (uint cnct = 0; cnct < this->_connections.size(); ++cnct) {
		connectionFd = this->_connections[cnct]->getDescriptor();
		FD_SET(connectionFd, fdSet);
		if (connectionFd > connectionFdMax)
			connectionFdMax = connectionFd;
	}
	return connectionFdMax;
}

void Server::processConnections(fd_set* readSet, fd_set* writeSet) {
	for (std::vector<Connection*>::iterator iter = this->_connections.begin(); iter != this->_connections.end(); ++iter) {
		if ((*iter)->processConnection(readSet, writeSet) != 0) {
			delete *iter;
			iter = this->_connections.erase(iter) - 1;
		}
	}
}

std::vector<Location*> Server::getLocation(void) {
	return (_locations);
	}
//setters

void	Server::setAddr(const std::string &adr){
	this->_addr = adr;
}

void	Server::setPort(int port) {
	this->_port = port;
}

void	Server::setName(const std::string &name) {
	this->_name = name;
}

void	Server::setBodySize(int maxBodySyze) {
	this->_bodySize = maxBodySyze;
}

void	Server::setAutoindex(bool status) {
	_autoindex = status;
}

void	Server::setRoot(const std::string &root) {
	_root = root;
	if (*(this->_root.end() - 1) != '/'){
		this->_root.push_back('/');
	}
}

void	Server::addMethod(const std::string &method) {
	_allowMethods.push_back(method);
}

void	Server::setIndex(std::string &index) {
	_index = index;
}

void	Server::setUploadPath(std::string &upload) {
	_uploadPath = upload;
	if (!(exists(upload))) {
		std::vector<std::string> tmpVector = split(upload, "/");
		std::string *tmpStr = new std::string();

		for (std::vector<std::string>::iterator i = tmpVector.begin(); i != tmpVector.end(); i++) {
			*tmpStr = (*tmpStr).append(*i);
				if (!(exists(*tmpStr))) {
					mkdir((*tmpStr).c_str(), 777);
				}
			*tmpStr = (*tmpStr).append("/");
		}
		delete tmpStr;
	}
}

//setters

std::string& Server::getName(void) {
	return (_name);
}

void	Server::setLocation(std::vector<Location *> locations) {
	_locations = locations;
}

int		Server::getPort(void) {
	return (_port);
}

inline bool Server::exists(const std::string& name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0); 
}