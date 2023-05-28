#include "openvr_driver.hpp"
#include "Serial.hpp"

HANDLE open_handle(const char* port, int baudrate);
void Serial::read(void* buffer, unsigned int size) const {
	if (!ReadFile(port_handle, buffer, size, NULL, NULL)) {
		throw serial_exception("Read failed");
	}
}

void Serial::reconnect() {
	CloseHandle(this->port_handle);
	port_handle = open_handle(m_port.c_str(), m_baudrate);
}

Serial::~Serial() {
	CloseHandle(this->port_handle);
}

Serial::Serial(const std::string port, int baudrate): m_baudrate(baudrate), m_port(port) {
	port_handle = open_handle(port.c_str(), baudrate);
}

HANDLE open_handle(const char * port, int baudrate) {
	HANDLE port_handle = CreateFile(
		port,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);

	if (port_handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			throw serial_exception("Serial port not found");
		}
		else {
			throw serial_exception("Could not open the serial port");
		}
	}

	DCB dcbSerialParams = { 0 };
	COMMTIMEOUTS timeouts = { 0 };

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(port_handle, &dcbSerialParams)) {
		CloseHandle(port_handle);
		throw serial_exception("Could not get the serial port configuration");
	}

	dcbSerialParams.BaudRate = baudrate;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = EVENPARITY;
	if (!SetCommState(port_handle, &dcbSerialParams)) {
		CloseHandle(port_handle);
		throw serial_exception("Could not change the serial port configuration");
	}

	const int timeout = 100;
	timeouts.ReadIntervalTimeout = timeout;
	timeouts.ReadTotalTimeoutConstant = timeout;
	timeouts.ReadTotalTimeoutMultiplier = 1;
	timeouts.WriteTotalTimeoutConstant = timeout;
	timeouts.WriteTotalTimeoutMultiplier = 1;
	if (!SetCommTimeouts(port_handle, &timeouts)) {
		throw serial_exception("Could not change the serial port timeout configuration");
		CloseHandle(port_handle);
	}

	return port_handle;
}
