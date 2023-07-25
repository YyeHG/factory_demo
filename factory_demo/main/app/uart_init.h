#ifndef _UART_INIT_H_
#define _UART_INIT_H_
#include "driver/uart.h"

#define BaudRate	115200
#define RX_BUFFER	1024
#define TX_BUFFER	1024

#define UART_PORT_NUM		UART_NUM_1
#define RTS			UART_PIN_NO_CHANGE
#define CTS			UART_PIN_NO_CHANGE

void uart_init();
void rec_buf();
#endif
