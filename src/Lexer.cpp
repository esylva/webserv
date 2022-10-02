#include "Lexer.hpp"
#include <typeinfo>

Lexer::Lexer(){}

Lexer::~Lexer(){}

void	Lexer::handleLine(const std::string &line){
	size_t prevPosition = 0;
	size_t currentPosition;
	std::string tokenString;

	currentPosition = line.find_first_not_of(_whiteSpase, prevPosition);
	if (currentPosition == std::string::npos) {
		Logger::exitMessage("Empty config file");
	}
	while (currentPosition != std::string::npos) {
		prevPosition = line.find_first_not_of(_whiteSpase, currentPosition);
		if (line[prevPosition] == '#') {
			currentPosition = line.find_first_of("\n", prevPosition);
		}
		else {
				currentPosition = line.find_first_of(_whiteSpase, prevPosition);
		}

	// получаем токены	
		if (prevPosition != std::string::npos) {
			tokenString = line.substr(prevPosition, currentPosition - prevPosition);
			tokenize(tokenString.c_str());
		}
	}

	// разделение токенов по серверам
	for (std::vector<TokenPair>::iterator i  = _tokens.begin(); i != _tokens.end(); i++) {
		switch ((*i).second) {
			case HASHTAG :
				continue;
				break;
			case WORD :
				if ((*i).first == "server") {
					if ((*++i).second != OPEN_BRACKET) {
						Logger::exitMessage("Config file syntax error");
					}
					_serverTokens.clear();
					_bracketStack = 1;
					_serverTokens.push_back(*i);
					i++;
					while (_bracketStack != 0) {
						switch ((*i).second) {
							case HASHTAG :
								break;
							case OPEN_BRACKET :
								_bracketStack++;
								_serverTokens.push_back(*i);
								break;
							case CLOSE_BRACKET :
								_bracketStack--;
								_serverTokens.push_back(*i);
								break;
							default:
								_serverTokens.push_back(*i);
						}
						if(_bracketStack) 
							i++;
						else {
							_splittedTokens.push_back(_serverTokens);
						}
					}
				}
				else {
					Logger::exitMessage("Keyword 'server' not found");
				}
				break;
			default:
				continue;
		}
	}
}

void	Lexer::tokenize(const char *tokenString) {
	TokenPair tmp;
	switch (tokenString[0]) {
		case '{':
			if (tokenString[1] == 0) {
				tmp.first = tokenString;
				tmp.second = OPEN_BRACKET;				
			}
			else {
				Logger::exitMessage("Config file syntax error");
			}
			break;
		case '}':
			if (tokenString[1] == 0) {
			tmp.first = tokenString;
			tmp.second = CLOSE_BRACKET;
			}
			else {
				Logger::exitMessage("Config file syntax error");
			}
			break;
		case '#':
			tmp.first = tokenString;
			tmp.second = HASHTAG;
			break;
		case ';':
			if (tokenString[1] == 0) {
			tmp.first = tokenString;
			tmp.second = SEMICOLON;
			}
			else {
				Logger::exitMessage("Config file syntax error");
			}
			break;
		default:
			tmp.first = tokenString;
			tmp.second = WORD;
	}
	if (tmp.second == WORD) {
		size_t semicolon = tmp.first.find_first_of(";");
		if (semicolon != std::string::npos) {
			if (semicolon != tmp.first.length() - 1) {
				Logger::exitMessage("Config file syntax error");
			}
			tmp.first = tmp.first.substr(0, semicolon);
			_tokens.push_back(tmp);
			tmp.first = ";";
			tmp.second = SEMICOLON;
		}
	}
	_tokens.push_back(tmp);
}

std::vector<Server*> Lexer::parse_server(void) {


	for (std::vector<std::vector<TokenPair> >::iterator i  = _splittedTokens.begin(); i != _splittedTokens.end(); i++) {
		_serverTokens = *i;

		std::vector<Location *> *tmpLocVector = new std::vector<Location *>;
		Server *tmp = new Server();

		for (std::vector<TokenPair>::iterator j  = _serverTokens.begin(); j != _serverTokens.end(); j++) {
			if ((*j).second == WORD) {
				if ((*j).first == "location") {
					Location *tmpLoc = new Location();
					j++;
					if ((*j).second == WORD){
						tmpLoc->setPath((*j).first);
						j++;
					}
					if ((*j).second == OPEN_BRACKET){
						_bracketStack++;
						j++;
					}
					else {
						Logger::exitMessage("Config file syntax error");
					}
					while (_bracketStack != 0) {
						switch ((*j).second) {
							case WORD :
								if ((*(j + 1)).second != WORD) {
									Logger::exitMessage("Config file syntax error");
								}
								if ((*j).first == "root") {
									j++;
									tmpLoc->setRoot((*j).first);
								}
								else if ((*j).first == "index") {
									j++;
									tmpLoc->setIndex((*j).first);
								}
								else if ((*j).first == "cgi_info") {
									j++;
									tmpLoc->setCgiInfo((*j).first);
								}
								else if ((*j).first == "redirection") {
									j++;
									tmpLoc->setRedirection((*j).first);
								}														
								else if ((*j).first == "client_max_body_size") {
									j++;
									int bodySize = atoi((*j).first.c_str()); 
									tmpLoc->setBodySize(bodySize);
								}
								else if ((*j).first == "autoindex") {
									j++;
									tmpLoc->setAutoindex ("on" ? true : false);
								}									
								else if ((*j).first == "allow_methods") {
									while (((*(j + 1)).second == WORD) && ((*(j + 1)).second != SEMICOLON)) {
										j++;
										tmpLoc->addMethod((*j).first);										
									}
								}
								else {
									Logger::exitMessage("Unknown keyword in config file");
								}
								if ((*(j + 1)).second != SEMICOLON) {
									Logger::exitMessage("Config file syntax error");
								}
								j++;
								break;
							case CLOSE_BRACKET :
								_bracketStack--;
								break;
							default:
								Logger::exitMessage("Config file syntax error");
						}
						if(_bracketStack) 
							j++;
						else {
							tmpLocVector->push_back(tmpLoc);
						}
					}
				}
				else if ((*j).first == "listen") {
					j++;
					parseAddrPort((*j).first, tmp);
				}
				else if ((*j).first == "server_name") {
					j++;
					std::string tmpName;
					while ((*j).second == WORD && (*j).second != SEMICOLON) {
						tmpName = tmpName + " " + (*j).first;
					j++;
					}						
					tmpName.erase(0, 1);
					tmp->setName(tmpName);
				}
				else if ((*j).first == "autoindex") {
					j++;
					tmp->setAutoindex ("on" ? true : false);
				}
				else if ((*j).first == "index") {
					j++;
					tmp->setIndex((*j).first);
				}
				else if ((*j).first == "client_max_body_size") {
					j++;
					int bodySize = atoi((*j).first.c_str()); 
					tmp->setBodySize(bodySize);
				}
				else if ((*j).first == "allow_methods") {
					while (((*(j + 1)).second == WORD) && ((*(j + 1)).second != SEMICOLON)) {
						j++;
						tmp->addMethod((*j).first);										
					}
				}
				else if ((*j).first == "uploads") {
					j++;
					tmp->setUploadPath((*j).first);
				}
				// template
				// else if ((*j).first == "server_name") {
				// 	j++;
				// }

			}
			else {
				continue;
			}
		}

		tmp->setLocation(*tmpLocVector);
		_servers.push_back(tmp);
	}
	return (_servers);
}

void Lexer::parseAddrPort(const std::string &str, Server* server) {
	size_t currentPosition = str.find_first_of(":");
	if (currentPosition == std::string::npos) {
		Logger::exitMessage("Syntax error: port error");
	}
	server->setAddr(str.substr(0, currentPosition));
	int port = atoi(str.substr(currentPosition + 1).c_str());
	if ((port != 80 && port < 1000) || port > 65535)
		Logger::exitMessage("Wrong port number");
	server->setPort(port);
}


/*
user  www www;

worker_processes  2;

pid /var/run/nginx.pid;

#                          [ debug | info | notice | warn | error | crit ]

error_log  /var/log/nginx.error_log  info;

events {
    worker_connections   2000;

    # use [ kqueue | epoll | /dev/poll | select | poll ];
    use kqueue;
}

http {

    include       conf/mime.types;
    default_type  application/octet-stream;


    log_format main      '$remote_addr - $remote_user [$time_local] '
                         '"$request" $status $bytes_sent '
                         '"$http_referer" "$http_user_agent" '
                         '"$gzip_ratio"';

    log_format download  '$remote_addr - $remote_user [$time_local] '
                         '"$request" $status $bytes_sent '
                         '"$http_referer" "$http_user_agent" '
                         '"$http_range" "$sent_http_content_range"';

    client_header_timeout  3m;
    client_body_timeout    3m;
    send_timeout           3m;

    client_header_buffer_size    1k;
    large_client_header_buffers  4 4k;

    gzip on;
    gzip_min_length  1100;
    gzip_buffers     4 8k;
    gzip_types       text/plain;

    output_buffers   1 32k;
    postpone_output  1460;

    sendfile         on;
    tcp_nopush       on;
    tcp_nodelay      on;
    send_lowat       12000;

    keepalive_timeout  75 20;

    #lingering_time     30;
    #lingering_timeout  10;
    #reset_timedout_connection  on;


    server {
        listen        one.example.com;
        server_name   one.example.com  www.one.example.com;

        access_log   /var/log/nginx.access_log  main;

        location / {
            proxy_pass         http://127.0.0.1/;
            proxy_redirect     off;

            proxy_set_header   Host             $host;
            proxy_set_header   X-Real-IP        $remote_addr;
            #proxy_set_header  X-Forwarded-For  $proxy_add_x_forwarded_for;

            client_max_body_size       10m;
            client_body_buffer_size    128k;

            client_body_temp_path      /var/nginx/client_body_temp;

            proxy_connect_timeout      70;
            proxy_send_timeout         90;
            proxy_read_timeout         90;
            proxy_send_lowat           12000;

            proxy_buffer_size          4k;
            proxy_buffers              4 32k;
            proxy_busy_buffers_size    64k;
            proxy_temp_file_write_size 64k;

            proxy_temp_path            /var/nginx/proxy_temp;

            charset  koi8-r;
        
			add_header 'Access-Control-Allow-Credentials' 'true';
			add_header 'Access-Control-Allow-Methods' 'GET, POST, OPTIONS';
        }

        error_page  404  /404.html;

        location = /404.html {
            root  /spool/www;
        }

        location /old_stuff/ {
            rewrite   ^/old_stuff/(.*)$  /new_stuff/$1  permanent;
        }

        location /download/ {

            valid_referers  none  blocked  server_names  *.example.com;

            if ($invalid_referer) {
                #rewrite   ^/   http://www.example.com/;
                return   403;
            }

            #rewrite_log  on;

            # rewrite /download/ * / mp 3/ *.any_ext to /download/ * /mp3/ *.mp3
            rewrite ^/(download/.*)/mp3/(.*)\..*$
                    /$1/mp3/$2.mp3                   break;

            root         /spool/www;
            #autoindex    on;
            access_log   /var/log/nginx-download.access_log  download;
        }

        location ~* \.(jpg|jpeg|gif)$ {
            root         /spool/www;
            access_log   off;
            expires      30d;
        }
    }
}
*/