#include "Response.hpp"

Response::Response() {
	this->_state = RESSTATE_OPEN;
	this->_status = Response::statusEmpty;
	this->_src = NULL;
	this->_errorContent = "";
	initMIMETypes();
}

std::map<int, HTTPStatus*> Response::_initHTTPStatuses() {
	std::map<int, HTTPStatus*> httpStatuses;
	httpStatuses.insert(std::pair<int, HTTPStatus*>(200, new HTTPStatus(200,
		"OK", "The request is OK.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(400, new HTTPStatus(400,
		"Bad Request", "The request cannot be processed due to a client error, such as malformed request.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(403, new HTTPStatus(403,
		"Forbidden", "The request is valid but is refused because the user does not have appropriate permissions, or because the action is not allowed.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(404, new HTTPStatus(404,
		"Not Found", "The resource could not be found, but may be available in the future.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(414, new HTTPStatus(414,
		"Request-URI Too Long", "The requested URI is too long for the server.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(500, new HTTPStatus(500,
		"Internal Server Error", "General error message for unexpected error conditions.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(501, new HTTPStatus(501,
		"Not Implemented", "The request method is not supported by this server.")));
	httpStatuses.insert(std::pair<int, HTTPStatus*>(505, new HTTPStatus(505,
		"HTTP Version Not Supported", "The requested version of HTTP is not supported.")));
	return httpStatuses;
}

std::map<int, HTTPStatus*> Response::_httpStatuses = Response::_initHTTPStatuses();

Response::~Response() {
	if (this->_src) {
		Logger::debug("Close response file stream %p", this->_src);
		fclose(this->_src);
	}
}

void Response::setStatus(int status) {
	if (this->_httpStatuses.find(status) == this->_httpStatuses.end()) {
		Logger::error("HTTP status %d id used but not defined", status);
		status = 500;
	}
	this->_status = status;
	if (this->_status != 200)
		this->_craftErrorContent();
	Logger::trace("Change response status to %d", status);
}

int Response::getStatus() {
	return this->_status;
}


void Response::initMIMETypes()
{
    mimeTypes["html"] = "text/html; charset=utf-8";
    mimeTypes["htm"] = "text/html; charset=utf-8";
    mimeTypes["shtml"] = "text/html; charset=utf-8";
    mimeTypes["css"] = "text/css";
    mimeTypes["xml"] = "text/xml";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["js"] = "application/javascript";
    mimeTypes["atom"] = "application/atom+xml";
    mimeTypes["rss"] = "application/rss+xml";

    mimeTypes["mml"] = "text/mathml";
    mimeTypes["txt"] = "text/plain; charset=utf-8";
    mimeTypes["jad"] = "text/vnd.sun.j2me.app-descriptor";
    mimeTypes["wml"] = "text/vnd.wap.wml";
    mimeTypes["htc"] = "text/x-component";

    mimeTypes["avif"] = "image/avif";
    mimeTypes["png"] = "image/png";
    mimeTypes["svg"] = "image/svg+xml";
    mimeTypes["svgz"] = "image/svg+xml";
    mimeTypes["tif"] = "image/tiff";
    mimeTypes["tiff"] = "image/tiff";
    mimeTypes["wbmp"] = "image/vnd.wap.wbmp";
    mimeTypes["webp"] = "image/webp";
    mimeTypes["ico"] = "image/x-icon";
    mimeTypes["jng"] = "image/x-jng";
    mimeTypes["bmp"] = "image/x-ms-bmp";

    mimeTypes["woff"] = "font/woff";
    mimeTypes["woff2"] = "font/woff2";

    mimeTypes["jar"] = "application/java-archive";
    mimeTypes["war"] = "application/java-archive";
    mimeTypes["ear"] = "application/java-archive";
    mimeTypes["json"] = "application/json";
    mimeTypes["hqx"] = "application/mac-binhex40";
    mimeTypes["doc"] = "application/msword";
    mimeTypes["pdf"] = "application/pdf";
    mimeTypes["ps"] = "application/postscript";
    mimeTypes["eps"] = "application/postscript";
    mimeTypes["ai"] = "application/postscript";
    mimeTypes["rtf"] = "application/rtf";
    mimeTypes["m3u8"] = "application/vnd.apple.mpegurl";
    mimeTypes["kml"] = "application/vnd.google-earth.kml+xml";
    mimeTypes["kmz"] = "application/vnd.google-earth.kmz";
    mimeTypes["xls"] = "application/vnd.ms-excel";
    mimeTypes["eot"] = "application/vnd.ms-fontobject";
    mimeTypes["ppt"] = "application/vnd.ms-powerpoint";
    mimeTypes["odg"] = "application/vnd.oasis.opendocument.graphics";
    mimeTypes["odp"] = "application/vnd.oasis.opendocument.presentation";
    mimeTypes["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    mimeTypes["odt"] = "application/vnd.oasis.opendocument.text";
    mimeTypes["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimeTypes["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeTypes["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeTypes["wmlc"] = "application/vnd.wap.wmlc";
    mimeTypes["wasm"] = "application/wasm";
    mimeTypes["7z"] = "application/x-7z-compressed";
    mimeTypes["cco"] = "application/x-cocoa";
    mimeTypes["jardiff"] = "application/x-java-archive-diff";
    mimeTypes["jnlp"] = "application/x-java-jnlp-file";
    mimeTypes["run"] = "application/x-makeself";
    mimeTypes["pl"] = "application/x-perl";
    mimeTypes["pm"] = "application/x-perl";
    mimeTypes["prc"] = "application/x-pilot";
    mimeTypes["pdb"] = "application/x-pilot";
    mimeTypes["rar"] = "application/x-rar-compressed";
    mimeTypes["rpm"] = "application/x-redhat-package-manager";
    mimeTypes["sea"] = "application/x-sea";
    mimeTypes["swf"] = "application/x-shockwave-flash";
    mimeTypes["sit"] = "application/x-stuffit";
    mimeTypes["tcl"] = "application/x-tcl";
    mimeTypes["tk"] = "application/x-tcl";
    mimeTypes["der"] = "application/x-x509-ca-cert";
    mimeTypes["pem"] = "application/x-x509-ca-cert";
    mimeTypes["crt"] = "application/x-x509-ca-cert";
    mimeTypes["xpi"] = "application/x-xpinstall";
    mimeTypes["xhtml"] = "application/xhtml+xml";
    mimeTypes["xspf"] = "application/xspf+xml";
    mimeTypes["zip"] = "application/zip";

    mimeTypes["bin"] = "application/octet-stream";
    mimeTypes["exe"] = "application/octet-stream";
    mimeTypes["dll"] = "application/octet-stream";
    mimeTypes["deb"] = "application/octet-stream";
    mimeTypes["dmg"] = "application/octet-stream";
    mimeTypes["iso"] = "application/octet-stream";
    mimeTypes["img"] = "application/octet-stream";
    mimeTypes["msi"] = "application/octet-stream";
    mimeTypes["msp"] = "application/octet-stream";
    mimeTypes["msm"] = "application/octet-stream";

    mimeTypes["mid"] = "audio/midi";
    mimeTypes["midi"] = "audio/midi";
    mimeTypes["kar"] = "audio/midi";
    mimeTypes["mp3"] = "audio/mpeg";
    mimeTypes["ogg"] = "audio/ogg";
    mimeTypes["m4a"] = "audio/x-m4a";
    mimeTypes["ra"] = "audio/x-realaudio";

    mimeTypes["3gpp"] = "video/3gpp";
    mimeTypes["3gp"] = "video/3gpp";
    mimeTypes["ts"] = "video/mp2t";
    mimeTypes["mp4"] = "video/mp4";
    mimeTypes["mpeg"] = "video/mpeg";
    mimeTypes["mpg"] = "video/mpeg";
    mimeTypes["mov"] = "video/quicktime";
    mimeTypes["webm"] = "video/webm";
    mimeTypes["flv"] = "video/x-flv";
    mimeTypes["m4v"] = "video/x-m4v";
    mimeTypes["mng"] = "video/x-mng";
    mimeTypes["asx"] = "video/x-ms-asf";
    mimeTypes["asf"] = "video/x-ms-asf";
    mimeTypes["wmv"] = "video/x-ms-wmv";
    mimeTypes["avi"] = "video/x-msvideo";
}


void Response::addHeader(std::string name, std::string value) {
	this->_headers[name] = value;
}

void Response::_craftErrorContent() {
	if (this->_httpStatuses[this->_status] == NULL) {
		this->setStatus(this->_status);
	}
	this->_errorContent = "";
	this->_errorContent = this->_errorContent +
		"<!DOCTYPE html>" +
		"<html>" +
		"<head><title>" + this->_httpStatuses[this->_status]->getCode() + "</title></head>" +
		"<body>" +
		"<h1>" + this->_httpStatuses[this->_status]->getCode() + "</h1>" +
		"<h2>" + this->_httpStatuses[this->_status]->getTitle() + "</h2>" +
		"<p>" + this->_httpStatuses[this->_status]->getDescription() + "</p>" +
		"</body>" +
		"</html>\r\n";
}

void Response::setSource(std::string path, Location* location) {
	if (location == NULL) {
		this->setStatus(404);
		return;
	}
	
	std::string fullPath = location->getFullPath(path);
	if (path.find(".")) {
		this->_contentType = mimeTypes[path.substr(path.find(".") + 1)];
	}
	else {
		this->_contentType = mimeTypes["html"];
	}
	
	Logger::debug("Fullpath for %s is %s", path.c_str(), fullPath.c_str());
	struct stat fileStat;
	if (stat(fullPath.c_str(), &fileStat) != 0) {
		Logger::warning("Cannot read stat for file %s", fullPath.c_str());
		this->setStatus(404);
		return;
	} else if (S_ISDIR(fileStat.st_mode)) {
		Logger::warning("Cannot read directory %s", fullPath.c_str());
		this->setStatus(404);
		return;
	}
	this->_src = fopen(fullPath.c_str(), "rb");
	if (this->_src == NULL) {
		Logger::warning("Cannot open file %s", fullPath.c_str());
		this->setStatus(403);
		return;
	}
	this->setStatus(200);
	Logger::debug("Open response file stream %p for %s", this->_src, fullPath.c_str());
}

size_t Response::getContentLength() {
	size_t len;
	if (this->_status == 200) {
		fseek(this->_src, 0, SEEK_END);
		len = ftell(this->_src);
		rewind(this->_src);
	} else {
		len = this->_errorContent.size();
	}
	return len;
}

ssize_t Response::getData(char* buf, size_t capacity) {
	size_t len = 0;
	switch (this->_state) {
		case RESSTATE_OPEN:
			this->_craftStatus();
			this->_state = RESSTATE_STATUS;
			// fall through
		case RESSTATE_STATUS:
			len += this->_copyBuffer(buf + len, capacity - len);
			if (len < capacity - 1)
				this->_state = RESSTATE_HEAD;
			else
				break;
			// fall through
		case RESSTATE_HEAD:
			len += this->_copyBuffer(buf + len, capacity - len);
			while (len < capacity - 1 && this->_headers.size()) {
				this->_craftHeader();
				len += this->_copyBuffer(buf + len, capacity - len);
			}
			if (len < capacity - 1 && this->_headers.size() == 0)
				this->_state = RESSTATE_BODY;
			else
				break;
			// fall through
		case RESSTATE_BODY:
			len += this->_copyContent(buf + len, capacity - len);
			if (len < capacity - 1)
				this->_state = RESSTATE_DONE;
			else
				break;
			// fall through
		case RESSTATE_DONE:
			// fall through
		default:;
	}
	return len;
}

void Response::_craftStatus() {
	this->_buf = "";
	this->_buf = this->_buf + "HTTP/1.1 " +
		this->_httpStatuses[this->_status]->getCode() + " " +
		this->_httpStatuses[this->_status]->getTitle() + "\r\n";
}

void Response::_craftHeader() {
	this->_buf = this->_buf + this->_headers.begin()->first + ": " + this->_headers.begin()->second + "\r\n";	
	this->_headers.erase(this->_headers.begin());
	if (this->_headers.size() == 0)
		this->_buf += "\r\n";
}

size_t Response::_copyBuffer(char* buf, size_t capacity) {
	size_t len = this->_buf.size();
	if (len > capacity - 1)
		len = capacity - 1;
	strncpy(buf, this->_buf.c_str(), len);
	buf[len] = '\0';
	this->_buf.erase(0, len);
	return len;
}

size_t Response::_copyContent(char* buf, size_t capacity) {
	size_t len;
	if (this->_status == 200) {
		len = fread(buf, 1, capacity - 1, this->_src);
	} else {
		len = this->_errorContent.size();
		if (len > capacity - 1)
			len = capacity - 1;
		strncpy(buf, this->_errorContent.c_str(), len);
		this->_errorContent = this->_errorContent.substr(len);
	}
	buf[len] = '\0';
	return len;
}

std::string	Response::getContentType() {
	return this->_contentType;
}

// std::string Response::getMIME() const {
//     // std::string key = fileType;
//     // std::string mime = "text/html; charset=utf-8";
//     // for (std::map<std::string, std::string>::const_iterator it = mimeTypes.begin(); it != mimeTypes.end(); it++)
//     // {
//     //     if (fileType == it->first)
//     //     {
//     //         mime = it->second;
//     //     }
//     // }
//     // return mime;
// }