#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <filesystem>
#include "/home/path/to/file/uart.hpp"

// ADAPTED FROM: https://github.com/sckulkarni246/ke-rpi-samples/tree/main/uart-c-termios

/* TODO --------------------------------------------------
- gotta get a real buff size
- gotta make it so that we make a new file with new name every init (eric/brandon question)
- scroll to bottom of /boot/firmware/config.txt and add "enable_uart=1"
- address mini todos below
*/

// maps to hardware on pi that's able to work at 1MBaud/s
#define SERIAL_PORT_PATH                "/dev/ttyAMA0"
// file/dir paths
#define LOGGER_DATA_DIRECTORY_PATH      "/home/path/to/directory/"
#define TEST_LOG_FILE_PATH              "/home/path/to/file"
// globals
struct termios uart_tty;
int uart_fd;
int log_file_fd;


// FILE OPERATION --------------------------------------------------
namespace fs = std::filesystem;
// UNFINISHED
fs::path get_new_filename(const fs::path& dir_path){
    fs::path latest_file;

    // check directory existence
    if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
        return latest_file; // Returns empty path
    }

    // iterate through everything in directory
    bool first = true;
    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            // search for certain characteristic
        }
    }
}

static int open_new_log_file(){
    // // todo: make a new file name and path
    // fs::path logger_file = get_new_filename((const fs::path&)LOGGER_DATA_DIRECTORY_PATH);
    // const char* filename = logger_file.filename().c_str();

    // if(open(filename, OPEN_FLAGS) < 0){
    //     printf("Could not open file %s\r\n",filename);
    //     return -1;
    // }
    // return 1;

    // test config
    log_file_fd = open(TEST_LOG_FILE_PATH, OPEN_FLAGS);
    if(log_file_fd < 0){ 
        printf("Couldn't open test file\n");
        exit(-1);
    }

    return 1;
}

static int file_close(int fd) {
    return close(fd);
}

// SERIAL PORT  --------------------------------------------------
static int open_serial_port(void) {
    uart_fd = open(SERIAL_PORT_PATH, O_RDWR | O_NONBLOCK);
    if(uart_fd < 0){
        perror("Couldn't open UART terminal interface\n");
        exit(-1);
    }
    return 1;
}

static void configure_serial_port(void) {
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
    file_close(uart_fd);
}


// RUNNIN' --------------------------------------------------
// todo: can probably find a better way to handle read/write errors and exits
static void run(void) {
    uint8_t r_buff[256];
    uint32_t r_buff_len = 256;
    uint32_t r_looper = 0;
    ssize_t bytes_read = 0;
    ssize_t bytes_written = 0;
    
    // reads enough data to fill the buffer or less
    while(bytes_read = read(uart_fd,r_buff,r_buff_len)){
        if(bytes_read < 0){
            // probably a reading error
            perror("Error while reading UART\n");
            exit(-1);
        }
        // write to log file
        if((bytes_written = write(log_file_fd, r_buff, bytes_read)) < 0){
            perror("Error while writing to log file!\n");
            exit(-1);
        }
        // error check
        if(bytes_written != bytes_read){
            perror("Error while writing\n");
            exit(-1);
        }
    }

}

// MAIN --------------------------------------------------
int main(void) {
    printf("Starting the loopback application...\r\n");

    // init port and file
    open_serial_port();
    configure_serial_port();
    open_new_log_file();

    run();

    close_serial_port();

    return 0;
}