#pragma once
#ifndef _WINDOWS_
//reduce the scope of the import to avoid conflicts
//it seems that windows headers are not the best or u don't know what im doing
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

class Serial {
#ifdef WIN32
	HANDLE port_handle;
#else
	int port_fd;
#endif

	const std::string m_port;
	const int m_baudrate;

public:
	Serial(const std::string port, int baudrate);
	~Serial();
	//you can't have twice the same port open so making a copy doesn't make sens
	Serial(Serial&) = delete;
	void reconnect();
	void read(void * buffer, unsigned int size) const;
};