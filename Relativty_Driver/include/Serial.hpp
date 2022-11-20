#include <cstddef>

int open_serial(const char * port, int baudrate);
void serial_close(int port);
/**
 * @brief Read data from the serial port into a buffer might timeout
 *
 * @param port identifier returned by open_serial
 * @return false in case of failure
 */
bool serial_read(int port, void * buffer, unsigned int size);
