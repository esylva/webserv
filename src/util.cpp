#include "util.hpp"

std::vector<std::string> split (std::string str, std::string delim) {
	size_t pos;
	std::vector<std::string> res;

	while ((pos = str.find(delim)) != std::string::npos) {
		res.push_back(str.substr(0, pos));
		str.erase(0, pos + delim.length());
	}
	res.push_back(str);
	return res;
}
