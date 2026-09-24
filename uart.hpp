#ifndef UART_HPP
#define UART_HPP

// [baud rate b/s]-[character size in bits]-[Y or N PARITY?]-1 (stop bit)
// ex. 115200-8-N-1

#include <stdint.h>

#define port_name "blahblah"

// file stuff
static int file_open_and_get_descriptor(const char *fname);
static int file_write_data(int fd, uint8_t *buff, uint32_t len_buff);
static int file_read_data(int fd, uint8_t *buff, uint32_t len_buff);
static int file_close(int fd);

// serial port stuff
static void open_serial_port(void);
static void configure_serial_port(void);
static void close_serial_port(void);

// run run run
static void run(void);

// main
int main(void);

#endif
