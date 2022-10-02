#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <exception>
#include "Logger.hpp"
#include "Server.hpp"

enum TOKENS {
    OPEN_BRACKET,
    CLOSE_BRACKET,
    HASHTAG,
    WORD,
    SEMICOLON,
};

typedef std::pair<std::string, TOKENS> TokenPair;
const std::string _whiteSpase = "\n\t\r\v\f ";

class Lexer {
	private:
		std::vector<Server*>							_servers;

		std::vector<TokenPair>							_tokens;
		std::vector<TokenPair>							_serverTokens;
		std::vector<std::vector<TokenPair> >			_splittedTokens; 
		int												_bracketStack;

		void						tokenize(const char *tokenString);
       	std::vector<Location*>		parse_location(void);
		void parseAddrPort(const std::string &str, Server *server);

	public:
		Lexer(void);
		Lexer(const Lexer &instance);

		Lexer  &operator=(const Lexer &rhs);

		void						handleLine(const std::string &line);
		std::vector<Server*>		parse_server(void);

		~Lexer(void);
};
