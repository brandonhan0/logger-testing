#include "uart.h"
// ADAPTED FROM: https://github.com/sckulkarni246/ke-rpi-samples/tree/main/uart-c-termios

// globals
struct termios uart_tty;
int uart_fd = -1;

// SERIAL PORT  --------------------------------------------------
// opens and configures serial port for UART
void uart::init(void){
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

    // defining terminal control modes
    uart_tty.c_cflag &= ~PARENB;      // no parity
    uart_tty.c_cflag &= ~CSTOPB;      // one stop bit
    uart_tty.c_cflag &= ~CSIZE;       // character size mask
    uart_tty.c_cflag |= CS8;          // eight data bits = character
    uart_tty.c_cflag |= CLOCAL | CREAD; // ignore modem control lines, enable receiver
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

ssize_t uart::send(void* buff, size_t buff_len){
    ssize_t bytes_write = write(uart_fd, (const void*)buff, buff_len);
    if(bytes_write == -1){
        printf("Error on uart_write() call!\n");
        return -1;
    }
    return bytes_write;
}

ssize_t uart::receive(void* buff, size_t buff_len){
    ssize_t bytes_read = read(uart_fd, (void*)buff, buff_len);
    if(bytes_read == -1){
        printf("Error on uart_read() call!\n");
        return -1;
    }
    return bytes_read;
}

void uart::terminate(void) {
    // todo: use cfsetospeed(uart_tty, 0) to close connection?
    close(uart_fd);
}

// RUNNIN' ---------------------------------------------
static int run_demo(uart uart_o){
    uint8_t l_buff[256];
    uint32_t l_len_buff = 256;
    uint32_t l_looper;
    
    // simple buffer with 0-256 in each byte
    for(l_looper=0; l_looper<l_len_buff; ++l_looper)
        l_buff[l_looper] = l_looper;

    // write to tx, sleep, clear buff
    uart_o.send(l_buff, l_len_buff);
    sleep(1);
    memset(l_buff,0,l_len_buff);
    // read from rx
    uart_o.receive(l_buff, l_len_buff);
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
    uart uart_o;
    printf("Starting...\r\n");

    // init port and file
    printf("Initializing serial port...\n");
    uart_o.init();

    printf("Running...\n");
    run_demo(uart_o);

    printf("Closing...\n");
    uart_o.terminate();

    return 0;
}