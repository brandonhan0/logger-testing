#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <filesystem>
#include "uart.hpp"

// ADAPTED FROM: https://github.com/sckulkarni246/ke-rpi-samples/tree/main/uart-c-termios

/* TODO --------------------------------------------------
- gotta get a real buff size (?)
- gotta make it object-oriented
*/

// maps to hardware on pi that's able to work at 1MBaud/s
//#define SERIAL_PORT_PATH                "/dev/ttyAMA0"

// maps to pi hardware that works at 1MBaud/s but it's clock isn't as stable
#define SERIAL_PORT_PATH		"/dev/ttyS0"

// globals
struct termios uart_tty;
int uart_fd;
int log_file_fd;

// SERIAL PORT  --------------------------------------------------
// opens and configures serial port for UART
static void init_serial_port(void){
    uart_fd = open(SERIAL_PORT_PATH, O_RDWR | O_NONBLOCK);
    if(uart_fd < 0){
        perror("Couldn't open UART terminal interface\n");
        exit(-1);
    }

    if(tcgetattr(uart_fd, &uart_tty) != 0) {
        printf("Something went wrong while getting port attributes...\r\n");
        exit(EXIT_FAILURE);
    }

    // set i/o speed
    cfsetispeed(&uart_tty,B1000000);
    cfsetospeed(&uart_tty,B1000000);

    cfmakeraw(&uart_tty);

    // Explicit 8-N-1, receiver enabled, no hardware flow control.
    uart_tty.c_cflag &= ~PARENB;      // no parity
    uart_tty.c_cflag &= ~CSTOPB;      // one stop bit
    uart_tty.c_cflag &= ~CSIZE;
    uart_tty.c_cflag |= CS8;          // eight data bits
    uart_tty.c_cflag |= CLOCAL | CREAD;
    uart_tty.c_cflag &= ~CRTSCTS;     // no RTS/CTS flow control

    // wait until we receive at least 1 byte
    uart_tty.c_cc[VMIN]  = 1;
    // no timeout
    uart_tty.c_cc[VTIME] = 0;

    if(tcsetattr(uart_fd, TCSANOW, &uart_tty) != 0) {
        printf("Something went wrong while setting port attributes...\r\n");
        exit(EXIT_FAILURE);
    }
}

static void close_serial_port(void) {
    // todo: use cfsetospeed(uart_tty, 0) to close connection?
    close(uart_fd);
}


// RUNNIN' ---------------------------------------------
static int run_demo(){
    uint8_t l_buff[256];
    uint32_t l_len_buff = 256;
    uint32_t l_looper;
    
    // simple buffer with 0-256 in each byte
    for(l_looper=0; l_looper<l_len_buff; ++l_looper)
        l_buff[l_looper] = l_looper;

    // write to tx, sleep, clear buff
    write(uart_fd,l_buff,l_len_buff);
    sleep(1);
    memset(l_buff,0,l_len_buff);
    // read from rx
    read(uart_fd,l_buff,l_len_buff);
    // print it out
    printf("|");
    for(l_looper=0; l_looper<l_len_buff; ++l_looper) {
        printf(" %02x |",l_buff[l_looper]);
	// data verification
        if(l_buff[l_looper] != l_looper) {
        printf("\r\nSomething went wrong in the loopback data check...%d and %d\r\n",l_buff[l_looper],l_looper) ;
            exit(EXIT_FAILURE);
        }
    }
    printf("\r\nThe data loopback was successful!\r\n");
    return 1;
}



// MAIN --------------------------------------------------
int main(void) {
    printf("Starting...\r\n");

    // init port and file
    printf("Initializing serial port...\n");
    init_serial_port();

    printf("Running...\n");
    run_demo();

    printf("Closing...\n");
    close_serial_port();

    return 0;
}
