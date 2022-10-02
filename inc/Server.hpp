#pragma once

#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Location.hpp"
#include "Connection.hpp"
#include "Logger.hpp"

#define PENDING_LIMIT 1

class Connection;

class Server {
private:
	std::string 				_addr; //127.0.0.1
	unsigned int 				_port; //8080
	std::string 				_name; //server_name   one.example.com  www.one.example.com;
	int 						_sd;
	int 						_bodySize; //  client_max_body_size       10m;
	bool 						_autoindex; //  autoindex    on;

	std::string					_index; //стартовая страница
	std::vector<std::string>	_allowMethods; //принимаемые методы
	std::string					_error_page; //станица ошибки
	std::string					_root; //корневой каталог
	std::string					_uploadPath; //путь загрузки
	
	int 						_connectionLimit;
	std::vector<Connection*> 	_connections;
	std::vector<Location*>		_locations;
	
	inline bool exists(const std::string& name);


public:
	Server();
	// Server(std::vector<Location *> *locations);
	~Server();

	// void	addLocation();
	void	start(void);

	void 	addConnection();

	int 	getDescriptor() const;
	int 	appendConnectionDescriptors(fd_set* fdSet);
	void 	processConnections(fd_set* readSet, fd_set* writeSet);
	
	std::vector<Location*> getLocation(void);

	void	setAddr(const std::string &adr);
	void	setPort(int port);
	void	setName(const std::string &name);
	void	setBodySize(int maxBodySyze);
	void	setAutoindex(bool status);
	void	setRoot(const std::string &root);
	void	addMethod(const std::string &root);
	void	setIndex(std::string &index);
	void	setLocation(std::vector<Location*>);
	void	setUploadPath(std::string &upload);
	std::string& getName(void);
	int		getPort(void);
};

std::ostream& operator << (std::ostream& o, const Server server);
