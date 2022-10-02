#pragma once

#include <string>
#include <vector>
#include "Logger.hpp"
#include "util.hpp"

class Location {
private:
	std::string 				_path;
	std::string					_root;
	std::string					_index;
	std::string					_cgiInfo;
	std::string					_redirection;
	int							_bodySize;
	bool						_autoindex;
	std::vector<std::string>	_allowMethods;
	
	std::vector<std::string> 	_pathTokens;

	// root+path
	// redirect
	// uploadPath;
	// binPath;

public:
	Location();
	Location(const std::string &path, const std::string &root);
	~Location();

	size_t comparePathTokens(std::vector<std::string> tokens);

	std::string getPath(void) const;
	std::string getRoot(void) const;
	std::string getFullPath(std::string path);
	std::vector<std::string> getPathTokens(void) const;

	void	setPath(const std::string &path);
	void	setRoot(const std::string &root);
	void	setCgiInfo(const std::string &cgiInfo);
	void	setRedirection(const std::string &redirection);
	void	setIndex(std::string &index);
	void	setBodySize(int maxBodySyze);
	void	setAutoindex(bool status);
	void	addMethod(const std::string &root);
	void	setAllowMethods(std::vector<std::string> &allow_methods);
};

std::ostream& operator << (std::ostream& o, const Location location);
