#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <filesystem>

// maps to hardware on pi that's able to work at 1 MBaud/s
//#define SERIAL_PORT_PATH                "/dev/ttyAMA0"

// maps to pi hardware that works at 1MBaud/s but its clock isn't as stable
#define SERIAL_PORT_PATH		"/dev/ttyS0"

class uart{
    private:
        int uart_fd;
        struct termios uart_tty;

    public:
        void init(void);
        ssize_t send(void* buff, size_t buff_len);
        ssize_t receive(void* buff, size_t buff_len);
        void terminate(void);

}

#endif