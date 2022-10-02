#pragma once

#include <string>
#include <map>
#include <sys/stat.h>
#include "HTTPStatus.hpp"
#include "Location.hpp"
#include "Logger.hpp"
#include "util.hpp"

#include <cstring>


#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

typedef enum {
	RESSTATE_OPEN,
	RESSTATE_STATUS,
	RESSTATE_HEAD,
	RESSTATE_BODY,
	RESSTATE_DONE
} t_resstate;

class Response {
private:
	t_resstate	_state;
	int			_status;
	t_head		_headers;
	std::string	_buf;
	FILE*		_src;
	std::string	_errorContent;

	std::string	_contentType;

	static std::map<int, HTTPStatus*> _httpStatuses;
	static std::map<int, HTTPStatus*> _initHTTPStatuses();

	std::map<std::string, std::string> mimeTypes;

	void _craftErrorContent();
	void _craftStatus();
	void _craftHeader();
	size_t _copyBuffer(char* buf, size_t capacity);
	size_t _copyContent(char* buf, size_t capacity);

public:
	Response();
	~Response();

	void setStatus(int status);
	int getStatus();
	void addHeader(std::string name, std::string value);
	void setSource(std::string path, Location* location);
	size_t getContentLength();

	ssize_t getData(char *buf, size_t capacity);


	void initMIMETypes();
	// std::string getMIME() const;
	std::string	getContentType();


	static const int statusEmpty = 0;
};
