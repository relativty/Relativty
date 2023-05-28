#pragma once
#include <exception>
#include <string>

#ifdef WIN32
#ifndef _WINDOWS_
//reduce the scope of the import to avoid conflicts
//it seems that windows headers are not the best or i don't know what im doing
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif
#endif

class Serial {
#ifdef WIN32
	HANDLE port_handle;
#else
	int port_fd;
#endif

	const int m_baudrate;
	const std::string m_port;

public:
	Serial(const std::string port, int baudrate);
	~Serial();
	//you can't have twice the same port open so making a copy doesn't make sens
	Serial(Serial&) = delete;
	void reconnect();
	void read(void * buffer, unsigned int size) const;
};

class serial_exception: std::exception {
	const std::string m_message;
public:
	serial_exception(const std::string& message): m_message(message) {};
	serial_exception(serial_exception &&) = default;
	serial_exception(const serial_exception &) = default;
	~serial_exception() = default;
	inline virtual const char* what() { return m_message.c_str(); };
};
