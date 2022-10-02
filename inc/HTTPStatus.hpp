#pragma once

#include <string>
#include <map>

#include <cstdio>

class HTTPStatus {
private:
	char		_code[4];
	std::string	_title;
	std::string	_description;

public:
	HTTPStatus(int num, std::string title, std::string description);
	~HTTPStatus();

	const char* getCode() const;
	std::string getTitle() const;
	std::string getDescription() const;
};
