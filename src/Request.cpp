#include "Request.hpp"
#include "Logger.hpp"

Request::Request() {
	this->_state = REQSTATE_METHOD;
	this->_path = "";
	this->_buf = "";
}

Request::~Request() {
}

int Request::parse(char* fragment) {
	this->_buf.append(fragment);
	int res = 500;
	switch (this->_state) {
		case REQSTATE_METHOD:
			res = this->_parseMethod();
			// fall through
		case REQSTATE_PATH:
			if (this->_state == REQSTATE_PATH)
				res = this->_parsePath();
			// fall through
		case REQSTATE_PROTOCOL:
			if (this->_state == REQSTATE_PROTOCOL)
				res = this->_parseProtocol();
			// fall through
		case REQSTATE_HEADER:
			if (this->_state == REQSTATE_HEADER)
				res = this->_parseHeader();
			// fall through
		case REQSTATE_BODY:
			if (this->_state == REQSTATE_BODY)
				res = this->_parseBody();
			// fall through
		default:
			return res;
	}
}

int Request::_error(int error) {
	this->_state = REQSTATE_DONE;
	return error;
}

int Request::_success() {
	switch (this->_state) {
		case REQSTATE_METHOD:
			this->_state = REQSTATE_PATH;
			return 0;
		case REQSTATE_PATH:
			this->_state = REQSTATE_PROTOCOL;
			return 0;
		case REQSTATE_PROTOCOL:
			this->_state = REQSTATE_HEADER;
			return 0;
		case REQSTATE_HEADER:
			if (this->_method == HTTP_POST) {
				this->_state = REQSTATE_BODY;
				return 0;
			} else {
				this->_state = REQSTATE_METHOD;
				return 200;
			}
		case REQSTATE_BODY:
			this->_state = REQSTATE_METHOD;
			return 200;
		default:;
			return 500;
	}
}

int Request::_parseMethod() {
	size_t pos = this->_buf.find(' ', 0);
	if (pos == std::string::npos) {
		if (this->_buf.size() > 7)
			return this->_error(501);
		else
			return 0;
	}
	if (this->_buf.compare(0, 5, "HEAD ") == 0) {
		this->_method = HTTP_HEAD;
		this->_buf.erase(0, 5);
		return this->_success();
	} else if (this->_buf.compare(0, 4, "GET ") == 0) {
		this->_method = HTTP_GET;
		this->_buf.erase(0, 4);
		return this->_success();
	} else if (this->_buf.compare(0, 5, "POST ") == 0) {
		this->_method = HTTP_POST;
		this->_buf.erase(0, 5);
		return this->_success();
	} else if (this->_buf.compare(0, 7, "DELETE ") == 0) {
		this->_method = HTTP_DELETE;
		this->_buf.erase(0, 7);
		return this->_success();
	} else {
		return this->_error(501);
	}
}

int Request::_parsePath() {
	size_t pos = this->_buf.find(' ', 0);
	if (pos == std::string::npos) {
		if (this->_buf.size() > MAX_URL_LEN) {
			return this->_error(414);
		} else
			return 0;
	}
	
	// if (this->_buf[0] == '/') {
	// 	this->_buf = this->_buf.substr(1);
	// }
	// std::cout << "BUF " << this->_buf << std::endl << std::endl;
	for (size_t ind = 0; ind < pos; ++ind) {
		char ch = this->_buf[ind];
		if (ch >= '/' && ch <= ';')
			break;
		if (ch >= 'A' && ch <= 'Z')
			break;
		if (ch >= '#' && ch <= ',')
			break;
		if (ch == '!' || ch == '=' || ch == '?' || ch == '@' || ch == '[' || ch == ']')
			break;
		return this->_error(400);
	}
	this->_path = this->_buf.substr(0, pos);
	this->_buf.erase(0, pos + 1);
	return this->_success();
}

int Request::_parseProtocol() {
	size_t pos = this->_buf.find("\r\n", 0);
	if (pos == std::string::npos) {
		if (this->_buf.size() > 10) {
			return this->_error(400);
		} else
			return 0;
	}
	if (this->_buf.compare(0, 10, "HTTP/1.1\r\n") != 0)
		return this->_error(505);
	this->_buf.erase(0, 10);
	return this->_success();
}

int Request::_parseHeader() {
	size_t end = this->_buf.find("\r\n", 0);
	size_t key_end, value_start;
	while (end != std::string::npos) {
		if (end == 0) {
			this->_buf.erase(0, 2);
			return this->_success();
		}

		key_end = this->_buf.find(":", 0);
		if (key_end == std::string::npos || key_end > end)
			return this->_error(400);

		value_start = key_end + 1;
		if (this->_buf[value_start] == ' ')
			++value_start;

		this->_addHeader(this->_buf.substr(0, key_end), this->_buf.substr(value_start, end - value_start));

		this->_buf.erase(0, end + 2);
		end = this->_buf.find("\r\n", 0);
	}
	return 0;
}

int Request::_parseBody() {
	const char* contentLength = this->getHeader("Content-Length");
	if (contentLength == NULL)
		return this->_error(400);
	size_t expectedLength = atol(contentLength);
	if (this->_buf.length() < expectedLength)
		return 0;
	else {
		// Logger::info("Request body is\n%s", this->_buf.substr(0, expectedLength).c_str());
		this->_buf = this->_buf.substr(this->_buf.find('-'));
		std::string key = this->_buf.substr(0, this->_buf.find("\r\n"));
		this->_buf = this->_buf.substr(this->_buf.find("\r\n") + 2);

		std::string filename;
		std::ofstream file1;
		while (this->_buf[0] != 0 && this->_buf[0] != '-' && this->_buf.find(key))
		{
			filename = "";
			filename = this->_buf.substr(this->_buf.find("filename=") + 10);
			filename = filename.substr(0, filename.find("\"") );
			if (filename == "") {
				if (this->_buf[0] != '-' && this->_buf.find(key)) {
					this->_buf = this->_buf.substr(this->_buf.find(key) + key.length());
				}
				continue;
			}
			filename.insert(0, "./files/");
			file1.open(filename.c_str(), std::ios::binary);
			if (file1.is_open())
			{
				std::string value = this->_buf.substr(this->_buf.find("\r\n\r\n") + 4, this->_buf.find(key) - this->_buf.find("\r\n\r\n") - 6);
				file1.write(value.c_str(), value.length());
				// strncpy(file1, value.c_str(), sizeof(value));
				// file1 << this->_buf.substr(this->_buf.find("\r\n\r\n") + 4, this->_buf.find(key) - this->_buf.find("\r\n\r\n") - 6) << std::endl;
				this->_buf = this->_buf.substr(this->_buf.find(key) + key.length());
			}
			else {
				return 0;
			}
			file1.close();
			this->_buf = this->_buf.substr(this->_buf.find("\r\n") + 2);
		}
		return this->_success();
	}
}

const char* Request::getHeader(std::string name) {
	if (this->_headers.find(name) == this->_headers.end())
		return NULL;
	return this->_headers[name].c_str();
}

void Request::_addHeader(std::string name, std::string value) {
	this->_headers[name] = value;
	Logger::trace("Add header %s: %s", name.c_str(), this->getHeader(name));
}

t_reqmethod Request::getMethod() {
	return this->_method;
}

std::string Request::getPath() {
	return this->_path;
}
