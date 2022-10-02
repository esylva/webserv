#include "Logger.hpp"

void Logger::exitMessage(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	std::cerr << std::endl;
	va_end(args);
	exit(EXIT_FAILURE);
}

void Logger::exitFatal(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::cerr << "Error: ";
	vfprintf(stderr, format, args);
	std::cerr << std::endl;
	va_end(args);
	exit(EXIT_FAILURE);
}

void Logger::trace(const char* format, ...) {
	if (LOG_LEVEL <= TRACE) {
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		std::cout << std::endl;
		va_end(args);
	}
}

void Logger::debug(const char* format, ...) {
	if (LOG_LEVEL <= DEBUG) {
		Logger::printTime();
		std::cout << " - ";
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		std::cout << std::endl;
		va_end(args);
	}
}

void Logger::info(const char* format, ...) {
	if (LOG_LEVEL <= INFO) {
		Logger::printTime();
		std::cout << " i ";
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		std::cout << std::endl;
		va_end(args);
	}
}

void Logger::warning(const char* format, ...) {
	if (LOG_LEVEL <= WARNING) {
		Logger::printTime();
		std::cout << " ? ";
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		std::cout << std::endl;
		va_end(args);
	}
}

void Logger::error(const char* format, ...) {
	if (LOG_LEVEL <= ERROR) {
		Logger::printTime();
		std::cout << " ! ";
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		std::cout << std::endl;
		va_end(args);
	}
}

void Logger::printTime() {
	time_t timestamp = time(NULL);
	int sec = timestamp % 60;
	timestamp /= 60;
	int min = timestamp % 60;
	timestamp /= 60;
	int hour = timestamp % 24;
	printf("%d:%d:%d", hour, min, sec);
}
