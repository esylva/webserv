#include "Location.hpp"

Location::Location(){
	_index = "index.html";
	_autoindex = false;
}

Location::Location(const std::string &path, const std::string &root) {
	_index = "index.html";
	_autoindex = false;
	this->_path = path;
	this->_pathTokens = split(this->_path, "/");
	this->_root = root;
	if (*this->_root.end() != '/')
		this->_root.append("/");
	Logger::info("Bind location path %s to root %s", this->_path.c_str(), this->_root.c_str());
}

Location::~Location() {
	Logger::info("Destroy Location");
}

size_t Location::comparePathTokens(std::vector<std::string> tokens) {

	size_t depth = 0;
	std::vector<std::string>::iterator self;
	std::vector<std::string>::iterator that;
	for (self = this->_pathTokens.begin(),that = tokens.begin();
			self < this->_pathTokens.end() && that < tokens.end();
			++self, ++that) {
		if (self->compare(*that) == 0)
			++depth;
		else
			break;
	}
	if (depth < this->_pathTokens.size() - 1){
		return 0;
	
	}
	return depth;
}

//getters

std::string Location::getPath() const {
	return this->_path;
}

std::string Location::getRoot() const {
	return this->_root;
}

std::string Location::getFullPath(std::string path) {
	for (int cnt = this->_pathTokens.size() - 1; cnt > 0; --cnt) {
		path.erase(0, path.find("/") + 1);
	}
	return this->_root + path;
}

std::vector<std::string> Location::getPathTokens(void) const {
	return (_pathTokens);
}

//setters 

void Location::setPath(const std::string& path) {
	this->_path = path;	
	if (*(this->_path.end() - 1) != '/'){
		this->_path.push_back('/');
	}
	this->_pathTokens = split(this->_path, "/");
}

void Location::setRoot(const std::string &root) {
	_root = root;
	if (*(this->_root.end() - 1) != '/'){
		this->_root.push_back('/');
	}
}

void Location::setCgiInfo(const std::string &cgiInfo) {
	_cgiInfo = cgiInfo;
}

void Location::setRedirection(const std::string &redirection) {
	_redirection = redirection;
}

void Location::setAllowMethods(std::vector<std::string> &allowMethods) {
	_allowMethods = allowMethods;
}

void Location::setIndex(std::string &index) {
	_index = index;
}

void Location::setBodySize(int maxBodySyze) {
	_bodySize = maxBodySyze;
}

void	Location::setAutoindex(bool status) {
	_autoindex = status;
}

void	Location::addMethod(const std::string &method) {
	this->_allowMethods = split(method, "/");

	for (std::vector<std::string>::iterator k  = _allowMethods.begin(); k != _allowMethods.end(); k++) {
		if ((!(*k).compare("GET")) || !((*k).compare("POST")) || (!(*k).compare("DELETE"))) {
			continue;
		}
		else {
			Logger::exitMessage("Syntax error: unknown method");
		}
	}
}
