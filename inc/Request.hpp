#pragma once

#include <string>
#include "util.hpp"


#include <iostream>
#include <fstream>

#define MAX_URL_LEN 2028

typedef enum {
	REQSTATE_METHOD,
	REQSTATE_PATH,
	REQSTATE_PROTOCOL,
	REQSTATE_HEADER,
	REQSTATE_BODY,
	REQSTATE_DONE
} t_reqstate;

typedef enum {
	HTTP_HEAD,
	HTTP_GET,
	HTTP_POST,
	HTTP_DELETE
} t_reqmethod;

class Request {
private:
	t_reqstate	_state;
	std::string _buf;
	t_reqmethod	_method;
	std::string	_path;
	t_head		_headers;

	int _error(int error);
	int _success();

	int _parseMethod();
	int _parsePath();
	int _parseProtocol();
	int _parseHeader();
	int _parseBody();
	void _addHeader(std::string name, std::string value);

public:
	Request();
	~Request();

	t_reqmethod getMethod();
	std::string getPath();
	const char* getHeader(std::string name);

	int parse(char* fragment);
};
