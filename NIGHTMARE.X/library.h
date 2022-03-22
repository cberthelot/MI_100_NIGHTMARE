/* 
 * File:   library.h
 * Author: cberthelo007
 *
 * Created on 15 mars 2022, 09:28
 */

#ifndef _UART_LIBRARY
#define _UART_LIBRARY
void begin (int baudRate);
int RX_available();
int TX_available();
void flush();
int print_UART(char toPrint);
char read();
void begin_led();
void begin_inter();
unsigned char read_inter(int number);
unsigned char read_inters();
void write_led(unsigned char buffer);


#endif

