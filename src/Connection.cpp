#include "Connection.hpp"

Connection::Connection(int sd, std::vector<Location*> locations) {
	struct sockaddr_in address;
	socklen_t len = sizeof(address);

	this->_fd = accept(sd, (struct sockaddr*)&address, &len);
	if (this->_fd < 0) {
		Logger::error("Connection accept error");
	} else {
		Logger::info("Open descriptor %d for socket %d %d:%d", this->_fd, sd, int(address.sin_addr.s_addr), ntohs(address.sin_port));
	}

	this->_state = CONSTATE_EMPTY;
	this->_locations = locations;
	this->_timeout = time(NULL) + CON_TIMEOUT;

// std::vector<Location*>::iterator iter;
// Logger::debug("А що там за небокраем?\n");

// for (iter = locations.begin(); iter < locations.end(); ++iter) {
// Logger::debug("Location is %s\n", (*iter)->getRoot().c_str());
// }

}

Connection::~Connection() {
	this->_closeTransaction();
	if (this->_fd >= 0) {
		Logger::info("Close connection descriptor %d", this->_fd);
		close(this->_fd);
	}
}

std::ostream& operator << (std::ostream& o, const Connection& connection) {
	char msg[50];
	sprintf(msg, "Connection(fd %d)", connection.getDescriptor());
	o << msg;
	return o;
}

int Connection::getDescriptor() const {
	return this->_fd;
}

Location* Connection::_findLocation(std::string path) {
	std::vector<std::string> pathTokens = split(path, "/");
	size_t maxDepth = 0;
	size_t depth;
	Location* location = NULL;

	std::vector<Location*>::iterator iter;
	for (iter = this->_locations.begin(); iter < this->_locations.end(); ++iter) {

// Logger::debug("Connection::_findLocation_2 is running. Path is %s\n", path.c_str());
// Logger::debug("Connection::_findLocation_2 is running. Path is %s\n", (*iter)->getRoot().c_str());


		depth = (*iter)->comparePathTokens(pathTokens);

		if (depth > 0)
			Logger::trace("Path %s corresponds to location %s", this->_request->getPath().c_str(), (*iter)->getPath().c_str());
		if (depth > maxDepth) {
			maxDepth = depth;
			location = *iter;
		}
	}
// Logger::debug("Connection::_findLocation_3 is running. Path is %s\n", path.c_str());

	if (location == NULL)
		Logger::warning("Location for path %s is undefined", this->_request->getPath().c_str());
	else
		Logger::debug("Location for path %s is %s", this->_request->getPath().c_str(), location->getPath().c_str());
	return location;
}

int Connection::processConnection(fd_set* readSet, fd_set* writeSet) {
	if (this->_timeout < time(NULL))
		this->_state = CONSTATE_DONE;
	switch (this->_state) {
		case CONSTATE_EMPTY:
			this->_initTransaction();
			// fall through
		case CONSTATE_READ:
			if (FD_ISSET(this->_fd, readSet))
				this->_read();
			return 0;
		case CONSTATE_EXEC:
			// Прокручиваю недочитанные данные в случае ошибки
			// иначе будут проблемы если следующий запрос придет в это же соединение
			if (FD_ISSET(this->_fd, readSet)) {
				char buffer[BUF_SIZE];
				int cnt = recv(this->_fd, buffer, BUF_SIZE - 1, 0);
				Logger::trace("Skip %d bytes from descriptor %d", cnt, this->_fd);
			} else
				this->_exec();
			return 0;
		case CONSTATE_WRITE:
			if (FD_ISSET(this->_fd, writeSet))
				this->_write();
			return 0;
		case CONSTATE_DONE:
		default:
			return 1;
	}
}

void Connection::_initTransaction() {
	if (this->_state != CONSTATE_EMPTY) return;
	this->_request = new Request();
	this->_response = new Response();
	this->_state = CONSTATE_READ;
	if (!this->_request || !this->_response)
		this->_closeTransaction();
}

void Connection::_read() {
	if (this->_state != CONSTATE_READ) return;
	this->_timeout = time(NULL) + CON_TIMEOUT;

	char buffer[BUF_SIZE];
	int cnt = recv(this->_fd, buffer, BUF_SIZE - 1, 0);
	if (cnt < 0) {
		Logger::error("Connection data receive error from descriptor %d", this->_fd);
		this->_state = CONSTATE_EXEC;
		this->_response->setStatus(500);
	} else if (cnt == 0) {
		this->_state = CONSTATE_DONE;
	} else {
		buffer[cnt] = '\0';
		Logger::debug("Read %d bytes from descriptor %d", cnt, this->_fd);
		Logger::trace("%s\n---", buffer);
		int requestStatus = this->_request->parse(buffer);
		if (requestStatus > 0) {
			this->_state = CONSTATE_EXEC;
			if (requestStatus == 200)
				Logger::debug("Finish reading from descriptor %d", this->_fd);
			else
				this->_response->setStatus(requestStatus);
		}
	}
}

void Connection::_exec() {
	if (this->_state != CONSTATE_EXEC) return;
	this->_timeout = time(NULL) + CON_TIMEOUT;

	Logger::info("Request to %s from descriptor %d", this->_request->getPath().c_str(), this->_fd);
	if (this->_response->getStatus() == Response::statusEmpty)
		this->_response->setSource(this->_request->getPath(), this->_findLocation(this->_request->getPath()));

	char tmp[10];
	sprintf(tmp, "%lu", this->_response->getContentLength());
	this->_response->addHeader("Content-Length", tmp);
	this->_response->addHeader("Content-Type", this->_response->getContentType());
	this->_response->addHeader("Connection", "keep-alive");
	this->_state = CONSTATE_WRITE;
}

void Connection::_write() {
	if (this->_state != CONSTATE_WRITE) return;
	this->_timeout = time(NULL) + CON_TIMEOUT;

	char buffer[BUF_SIZE];
	buffer[0] = '\0';
	ssize_t cnt = this->_response->getData(buffer, BUF_SIZE);
	cnt = send(this->_fd, buffer, cnt, 0);
	if (cnt < 0) {
		Logger::error("Connection send error");
		this->_state = CONSTATE_DONE;
	} else {
		// Logger::debug("Send %d bytes to descriptor %d", cnt, this->_fd);
		Logger::trace("%s\n---", buffer);
		if (cnt < BUF_SIZE - 1) {
			Logger::info("Response to descriptor %d sent", this->_fd);
			this->_closeTransaction();
		}
	}
}

void Connection::_closeTransaction() {
	if (this->_request)
		delete this->_request;
	this->_request = NULL;
	if (this->_response)
		delete this->_response;
	this->_response = NULL;
	this->_state = CONSTATE_EMPTY;
}
