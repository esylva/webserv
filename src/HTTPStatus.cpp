#include "HTTPStatus.hpp"

HTTPStatus::HTTPStatus(int num, std::string title, std::string description) {
	sprintf(this->_code, "%d", num);
	this->_title = title;
	this->_description = description;
}

HTTPStatus::~HTTPStatus() {};

const char* HTTPStatus::getCode() const {
	return this->_code;
}

std::string HTTPStatus::getTitle() const {
	return this->_title;
}

std::string HTTPStatus::getDescription() const {
	return this->_description;
}
