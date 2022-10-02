#pragma once

#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <ctime>
#include "Server.hpp"
#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Logger.hpp"
#include "util.hpp"

#define BUF_SIZE 64
#define CON_TIMEOUT 10

typedef enum {
	CONSTATE_EMPTY,
	CONSTATE_READ,
	CONSTATE_EXEC,
	CONSTATE_WRITE,
	CONSTATE_DONE
} t_constate;

class Connection {
private:
	int         _fd;
	std::vector<Location*> _locations;
	t_constate	_state;
	Request*	_request;
	Response*	_response;
	time_t		_timeout;

	Location* _findLocation(std::string path);

	void _initTransaction();
	void _read();
	void _exec();
	void _write();
	void _closeTransaction();

public:
	Connection(int sd, std::vector<Location*> locations);
	~Connection();

	int getDescriptor() const;
	int processConnection(fd_set* readSet, fd_set* writeSet);
};

std::ostream& operator << (std::ostream& o, const Connection connection);
