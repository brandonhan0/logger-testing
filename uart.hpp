#ifndef UART_HPP
#define UART_HPP

// [baud rate b/s]-[character size in bits]-[Y or N PARITY?]-1 (stop bit)
// ex. 115200-8-N-1

#include <stdint.h>

#define OPEN_FLAGS O_CREAT|S_IRUSR

// file stuff
fs::path get_new_filename(const fs::path& dir_path);
static int open_new_log_file();
static int file_close(int fd);
// serial port stuff
static int open_serial_port(void);
static void configure_serial_port(void);
static void close_serial_port(void);
// run run run
static void run(void);

// main
int main(void);

#endif
