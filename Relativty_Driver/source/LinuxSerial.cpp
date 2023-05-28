#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "../include/openvr_driver.hpp"
#include "Serial.hpp"

int open_serial(const char* port, int baudrate);

int get_baud(int baud)
{
    switch (baud) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B0;
    }
}

void Serial::read(void* buffer, unsigned int size) const {
	if (::read(port_fd, buffer, size) != size) {
		throw serial_exception("Read failed");
	}
}

void Serial::reconnect() {
	close(port_fd);
	port_fd = open_serial(m_port.c_str(), m_baudrate);
}

Serial::~Serial() {
	close(port_fd);
}

Serial::Serial(const std::string port, int baudrate) : m_baudrate(baudrate), m_port(port) {
	port_fd = open_serial(m_port.c_str(), m_baudrate);
}


int open_serial(const char * port, int baudrate) {
	if(access(port, F_OK) != 0) {
		throw serial_exception("Could not open the serial port");
	}


	//we need write to run tcsetattr
	int port_fd = open(port, O_RDWR);
	if(port_fd < 0) {
		throw serial_exception("Could not open the serial port");
	}

	struct termios settings;

	//read the settings
	if(tcgetattr(port_fd, &settings) != 0) {
		close(port_fd);
		throw serial_exception("Could not get the port settings");
	}

	//enable parity bit (even for odd you need to add PARODD)
	settings.c_cflag |= PARENB;
	//only one stop bit
	settings.c_cflag &= ~CSTOPB;
	//8bits per byte
	settings.c_cflag |= CS8;
	//no control flow
	settings.c_cflag &= ~CRTSCTS;
	//enable read and disable modem specifig features
	settings.c_cflag |= CREAD | CLOCAL;
	//raw read/write
	settings.c_lflag &= ~ICANON;
	//disable echo
	settings.c_lflag &= ~ECHO;
	settings.c_lflag &= ~ECHOE;
	settings.c_lflag &= ~ECHONL;
	//disable interpretation of QUIT INTR & SUSP
	settings.c_lflag &= ~ISIG;
	//disable control flow
	settings.c_iflag &= ~(IXON | IXOFF | IXANY);
	//disable any special handling
	settings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

	//we don't change anything output related since the only write we are going to do is tcsetattr
	// VMIN = 0 VTIME > 0 => blocking read
	// when blocking VTIME is the timeout in déciseconde (0.1 seconds)
	// 0.1 second timeout seems pretty high still but is will allow us to catch disconnections
	settings.c_cc[VTIME] = 1;
	settings.c_cc[VMIN] = 0;

	speed_t baud = get_baud(baudrate);

	if(baud == B0) {
		close(port_fd);
		throw serial_exception("Invalid baud rate");
	}

	if(cfsetispeed(&settings, baud) != 0) {
		close(port_fd);
		throw serial_exception("Could not set baud");
	}

	if(tcsetattr(port_fd, TCSANOW, &settings) != 0) {
		close(port_fd);
		throw serial_exception("Could not set baud");
	}

	return port_fd;
}
