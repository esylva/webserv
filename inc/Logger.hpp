#pragma once

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdarg>

#define LOG_LEVEL DEBUG

typedef enum {
	TRACE = 0,
	DEBUG = 1,
	INFO = 	2,
	WARNING = 3,
	ERROR = 4,
	NONE = 6
} t_loglevel;

class Logger {
private:
public:
	static void exitMessage(const char* format, ...);
	static void exitFatal(const char* format, ...);
	static void trace(const char* format, ...);
	static void debug(const char* format, ...);
	static void info(const char* format, ...);
	static void warning(const char* format, ...);
	static void error(const char* format, ...);
	static void printTime();
};
