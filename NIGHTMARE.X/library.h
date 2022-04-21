/* 
 * File:   library.h
 * Author: cberthelo007
 *
 * Created on 15 mars 2022, 09:28
 */

#ifndef _UART_LIBRARY
#define _UART_LIBRARY
void begin (int baudRate);
int begin_BL(int master);
int begin_BL_fast(int master);
int RX_available();
int TX_available();
void flush();
int print_UART(char toPrint);
int print_UART_n(char *toPrint, int n);
char read();
void begin_led();
void send_led();
void begin_inter();
void parse_switch(char *buffer,int n);
unsigned char read_inter(int number);
unsigned char read_inters();
void write_led(unsigned char buffer);


#endif

